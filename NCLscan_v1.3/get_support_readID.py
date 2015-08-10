#! /usr/bin/env python

import argparse
import re


def get_support_readID(result_tmp_file, result_sam_file, output_file):
    support_readID_dict = get_support_readID_dict(readTSV(result_sam_file))

    result_tmp = readTSV(result_tmp_file)

    result_support_readIDs = []
    for line in result_tmp:
        main_ID = line[0]
        if support_readID_dict.has_key(main_ID):
            result_support_readIDs.append(line[1:7] + line[10:11] + [','.join(support_readID_dict[main_ID])])
        else:
            result_support_readIDs.append(line[1:7] + line[10:11] + [])
            
    writeTSV(result_support_readIDs, output_file)


def get_support_readID_dict(result_sam):
    support_readID_dict = {}
    for line in result_sam:
        main_ID = re.search('^(.*)\.[0-9]+', line[2]).group(1)
        supp_ID = line[0]
        if support_readID_dict.has_key(main_ID):
            if supp_ID not in support_readID_dict[main_ID]:
                support_readID_dict[main_ID].append(supp_ID)
        else:
            support_readID_dict[main_ID] = []
    return support_readID_dict


def readTSV(in_file):
    with open(in_file) as data_reader:
        all_data = [line.rstrip('\n').split('\t') for line in data_reader]
    return all_data

def writeTSV(result, out_file):
    with open(out_file, 'w') as data_writer:
        for line in result:
            print >> data_writer, '\t'.join(line)



def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('Result_tmp', help='PROJECT.result.tmp')
    parser.add_argument('Result_sam', help='PROJECT.result.sam')
    parser.add_argument('Output', help='Output file name')
    args = parser.parse_args()

    get_support_readID(args.Result_tmp, args.Result_sam, args.Output)


if __name__ == "__main__":
    main()


