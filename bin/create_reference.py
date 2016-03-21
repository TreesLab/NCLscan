#! /usr/bin/env python2

import argparse
import re
import os
import sys
from collections import OrderedDict

def create_reference_and_index(config):
    '''
        Create the reference and index for NCLscan.
        1. Repeat_ChrM
        2. AllRef.fa
        3. Bwa index
        4. Novoalign index
    '''

    config_options = config.options.copy()
    Run_with_args = Run_cmd(config_options)

    Run_with_args("mkdir -p {NCLscan_ref_dir}")

    # Generate the "Repeat_ChrM"
    Run_with_args("echo 'chrM' > {NCLscan_ref_dir}/chrM.list")
    Run_with_args("cat {Reference_genome} | {SeqOut_bin} {NCLscan_ref_dir}/chrM.list 1 > {NCLscan_ref_dir}/chrM.fa")
    Run_with_args("cat {NCLscan_ref_dir}/chrM.fa | sed '1d' > {NCLscan_ref_dir}/chrM.seq")
    Run_with_args("head -n 1 {NCLscan_ref_dir}/chrM.fa > {NCLscan_ref_dir}/chrM.title")
    Run_with_args("cat {NCLscan_ref_dir}/chrM.title {NCLscan_ref_dir}/chrM.seq {NCLscan_ref_dir}/chrM.seq > {NCLscan_ref_dir}/RepChrM.fa")

    # Generate the "AllRef.fa"
    Run_with_args("cat {Reference_genome} {Protein_coding_transcripts} {lncRNA_transcripts} {NCLscan_ref_dir}/RepChrM.fa > {NCLscan_ref_dir}/AllRef.fa")

    # Generate the index for bwa
    Run_with_args("{bwa_bin} index {NCLscan_ref_dir}/AllRef.fa")
    
    # Generate the index for novoalign
    Run_with_args("{novoindex_bin} {NCLscan_ref_dir}/AllRef.ndx {NCLscan_ref_dir}/AllRef.fa")

    # Remove tmp files
    Run_with_args("rm -f {NCLscan_ref_dir}/chrM.list {NCLscan_ref_dir}/chrM.fa {NCLscan_ref_dir}/chrM.seq {NCLscan_ref_dir}/chrM.title")


def Run_cmd(args):
    def Run(cmd_str):
        os.system(cmd_str.format(**args))
    return Run


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

    def get(self, key):
        return self.options[key]



if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--config", help="The config file of NCLscan.")
    args = parser.parse_args()

    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit()

    with open(args.config) as config_file:
        config = NCLscanConfig(config_file.read())

    create_reference_and_index(config)



