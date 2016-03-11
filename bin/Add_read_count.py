#! /usr/bin/env python2

import argparse
import subprocess as sp
import re
from utils import *


def add_read_count(result_tmp_file, result_sam_file, output_file, JSParser_bin='./JSParser'):
    result_tmp_data = read_TSV(result_tmp_file)
    result_sam_data = read_TSV(result_sam_file)
    
    all_junc_read_with_ref = get_junc_read(result_sam_data, JSParser_bin)
    all_support_read_with_ref = get_read_with_ref(result_sam_data)
    
    all_result_id = [line[0] for line in result_tmp_data]
    all_junc_read_with_ref = retain_wanted(all_junc_read_with_ref, all_result_id)
    all_junc_read_ref_id = all_junc_read_with_ref.keys()
    all_support_read_with_ref = retain_wanted(all_support_read_with_ref, all_junc_read_ref_id)

    
    all_span_read_with_ref = {}
    for ID in all_junc_read_ref_id:
        junc_reads_id = all_junc_read_with_ref[ID]
        support_reads_id = all_support_read_with_ref[ID]
        span_reads_id = list(set(support_reads_id) - set(junc_reads_id))
        all_span_read_with_ref[ID] = span_reads_id


    all_result_read_count = []
    for ln, line in enumerate(result_tmp_data):
        ref_id = result_tmp_data[ln][0]
        if ref_id in all_junc_read_ref_id:
            num_support_reads = len(all_support_read_with_ref[ref_id])
            num_junc_reads = len(all_junc_read_with_ref[ref_id])
            num_span_reads = len(all_span_read_with_ref[ref_id])
            all_result_read_count.append(line + [num_support_reads, num_junc_reads, num_span_reads])
        
    write_TSV(all_result_read_count, output_file)
    


def get_read_with_ref(sam_data):
    read_with_ref = {}
    for line in sam_data:
        ref_id = re.sub("\.[0-9]*$", "", line[2])
        read_id = line[0]
        if ref_id in read_with_ref:
            if read_id not in read_with_ref[ref_id]:
                read_with_ref[ref_id].append(read_id)
        else:
            read_with_ref[ref_id] = [read_id]
    return read_with_ref


def get_junc_read(res_sam_data, JSParser_bin):
    ref_names = list(set(map(lambda line: line[2], res_sam_data)))
    new_header = map(lambda ref_name: ["@SQ", "SN:%s"%ref_name, "LN:1000"], ref_names)
    re_headered_res_sam_data = new_header + res_sam_data
    
    p1 = sp.Popen([JSParser_bin, "95", "10", "10"], stdin=sp.PIPE, stdout=sp.PIPE)
    
    tsv_result = write_TSV(re_headered_res_sam_data, write_to_string=True)
    JSParser_result = p1.communicate(tsv_result)[0]
    junc_read_sam_data = read_TSV(JSParser_result, read_from_string=True)
    junc_read_data = get_read_with_ref(junc_read_sam_data)
    
    return junc_read_data



def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-pj", "--project_name", dest="project_name", help="The project name.")
    parser.add_argument("-tmp", "--result_tmp_file", dest="result_tmp_file", help="[Project].result.tmp")
    parser.add_argument("-sam", "--result_sam_file", dest="result_sam_file", help="[Project].result.sam")
    parser.add_argument("-o", "--output", dest="output", help="The output filename.")
    parser.add_argument("--JSParser_bin", default="./JSParser")
    args = parser.parse_args()


    pj_name        = args.project_name    or get_parameter("config.tmp", 2)
    res_tmp_file   = args.result_tmp_file or "{PJ}.result.tmp".format(PJ=pj_name)
    res_sam_file   = args.result_sam_file or "{PJ}.result.sam".format(PJ=pj_name)
    output_file    = args.output          or "{result_tmp}.read_count".format(result_tmp=res_tmp_file)


    add_read_count(res_tmp_file, res_sam_file, output_file, args.JSParser_bin)


if __name__ == "__main__":
    main()

