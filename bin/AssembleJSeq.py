#! /usr/bin/env python2

import argparse
import os
import sys
import re

def AssembleJSeq(jseq1_file, jseq2_file):
    jseq1 = get_ID_and_seq(jseq1_file)
    jseq2 = get_ID_and_seq(jseq2_file)

    get_ID = lambda seq: seq[0]
    common_IDs = list(set(map(get_ID, jseq1)) & set(map(get_ID, jseq2)))
    #print(common_IDs)

    jseq1_common = iter(filter(lambda seq: seq[0] in common_IDs, jseq1))
    jseq2_common = iter(filter(lambda seq: seq[0] in common_IDs, jseq2))

    jseq1_common_gp = groupby(lambda seq: seq[0], jseq1_common)
    jseq2_common_gp = groupby(lambda seq: seq[0], jseq2_common)

    if len(jseq1_common_gp) != len(jseq2_common_gp):
        print >> sys.stderr, "Format Error!!"
        sys.exit()
    else:
        for i in range(len(jseq1_common_gp)):
            idx = 0
            for seq1 in jseq1_common_gp[i]:
                ID = seq1[0]
                for seq2 in jseq2_common_gp[i]:
                    print "{}.{}\t{}{}".format(ID, idx, seq1[1], seq2[1])
                    idx += 1


def get_ID_and_seq(jseq_file):
    with open(jseq_file) as jseq:
        ID_and_seq = re.findall("^(.+)\.[35]p\.[0-9]+(?:\([+-]\))?\t(.+)$", jseq.read(), flags=re.M)
    return ID_and_seq


def groupby(group_key, sorted_data):
    result_box = []
    old_id = None
    tmp_box = []
    for line in sorted_data:
        new_id = group_key(line)
        if old_id == None:
            old_id = new_id
        if new_id != old_id:
            result_box.append(tmp_box)
            old_id = new_id
            tmp_box = []
        tmp_box.append(line)
    else:
        result_box.append(tmp_box)
    return result_box



if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("jseq1")
    parser.add_argument("jseq2")
    args = parser.parse_args()

    AssembleJSeq(args.jseq1, args.jseq2)

