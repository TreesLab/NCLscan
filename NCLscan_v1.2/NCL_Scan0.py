#! /usr/bin/env python

import argparse
import os
import subprocess

def get_bwa_unmapped(fq_1, fq_2, pj_name, bwa_idx, bwa_opt):
    os.system("./bwa mem {bwa_opt} {bwa_idx} {fq_1} {fq_2} > {pj_name}.bwa.sam".format( \
                fq_1=fq_1, fq_2=fq_2, pj_name=pj_name, bwa_idx=bwa_idx, bwa_opt=bwa_opt))

    os.system("samtools view -S -F 2 -f 1 {pj_name}.bwa.sam > {pj_name}.bwa.unmapped.sam".format(pj_name=pj_name))

    os.mkdir("split_unmap.tmp")
    os.system("cut -f '1' {pj_name}.bwa.unmapped.sam | uniq | split -C 1G - split_unmap.tmp/bwa_unmapped.".format(pj_name=pj_name))

    # get the output of the command "ls split_unmap.tmp"
    all_splited_unmapped = subprocess.check_output(['ls', 'split_unmap.tmp']).rstrip('\n').split('\n')

    os.system("cat {fq_1} | ./FastqOut split_unmap.tmp/{sp_f} 1 > {pj_name}.bwa.unmapped_1.fastq".format(fq_1=fq_1, sp_f=all_splited_unmapped[0], pj_name=pj_name))
    os.system("cat {fq_2} | ./FastqOut split_unmap.tmp/{sp_f} 1 > {pj_name}.bwa.unmapped_2.fastq".format(fq_2=fq_2, sp_f=all_splited_unmapped[0], pj_name=pj_name))
    for splited_file in all_splited_unmapped[1:]:
        os.system("cat {fq_1} | ./FastqOut split_unmap.tmp/{sp_f} 1 >> {pj_name}.bwa.unmapped_1.fastq".format(fq_1=fq_1, sp_f=splited_file, pj_name=pj_name))
        os.system("cat {fq_2} | ./FastqOut split_unmap.tmp/{sp_f} 1 >> {pj_name}.bwa.unmapped_2.fastq".format(fq_2=fq_2, sp_f=splited_file, pj_name=pj_name))

    # remove tmp files
    for splited_file in all_splited_unmapped:
        os.remove('split_unmap.tmp/%s'%splited_file)
    os.rmdir('split_unmap.tmp')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("Fastq_1")
    parser.add_argument("Fastq_2")
    parser.add_argument("Project_Name")
    parser.add_argument("-idx", "--bwa_index_prefix", help="the index prefix for bwa.")
    parser.add_argument("-opt", "--bwa_option", help="pass the options to bwa.", default="")
    args = parser.parse_args()

    get_bwa_unmapped(args.Fastq_1, args.Fastq_2, args.Project_Name, args.bwa_index_prefix, args.bwa_option)



if __name__ == "__main__":
    main()
