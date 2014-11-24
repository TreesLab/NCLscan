#! /usr/bin/env python

# version: 1.1

from __future__ import print_function

import sys
import re
import optparse

# The following two lines are used to solve the problem of 'IOError: [Errno 32] Broken pipe'
from signal import signal, SIGPIPE, SIG_DFL
signal(SIGPIPE,SIG_DFL)

def get_Gene_Name(in_file, out_file, anno_file, to_stdout=False, gene_sep='|'):
    # read annotation data
    with open(anno_file) as data_reader:
        anno_raw_data = data_reader.read()
    
    # parse gene name
    chrm_exon_gene_data = list(set(re.findall('(chr[^\t]+)\t.*exon\t([0-9]+)\t([0-9]+).*gene_name "(.+)"; transcript_type ', anno_raw_data)))
    
    # release memory
    del anno_raw_data
    
    chrm_exon_gene_data_sep = map(lambda line: [line[0], line[1], line[3]], chrm_exon_gene_data) + map(lambda line: [line[0], line[2], line[3]], chrm_exon_gene_data)    
    chrm_exon_gene_data_sep = list(set(map(tuple, chrm_exon_gene_data_sep)))
    
    # generating gene_name_dict
    gene_name_dict = {}
    for line in chrm_exon_gene_data_sep:
        gene_key = '_'.join(line[:2])
        gene_name = line[2]
        if gene_name_dict.has_key(gene_key):
            gene_name_dict[gene_key].append(gene_name)
        else:
            gene_name_dict[gene_key] = [gene_name]
    
    # read in_file
    with open(in_file) as data_reader:
        input_data = map(lambda line: re.sub('\r?\n?$', '', line).split('\t'), data_reader.readlines())
        
    # get gene name
    result_data_with_gene_name = []
    for line in input_data:
        result_data_with_gene_name.append(line + [gene_sep.join(sorted(gene_name_dict['_'.join(line[1:3])])), gene_sep.join(sorted(gene_name_dict['_'.join(line[4:6])]))])
    
    # output
    if to_stdout:
        for line in result_data_with_gene_name:
            print('\t'.join(line))
    else:
        with open(out_file, 'w') as res_writer:
            for line in result_data_with_gene_name:
                print('\t'.join(line), file=res_writer)
            


def main():
    p = optparse.OptionParser()
    p.usage = '%prog [-i INPUT] [-o OUTPUT] [-a ANNO] [-s SEP] [--stdout]'
    p.add_option('-i', dest='input', help='Input the result of NCL_Scan.')
    p.add_option('-o', dest='output', help='The output file.')
    p.add_option('-a', dest='anno', help='The annotaion file (.gtf).')
    p.add_option('-s', dest='gene_sep', help='Separator of gene names. Default "|"')
    p.add_option('--stdout', dest='stdout', action='store_true', help='Send the output to stdout.')
    opts, args = p.parse_args()

    if opts.input:
        NCLScan_result = opts.input
    else:
        try:
            with open('config.tmp') as config_tmp:
                pj_name = re.sub('\r?\n?$', config_tmp.readlines()[2])
        except IOError:
            print("Error: No such file 'config.tmp', or you may just assign an input file.\n")
            p.print_help()
            return
        NCLScan_result = pj_name + '.result'

    if opts.output:
        output_file = opts.output
    else:
        output_file = NCLScan_result + '.gene_name'

    if opts.anno:
        anno_file = opts.anno
    else:
        try:
            with open('config.txt') as config_txt:
                anno_file = re.sub('\r?\n?', config_txt.readlines()[3])
        except IOError:
            print("Error: No such file 'config.txt', or you may just assign an annotation file (in GTF format).\n")
            p.print_help()
            return
    
    if opts.gene_sep:
        get_Gene_Name(NCLScan_result, output_file, anno_file, opts.stdout, opts.gene_sep)
    else:
        get_Gene_Name(NCLScan_result, output_file, anno_file, opts.stdout)

if __name__ == '__main__':
    main()
