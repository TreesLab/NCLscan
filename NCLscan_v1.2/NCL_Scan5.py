#! /usr/bin/env python

import os
import re
import argparse

def add_gene_name(result_file, anno_file, output_file):
    with open(result_file) as data_reader:
        all_result = [line.rstrip('\n').split('\t') for line in data_reader]

    gene_name_dict = get_gene_name_dict(anno_file)

    with open(output_file, 'w') as data_writer:
        result_data_with_gene_name = []
        for line in all_result:
            gene_name_1 = sorted(gene_name_dict['_'.join(line[1:3])])
            gene_name_2 = sorted(gene_name_dict['_'.join(line[4:6])])
        
            if len(gene_name_1) + len(gene_name_2) > len(set(gene_name_1 + gene_name_2)):
                isIntragenic = '1'
            else:
                isIntragenic = '0'

            print >> data_writer, '\t'.join(line[1:7] + [','.join(gene_name_1), ','.join(gene_name_2), isIntragenic])


def get_gene_name_dict(anno_file):
    # read annotation data
    with open(anno_file) as data_reader:
        anno_raw_data = data_reader.read()
    
    # parse gene name
    chrm_exon_gene_data = list(set(re.findall('(chr[^\t]+)\t.*exon\t([0-9]+)\t([0-9]+).*gene_name "([^;]+)";', anno_raw_data)))
    
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

    return gene_name_dict


def get_parameter(config_file, key_num):
    with open(config_file) as data_reader:
        pm = data_reader.readlines()[key_num].rstrip('\n')
    return pm


def main():
    parser = argparse.ArgumentParser()
    args = parser.parse_args()

    pj_name = get_parameter('config.tmp', 2)
    anno_file = get_parameter('config.txt', 3)

    os.system('mv {pj_name}.result {pj_name}.result.tmp'.format(pj_name=pj_name))

    result_file = pj_name + ".result.tmp"
    output_file = pj_name + ".result"

    add_gene_name(result_file, anno_file, output_file)
    

if __name__ == "__main__":
    main()
