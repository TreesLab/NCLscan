#! /usr/bin/env python

import re
import argparse
from utils import *

def add_gene_name(result_tmp_file, anno_file, output_file):
    result_tmp_data = read_TSV(result_tmp_file)

    gene_name_dict = get_gene_name_dict(anno_file)

    result_tmp_data_with_gene_name = []
    for line_list in result_tmp_data:
        gene_name_1 = sorted(gene_name_dict.get('_'.join(line_list[1:3]), [None]))
        gene_name_2 = sorted(gene_name_dict.get('_'.join(line_list[4:6]), [None]))

        if (gene_name_1 == [None]) or (gene_name_2 == [None]):
            isIntragenic = 'NA'
        else:
            if len(gene_name_1) + len(gene_name_2) > len(set(gene_name_1 + gene_name_2)):
                isIntragenic = '1'
            else:
                isIntragenic = '0'

        result_tmp_data_with_gene_name.append(line_list + [','.join(gene_name_1), ','.join(gene_name_2), isIntragenic])

    write_TSV(result_tmp_data_with_gene_name, output_file)


def get_gene_name_dict(anno_file):
    # read annotation data
    with open(anno_file) as data_reader:
        anno_raw_data = data_reader.read()
    
    # parse gene name
    chrm_exon_gene_data = list(set(re.findall('(chr[^\t]+)\t.*exon\t([0-9]+)\t([0-9]+).*gene_name "([^;]+)";', anno_raw_data)))
    
    # release memory
    del anno_raw_data
    
    chrm_exon_gene_data_sep = map(lambda line: (line[0], line[1], line[3]), chrm_exon_gene_data) + map(lambda line: (line[0], line[2], line[3]), chrm_exon_gene_data)    
    chrm_exon_gene_data_sep = list(set(chrm_exon_gene_data_sep))
    
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


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-pj", "--project_name", dest="project_name", help="The project name.")
    parser.add_argument("-tmp", "--result_tmp_file", dest="result_tmp_file", help="[Project].result.tmp")    
    parser.add_argument('-g', '--gene_annotation', dest="gene_anno", help="Assign custom gene annotation GTF file.")
    parser.add_argument("-o", "--output", dest="output", help="The output filename.")
    args = parser.parse_args()


    pj_name        = args.project_name    or get_parameter("config.tmp", 2)
    res_tmp_file   = args.result_tmp_file or "{PJ}.result.tmp".format(PJ=pj_name)
    anno_file      = args.gene_anno       or get_parameter('config.txt', 3)
    output_file    = args.output          or "{result_tmp}.gene_name".format(result_tmp=res_tmp_file)


    add_gene_name(res_tmp_file, anno_file, output_file)    

if __name__ == "__main__":
    main()

