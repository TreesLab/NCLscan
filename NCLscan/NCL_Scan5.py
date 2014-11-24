#! /usr/bin/env python

# NCL_Scan5
# version: 1.7
# Append the number and the ID of support reads from each sample to the result induced from NCL_Scan4.

import re
import optparse


def NCL_Scan5(project_name=None):
    
    def get_TSV_data_to_list(in_data_path):
        with open(in_data_path) as in_data:
            TSV_data = map(lambda line: re.sub('\r?\n?$', '', line).split('\t'), in_data.readlines())
        return TSV_data

    if project_name == None:
        config_tmp = get_TSV_data_to_list('./config.tmp')
        project_name = config_tmp[2][0]

    result = project_name + '.result'
    support = project_name + '.result.sam'
    output = project_name + '.result.support_reads'

    # read data
    result_data = get_TSV_data_to_list(result)
    support_data = get_TSV_data_to_list(support)
    
    # get the leader IDs of support reads
    support_read_ID_leader = [line[0] for line in result_data]

    # get sample names
    ## should find sample name from './test_data/ORIGIN_FASTQ_FILE_1or2'
    sample_name = sorted(list(set([re.split('\.', line)[0] for line in support_read_ID_leader]))) # fix in 1.6: '[ _:.]' -> '.'   # 1.7 - fix bug: '.' -> '\.'    - add 'sorted()'

    # generate the 1st container of support read IDs for each event
    support_read_IDs = {}
    for ID in support_read_ID_leader:
        support_read_IDs[ID] = []

    # get the support read IDs for each event
    for data in support_data:
        leader_of_support_read = re.findall('(.*)\.[0-9]*', data[2])[0]
        if leader_of_support_read in support_read_ID_leader:
            if data[0] not in support_read_IDs[leader_of_support_read]:
                support_read_IDs[leader_of_support_read].append(data[0])

    # separate support read IDs by sample names for each event
    sample_support_read_IDs = {}
    for ID in support_read_ID_leader:
        sample_support_read_IDs[ID] = []
        for name in sample_name:
            sample_support_read_IDs[ID].append([read_ID for read_ID in support_read_IDs[ID] if re.match(name, read_ID)])        

    # write the result of support IDs to output
    with open(output, 'w') as data_writer:
        for line in result_data:
            out_line = '\t'.join(line + 
                                 map(lambda IDs: str(len(IDs)), sample_support_read_IDs[line[0]]) + 
                                 map(lambda IDs: ','.join(IDs), sample_support_read_IDs[line[0]]))
            print >> data_writer, out_line

    # write the order of sample names to the output (new in 1.7)
    with open(output + '.order', 'w') as data_writer:
        for name in sample_name:
            print >> data_writer, name


def main():
    p = optparse.OptionParser()
    p.usage = '%prog [-p PROJECT_NAME]'
    p.add_option('-p', '--project_name', help='assign the project name.')
    opts, args = p.parse_args()
    
    NCL_Scan5(opts.project_name)


if __name__ == '__main__':
    main()
