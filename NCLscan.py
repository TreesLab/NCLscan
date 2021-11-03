#! /usr/bin/env python2

import argparse
import sys
import os
import subprocess as sp
import re
from collections import OrderedDict

def NCL_Scan1(config, datasets_list, output_dir):

    '''
        # Mapping and get unmapped reads
        for dataset in (Main_datasets + Support_datasets):
           1. bwa mapping
           2. get bwa unmapped
           3. novoalign mapping
           4. get novoalign unmapped
    '''

    config_options = config.options.copy()
    Run_with_args = Run_cmd(config_options)

    all_datasets = datasets_list.main_datasets.values() + datasets_list.support_datasets.values() # [Dataset_main_1, Dataset_support_1, Dataset_support_2, ...]
    for dataset in all_datasets:
        # bwa mapping
        config_options.update({"fastqs":' '.join(dataset.reads), "prefix":"{}/{}.{}".format(output_dir, dataset.name, dataset.dataset_type)})
        Run_with_args("{bwa_bin} mem {bwa_options} {bwa_index} {fastqs} | {samtools_bin} view -Shb - > {prefix}.bwa.bam")

        # get bwa unmapped
        Run_with_args("{samtools_bin} view -F 2 -f 1 {prefix}.bwa.bam | tee {prefix}.bwa.unmapped.sam | cut -f '1' | sort | uniq > {prefix}.bwa.unmapped.sam.id")
        bwa_unmapped_dataset = []
        for idx, data in enumerate(dataset.reads):
            if re.search("\.gz$", data):
                cat = "zcat"
            else:
                cat = "cat"
            config_options.update({"cat":cat, "fastq":data, "idx":idx+1})
            Run_with_args("{cat} {fastq} | {FastqOut_bin} {prefix}.bwa.unmapped.sam.id 1 > {prefix}.bwa.unmapped_{idx}.fastq")
            bwa_unmapped_dataset.append("{prefix}.bwa.unmapped_{idx}.fastq".format(**config_options))

        # novoalign mapping
        config_options.update({"unmapped_fastqs":' '.join(bwa_unmapped_dataset)})
        Run_with_args("{novoalign_bin} -r A 1 -n 30 -d {novoalign_index} -f {unmapped_fastqs} -o SAM > {prefix}.sam")

        # get novoalign unmapped
        Run_with_args("{samtools_bin} view -S {prefix}.sam | {RmBadMapping_bin} 30 0 {quality_score} | {RmColinearPairInSam_bin} | tee {prefix}.unmapped.sam | cut -f '1' | sort | uniq > {prefix}.unmapped.sam.id")
        for idx, data in enumerate(bwa_unmapped_dataset):
            config_options.update({"fastq":data, "idx":idx+1})
            Run_with_args("cat {fastq} | {FastqOut_bin} {prefix}.unmapped.sam.id 1 > {prefix}.unmapped_{idx}.fastq")
        


def NCL_Scan2(config, datasets_list, project_name, output_dir):

    '''
        # Find candidates from main datasets
        
        Future work: try to make blat 'multi-threading' (use subprocess)
    '''    

    config_options = config.options.copy()
    Run_with_args = Run_cmd(config_options)

    unmmaped_fastas = []
    for dataset in datasets_list.main_datasets.values():
        config_options.update({"prefix":"{}/{}.{}".format(output_dir, dataset.name, dataset.dataset_type)})
        Run_with_args("{AssembleFastq_bin} {prefix}.unmapped_1.fastq {prefix}.unmapped_2.fastq > {prefix}.unmapped.fa")
        unmmaped_fastas.append("{prefix}.unmapped.fa".format(**config_options))

    # merge all unmapped fasta of main datasets
    config_options.update({"unmapped_fastas":' '.join(unmmaped_fastas), "prefix":"{}/{}".format(output_dir, project_name), "output_dir":output_dir})
    Run_with_args("cat {unmapped_fastas} > {prefix}.unmapped.fa")

    Run_with_args("{mp_blat_bin} {Reference_genome} {prefix}.unmapped.fa {prefix}.rG.psl -p {mp_blat_process} --blat_bin {blat_bin} --tmp_path {output_dir}")

    Run_with_args("cat {prefix}.rG.psl | awk 'substr($14,0,2) == \"GL\"' | awk '($1+$3) >= 50' > {prefix}.un.psl")
    Run_with_args("cat {prefix}.rG.psl | {RemoveInList_bin} 10 {prefix}.un.psl 10 > {prefix}.rG.non_un.psl")
    Run_with_args("cat {prefix}.rG.non_un.psl | {PslChimeraFilter_bin} 30 5 > {prefix}.chi0.bed")
    Run_with_args("cat {prefix}.unmapped.fa | {SeqOut_bin} {prefix}.chi0.bed 4 > {prefix}.unmapped.2.fa")

    Run_with_args("{mp_blat_bin} {Protein_coding_transcripts} {prefix}.unmapped.2.fa {prefix}.coding.2.psl -p {mp_blat_process} --blat_bin {blat_bin} --tmp_path {output_dir}")
    Run_with_args("{mp_blat_bin} {lncRNA_transcripts} {prefix}.unmapped.2.fa {prefix}.lncRNA.2.psl -p {mp_blat_process} --blat_bin {blat_bin} --tmp_path {output_dir}")
    Run_with_args("{mp_blat_bin} {Repeat_ChrM} {prefix}.unmapped.2.fa {prefix}.chrM.2.psl -p {mp_blat_process} --blat_bin {blat_bin} --tmp_path {output_dir}")

    Run_with_args("cat {prefix}.coding.2.psl {prefix}.lncRNA.2.psl {prefix}.chrM.2.psl | awk '$12 < 10' | awk '($11-$13) < 10' > {prefix}.colinear.psl")
    Run_with_args("cat {prefix}.chi0.bed | {RemoveInList_bin} 4 {prefix}.colinear.psl 10 > {prefix}.chi.bed")

    max_fragment_size = config_options["max_fragment_size"]
    config_options.update({"extend":int(int(max_fragment_size)/2 + int(max_fragment_size)/4 - 30)})
    Run_with_args("cat {prefix}.chi.bed | {JunctionSite2BED_bin} {Gene_annotation} {max_read_len} {extend} > {prefix}.preJS.bed")

    Run_with_args("cat {prefix}.preJS.bed | awk '{{print $4 \"\\t\" $1 \",\" $2 \",\" $3 \",\" $6 \",\"}}' > {prefix}.preJS.info")
    Run_with_args("cat {prefix}.preJS.info | {AssembleExons_bin} > {prefix}.preJS.info2")
    Run_with_args("cat {prefix}.preJS.info2 | grep \".5p.\" | awk '{{print $1 \"\\t\" $2 \"{{(}})0{{(}})\"}}' > {prefix}.JS.info_1")
    Run_with_args("cat {prefix}.preJS.info2 | grep \".3p.\" > {prefix}.JS.info_2")
    Run_with_args("{AssembleJSeq_bin} {prefix}.JS.info_1 {prefix}.JS.info_2 > {prefix}.JS.info")
    Run_with_args("cat {prefix}.JS.info | {RmRedundance_bin} 2 > {prefix}.JS.cleaned.info")

    Run_with_args("{bedtools_bin} getfasta -fi {Reference_genome} -bed {prefix}.preJS.bed -tab -name -s -fo {prefix}.preJS.seq")
    Run_with_args("cat {prefix}.preJS.seq | {AssembleExons_bin} > {prefix}.preJS.seq2")
    Run_with_args("cat {prefix}.preJS.seq2 | grep \".5p.\" > {prefix}.JS.seq_1")
    Run_with_args("cat {prefix}.preJS.seq2 | grep \".3p.\" > {prefix}.JS.seq_2")
    Run_with_args("{AssembleJSeq_bin} {prefix}.JS.seq_1 {prefix}.JS.seq_2 > {prefix}.JS.seq12")
    Run_with_args("cat {prefix}.JS.seq12 | {RetainInList_bin} 1 {prefix}.JS.cleaned.info 1 > {prefix}.JS.seq")
    Run_with_args("cat {prefix}.JS.seq | awk '{{print \">\" $1 \"\\n\" $2}}' > {prefix}.JS.fa")


    
def NCL_Scan3(config, datasets_list, project_name, output_dir):
    # check if there are other support reads for each candidate.
    
    config_options = config.options.copy()
    Run_with_args = Run_cmd(config_options)

    # find junction-supported candidates (find junc-reads)
    config_options.update({"prefix":"{}/{}".format(output_dir, project_name), "prefix_all":"{}/all.{}".format(output_dir, project_name), "output_dir":output_dir})
    Run_with_args("{novoindex_bin} {prefix}.JS.ndx {prefix}.JS.fa")
    
    if len(datasets_list.support_datasets) > 0:
        datasets = datasets_list.support_datasets.values()
    else:
        datasets = datasets_list.main_datasets.values()

    JS_files = []
    for dataset in datasets:
        for idx in range(1, len(dataset.reads)+1):
            config_options.update({"dataset_prefix": "{}/{}.{}".format(output_dir, dataset.name, dataset.dataset_type), "idx":idx})
            Run_with_args("{novoalign_bin} -r A 1 -g 99 -x 99 -d {prefix}.JS.ndx -f {dataset_prefix}.unmapped_{idx}.fastq -o SAM > {dataset_prefix}_{idx}.JS.sam")
            JS_files.append("{dataset_prefix}_{idx}.JS.sam".format(**config_options))

    config_options.update({"JS_files":' '.join(JS_files)})
    Run_with_args("cat {JS_files} > {prefix_all}.JS.sam")
    Run_with_args("cat {prefix_all}.JS.sam | {JSParser_bin} 95 10 10 > {prefix_all}.JS.Parsered.sam")
    Run_with_args("cat {prefix}.JS.cleaned.info | {RetainInList_bin} 1 {prefix_all}.JS.Parsered.sam 3 > {output_dir}/tmp.info")
    Run_with_args("{GetInfo_bin} {output_dir}/tmp.info > {prefix}.info")

    # Preform more sensitive blat alignment
    Run_with_args("cat {prefix}.JS.fa | {SeqOut_bin} {prefix}.info 1 > {prefix}.JS.clear.fa")
    Run_with_args("{mp_blat_bin} {Reference_genome} {prefix}.JS.clear.fa {prefix}.JS.GRCh37.psl -p {mp_blat_process} --blat_bin {blat_bin} --tmp_path {output_dir}")
    Run_with_args("cat {prefix}.JS.GRCh37.psl | awk '((($11/2)-$12) > 50) && (($13-($11/2)) > 50) && ($1/($13-$12) > 0.8)' > {prefix}.JS.GRCh37.2.psl")
    Run_with_args("cat {prefix}.JS.GRCh37.2.psl | awk '{{print $10}}' | sed 's/\\.[0-9]*$//g' > {prefix}.linearJS")
    Run_with_args("cat {prefix}.info | sed 's/\\.[0-9]*\\t/\\t/g' > {prefix}.tmp.info")
    Run_with_args("cat {prefix}.tmp.info | {RmRedundance_bin} 1 | awk '{{print $1}}' > {prefix}.tmp2.info")
    Run_with_args("cat {prefix}.tmp2.info | {RemoveInList_bin} 1 {prefix}.linearJS 1 > {prefix}.2.info")
    Run_with_args("cat {prefix}.unmapped.fa | {SeqOut_bin} {prefix}.2.info 1 > {prefix}.2.info.fa")
    Run_with_args("{mp_blat_bin} {Reference_genome} {prefix}.2.info.fa {prefix}.2.info.GRCh37.psl -p {mp_blat_process} --blat_bin {blat_bin} --blat_opt \"-tileSize=9 -stepSize=9 -repMatch=32768\" --tmp_path {output_dir}")
    Run_with_args("cat {prefix}.2.info.GRCh37.psl | {PslChimeraFilter_bin} 30 1 > {prefix}.2.chi.bed")
    Run_with_args("cat {prefix_all}.JS.Parsered.sam | {GetNameB4Dot_bin} 3 > {output_dir}/temp.list")
    Run_with_args("cat {prefix}.chi.bed | {RetainInList_bin} 4 {output_dir}/temp.list 1 > {prefix}.chi2.bed")
    Run_with_args("cat {prefix}.chi2.bed | {RetainInList_bin} 4 {prefix}.2.chi.bed 4 > {prefix}.chi3.bed")

    max_fragment_size = config_options["max_fragment_size"]
    config_options.update({"extend":int(int(max_fragment_size)/2 + int(max_fragment_size)/4 - 30)})
    Run_with_args("cat {prefix}.chi3.bed | {JunctionSite2BED_bin} {Gene_annotation} {max_fragment_size} {extend} > {prefix}.PreJS2.bed")
    Run_with_args("cat {prefix}.PreJS2.bed | awk '{{print $4 \"\\t\" $1 \",\" $2 \",\" $3 \",\" $6 \",\"}}' > {prefix}.PreJS2.info")
    Run_with_args("cat {prefix}.PreJS2.info | {AssembleExons_bin} > {prefix}.PreJS2.info2")
    Run_with_args("cat {prefix}.PreJS2.info2 | grep \".5p.\" | awk '{{print $1 \"\\t\" $2 \"{{(}})0{{(}})\"}}' > {prefix}.JS2.info_1")
    Run_with_args("cat {prefix}.PreJS2.info2 | grep \".3p.\" > {prefix}.JS2.info_2")
    Run_with_args("{AssembleJSeq_bin} {prefix}.JS2.info_1 {prefix}.JS2.info_2 > {prefix}.JS2.info")
    Run_with_args("cat {prefix}.JS2.info | {RmRedundance_bin} 2 > {prefix}.JS2.cleaned.info")
    Run_with_args("{GetKey_bin} {prefix}.JS2.cleaned.info > {prefix}.JS2.preIdx")
    Run_with_args("cat {prefix}.info | awk '{{print $1 \"\\t\" $2 \"-\" $3 $4 \":\" $5 \"-\" $6 $7 \":\" $8}}' > {prefix}.JS1.preIdx")
    Run_with_args("cat {prefix}.JS1.preIdx | sed 's/\\.[0-9]*\\t/\\t/g' | {RmRedundance_bin} 1 > {prefix}.JS1.Idx")
    Run_with_args("cat {prefix}.JS2.preIdx | {RetainInList_bin} 2 {prefix}.JS1.Idx 2 > {prefix}.JS2.Idx")
    Run_with_args("{bedtools_bin} getfasta -fi {Reference_genome} -bed {prefix}.PreJS2.bed -tab -name -s -fo {prefix}.PreJS2.seq")
    Run_with_args("cat {prefix}.PreJS2.seq | {AssembleExons_bin} > {prefix}.PreJS2.seq2")
    Run_with_args("cat {prefix}.PreJS2.seq2 | grep \".5p.\" > {prefix}.JS2.seq_1")
    Run_with_args("cat {prefix}.PreJS2.seq2 | grep \".3p.\" > {prefix}.JS2.seq_2")
    Run_with_args("{AssembleJSeq_bin} {prefix}.JS2.seq_1 {prefix}.JS2.seq_2 > {prefix}.JS2.seq")
    Run_with_args("cat {prefix}.JS2.seq | awk '{{print \">\" $1 \"\\n\" $2}}' > {prefix}.JS2.prefa")
    Run_with_args("cat {prefix}.JS2.prefa | {SeqOut_bin} {prefix}.JS2.Idx 1 > {prefix}.JS2.fa")



def NCL_Scan4(config, datasets_list, project_name, output_dir):

    config_options = config.options.copy()
    Run_with_args = Run_cmd(config_options)

    # find span-reads
    config_options.update({"prefix":"{}/{}".format(output_dir, project_name), "prefix_all":"{}/all.{}".format(output_dir, project_name), "output_dir":output_dir})
    Run_with_args("{novoindex_bin} {prefix}.JS2.ndx {prefix}.JS2.fa")

    all_datasets = datasets_list.main_datasets.values() + datasets_list.support_datasets.values()
    JS2_files = []
    JSFilter_result_files = []
    for idx, dataset in enumerate(all_datasets):
        config_options.update({"dataset_prefix": "{}/{}.{}".format(output_dir, dataset.name, dataset.dataset_type), "idx":idx+1})
        Run_with_args("{novoalign_bin} -r A 1 -t 0,1 -d {prefix}.JS2.ndx -f {dataset_prefix}.unmapped_1.fastq {dataset_prefix}.unmapped_2.fastq --3Prime -o SAM > {dataset_prefix}.JS2.sam")
        Run_with_args("cat {dataset_prefix}.JS2.sam | {JSFilter_bin} 10 10 {prefix_all}.{idx}.result > {prefix_all}.{idx}.sam")
        JS2_files.append("{dataset_prefix}.JS2.sam".format(**config_options))
        JSFilter_result_files.append("{prefix_all}.{idx}.result".format(**config_options))

    config_options.update({"JS2_files":' '.join(JS2_files)})
    Run_with_args("cat {JS2_files} > {prefix_all}.JS2.sam")

    Run_with_args("cat {prefix_all}.JS2.sam | {JSFilter_bin} 10 {span_range} {prefix_all}.result > {prefix}.ncl.sam")
    Run_with_args("cat {prefix}.JS2.cleaned.info | {RetainInList_bin} 1 {prefix}.JS2.Idx 1 > {prefix}.JS2.result.info")
    Run_with_args("{GetInfo_bin} {prefix}.JS2.result.info > {prefix}.result.info")
    Run_with_args("cat {prefix}.result.info | sed 's/\\.[0-9]*\\t/\\t/1' > {prefix}.2b.info")
    Run_with_args("cat {prefix}.2b.info | {RetainInList_bin} 1 {prefix_all}.result 1 > {prefix}.3.info")
    Run_with_args("cat {prefix}.3.info | {InsertInList_bin} 1 {prefix_all}.result 1 2 | {InsertInList_bin} 1 {prefix_all}.result 1 3 | {InsertInList_bin} 1 {prefix_all}.result 1 4 > {prefix}.4.info")

    cmd_str = "cat {prefix}.4.info | " + " | ".join(map(lambda result_file: "{InsertInList_bin} 1 " + result_file + " 1 4 0", JSFilter_result_files)) + " > {prefix}.preResult"
    Run_with_args(cmd_str)

    um3_fastas = []
    for idx, dataset in enumerate(all_datasets):
        config_options.update({"dataset_prefix": "{}/{}.{}".format(output_dir, dataset.name, dataset.dataset_type), "idx":idx+1})
        Run_with_args("cat {dataset_prefix}.unmapped_1.fastq | {FastqOut_bin} {prefix}.ncl.sam 1 > {dataset_prefix}_1.um3.fastq")
        Run_with_args("cat {dataset_prefix}.unmapped_2.fastq | {FastqOut_bin} {prefix}.ncl.sam 1 > {dataset_prefix}_2.um3.fastq")
        Run_with_args("{AssembleFastq_bin} {dataset_prefix}_1.um3.fastq {dataset_prefix}_2.um3.fastq > {dataset_prefix}.um3.fa")
        um3_fastas.append("{dataset_prefix}.um3.fa".format(**config_options))

    config_options.update({"um3_fastas":' '.join(um3_fastas)})
    Run_with_args("cat {um3_fastas} > {prefix_all}.um3.fa")

    Run_with_args("{mp_blat_bin} {Reference_genome} {prefix_all}.um3.fa {prefix}.rG.um3.psl -p {mp_blat_process} --blat_bin {blat_bin} --blat_opt \"-tileSize=9 -stepSize=9 -repMatch=32768\" --tmp_path {output_dir}")
    Run_with_args("{mp_blat_bin} {Protein_coding_transcripts} {prefix_all}.um3.fa {prefix}.coding.um3.psl -p {mp_blat_process} --blat_bin {blat_bin} --blat_opt \"-tileSize=9 -stepSize=9 -repMatch=32768\" --tmp_path {output_dir}")
    Run_with_args("{mp_blat_bin} {lncRNA_transcripts} {prefix_all}.um3.fa {prefix}.lncRNA.um3.psl -p {mp_blat_process} --blat_bin {blat_bin} --blat_opt \"-tileSize=9 -stepSize=9 -repMatch=32768\" --tmp_path {output_dir}")
    Run_with_args("{mp_blat_bin} {Repeat_ChrM} {prefix_all}.um3.fa {prefix}.chrM.um3.psl -p {mp_blat_process} --blat_bin {blat_bin} --blat_opt \"-tileSize=9 -stepSize=9 -repMatch=32768\" --tmp_path {output_dir}")

    Run_with_args("cat {prefix}.rG.um3.psl {prefix}.coding.um3.psl {prefix}.lncRNA.um3.psl {prefix}.chrM.um3.psl | awk '$12 < 5' | awk '($11-$13) < 5' > {prefix_all}.um3.colinear.psl")

    JS2b_files = []
    JSFilter_result_b_files = []
    for idx, dataset in enumerate(all_datasets):
        config_options.update({"dataset_prefix":"{}/{}.{}".format(output_dir, dataset.name, dataset.dataset_type), "idx":idx+1})
        if dataset.dataset_type == "main":
            Run_with_args("cat {dataset_prefix}.JS2.sam | {RemoveInList_bin} 1 {prefix_all}.um3.colinear.psl 10 | awk 'sqrt($9*$9) <= {max_fragment_size}' > {dataset_prefix}.JS2b.sam")
            Run_with_args("cat {dataset_prefix}.JS2b.sam | {JSFilter_bin} 10 10 {prefix_all}.{idx}b.result > {prefix_all}.{idx}b.sam")
        elif dataset.dataset_type == "support":
            Run_with_args("cat {dataset_prefix}.JS2.sam | {RemoveInList_bin} 1 {prefix}.colinear.psl 10 > {dataset_prefix}.JS2b.sam")
            Run_with_args("cat {dataset_prefix}.JS2b.sam | {JSFilter_bin} 10 10 {prefix_all}.{idx}b.result > {prefix_all}.{idx}b.sam")

        JS2b_files.append("{dataset_prefix}.JS2b.sam".format(**config_options))
        JSFilter_result_b_files.append("{prefix_all}.{idx}b.result".format(**config_options))

    config_options.update({"JS2b_files":' '.join(JS2b_files)})
    Run_with_args("cat {JS2b_files} > {prefix_all}.JS2b.sam")

    Run_with_args("cat {prefix_all}.JS2b.sam | {JSFilter_bin} 10 {span_range} {prefix_all}.b.result > {prefix}.result.sam")

    Run_with_args("cat {prefix}.2b.info | {RetainInList_bin} 1 {prefix_all}.b.result 1 > {prefix}.3b.info")
    Run_with_args("cat {prefix}.3b.info | {InsertInList_bin} 1 {prefix_all}.b.result 1 2 | {InsertInList_bin} 1 {prefix_all}.b.result 1 3 | {InsertInList_bin} 1 {prefix_all}.b.result 1 4 > {prefix}.4b.info")

    cmd_str = "cat {prefix}.4b.info | " + " | ".join(map(lambda result_file: "{InsertInList_bin} 1 " + result_file + " 1 4 0", JSFilter_result_b_files)) + " | {RmRedundance_bin} 1 | awk '$12 != 0' > {prefix}.result.tmp"
    Run_with_args(cmd_str)

    Run_with_args("{Add_read_count_bin} -tmp {prefix}.result.tmp -sam {prefix}.result.sam -o {prefix}.result.tmp2 --JSParser_bin {JSParser_bin}")
    Run_with_args("{get_gene_name_bin} -tmp {prefix}.result.tmp2 -g {Gene_annotation} -o {prefix}.result.tmp3")

    # get final result
    final_tmp = read_TSV("{prefix}.result.tmp3".format(**config_options))
    final_result = map(lambda field: field[1:7] + field[-3:] + field[-6:-3], final_tmp)
    write_TSV(final_result, "{prefix}.result".format(**config_options))

    print """

The result will be written to {PJ}.result
See {PJ}.result.sam for the final alignment result.

""".format(PJ=project_name)


def Run_cmd(args):
    def Run(cmd_str):
        os.system(cmd_str.format(**args))
    return Run


def read_TSV(tsv_file, read_from_string=False):
    if read_from_string:
        tsv_data_lines = tsv_file.rstrip('\n').split('\n')
        tsv_data_list = [line.split('\t') for line in tsv_data_lines]
        return tsv_data_list
    else:
        with open(tsv_file) as data_reader:
            tsv_data_list = [line.rstrip('\n').split('\t') for line in data_reader]
        return tsv_data_list


def write_TSV(result, out_file="result.txt", write_to_string=False):
    if write_to_string:
        result_tsv = '\n'.join(['\t'.join(line) for line in result])
        return result_tsv
    else:
        with open(out_file, 'w') as data_writer:
            for line in result:
                print >> data_writer, '\t'.join(map(str, line))


class NCLscanConfig(object):
    def __init__(self, config_text):
        self.parse_config(config_text)

    def parse_config(self, config_text):
        format_options = re.sub("^ *(.*?)/? *$", "\g<1>", config_text, flags=re.M)
        all_options = OrderedDict(re.findall("(^[^#\n][\w-]*) *= *(.*)", format_options, flags=re.M))
        for key, value in all_options.items():

            if value == "":
                print >> sys.stderr, "Error: There is a key with empty value: {}".format(key)
                exit(1)

            all_options[key] = value.format(**all_options)
        self.options = all_options

        # check if novoalign v4 is used
        if self.options['novoalign_version'] == '4':
            self.options['novoalign_bin'] = self.options['novoalign_bin'] + ' --pechimera off'

        # parse bwa options
        bwa_option_keys = filter(lambda key: re.match("^bwa-mem", key), self.options.keys())
        bwa_options = []
        for key in bwa_option_keys:
            bwa_options += [re.sub("^bwa-mem", "", key), self.options[key]]
        self.options["bwa_options"] = ' '.join(bwa_options)

        # bwa index
        self.options["bwa_index"] = self.options["NCLscan_ref_dir"] + "/AllRef.fa"

        # novoalign index
        self.options["novoalign_index"] = self.options["NCLscan_ref_dir"] + "/AllRef.ndx"

        # RepChrM.fa
        self.options["Repeat_ChrM"] = self.options["NCLscan_ref_dir"] + "/RepChrM.fa"

        # quality score
        self.options["quality_score"] = int(self.options["quality_score"]) + 33


    def get(self, key):
        return self.options[key]



class DatasetsList(object):

    class Dataset(object):
        def __init__(self, name, dataset_type, reads):
            self.name = name
            self.dataset_type = dataset_type 
            self.reads = reads # eg. ["/path/to/SRR123_1.fq.gz", "/path/to/SRR123_2.fq.gz"]

    def __init__(self, datasets_list):
        self.main_datasets = OrderedDict() # OrderedDict([('name1', Dataset_1), ('name2', Dataset_2), ...])
        self.support_datasets = OrderedDict()

        self.parse_datasets_list(datasets_list)

    def parse_datasets_list(self, datasets_list):
        remove_head_tail_blanks = re.sub("^ *(.*?) *$", "\g<1>", datasets_list, flags=re.M)
        non_comment_lines = re.findall("(^[^#\n].*)", remove_head_tail_blanks, flags=re.M)

        tmp = []
        dataset_type = None
        dataset_name = None
        for line in non_comment_lines:
            dataset_info = re.search("\[ *(\w+) *\. *(\w+) *\]", line)
            if dataset_info:
                if tmp != []:
                    if len(tmp) == 2:
                        if dataset_type == "main":
                            self.main_datasets[dataset_name] = self.Dataset(dataset_name, dataset_type, tmp)
                        #[Support]elif dataset_type == "support":
                        #[Support]    self.support_datasets[dataset_name] = self.Dataset(dataset_name, dataset_type, tmp)
                        else:
                            # report error: Unrecoganized type: 
                            pass
                    tmp = []

                dataset_type = dataset_info.group(1).lower()
                dataset_name = dataset_info.group(2)

            else:
                tmp.append(line)
        else:
            if tmp != []:
                if len(tmp) == 2:
                    if dataset_type == "main":
                        self.main_datasets[dataset_name] = self.Dataset(dataset_name, dataset_type, tmp)
                    #[Support]elif dataset_type == "support":
                    #[Support]    self.support_datasets[dataset_name] = self.Dataset(dataset_name, dataset_type, tmp)
                    else:
                        # report error: Unrecoganized type:
                        pass
        
        if len(self.main_datasets) == 0:
            # report error: No main dataset!
            print >> sys.stderr, "There are no main datasets assigned."
            sys.exit(1)

        



if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--config", help="The config file of NCLscan.")
    #[Support]parser.add_argument("-l", "--datasets_list", help="The list of the datasets for the run.")
    parser.add_argument("--fq1", help="The 1st end of the paired-end reads.")
    parser.add_argument("--fq2", help="The 2nd end of the paired-end reads.")
    parser.add_argument("-pj", "--project_name")
    parser.add_argument("-o", "--output_dir")
    args = parser.parse_args()

    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit()

    # check if each argument has a value.
    too_few_args = False
    if args.config == None:
        print >> sys.stderr, "Need the config file!"
        too_few_args = True
    if (args.fq1 == None) or (args.fq2 == None):
    #[Support]if (args.datasets_list == None) and ((args.fq1 == None) or (args.fq2 == None)):
        print >> sys.stderr, "Need to assign the input dataset!"
        too_few_args = True
    if args.project_name == None:
        print >> sys.stderr, "Need to assign the project name!"
        too_few_args = True
    if args.output_dir == None:
        print >> sys.stderr, "Need to assign the output dir!"
        too_few_args = True
    if too_few_args:
        parser.print_usage()
        sys.exit(1)


    with open(args.config) as config_file:
        config = NCLscanConfig(config_file.read())

    #[Support]
    #[Support]if args.datasets_list != None:
    #[Support]    with open(args.datasets_list) as datasets_list_file:
    #[Support]        datasets_list = DatasetsList(datasets_list_file.read())
    #[Support]else:
    #[Support]    datasets_list_text = "[Main.{}]\n{}\n{}".format(args.project_name, args.fq1, args.fq2)
    #[Support]    datasets_list = DatasetsList(datasets_list_text)
    datasets_list_text = "[Main.{}]\n{}\n{}".format(args.project_name, args.fq1, args.fq2)
    datasets_list = DatasetsList(datasets_list_text)


    os.system("mkdir -p {}".format(args.output_dir))

    NCL_Scan1(config, datasets_list, args.output_dir)
    NCL_Scan2(config, datasets_list, args.project_name, args.output_dir)
    NCL_Scan3(config, datasets_list, args.project_name, args.output_dir)
    NCL_Scan4(config, datasets_list, args.project_name, args.output_dir)




