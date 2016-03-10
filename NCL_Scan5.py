#! /usr/bin/env python2

import argparse
import os
from utils import *

def NCL_Scan5(pj_name):
    os.system("./Add_read_count.py -tmp {PJ}.result.tmp -sam {PJ}.result.sam -o {PJ}.result.tmp2".format(PJ=pj_name))
    os.system("./get_gene_name.py -tmp {PJ}.result.tmp2 -o {PJ}.result.tmp3".format(PJ=pj_name))

    final_tmp = read_TSV("{PJ}.result.tmp3".format(PJ=pj_name))
    final_result = map(lambda field: field[1:7] + field[-3:] + field[-6:-3], final_tmp)
    write_TSV(final_result, "{PJ}.result".format(PJ=pj_name))


    print """

The result will be written to {PJ}.result
See {PJ}.result.sam for the final alignment result.

""".format(PJ=pj_name)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    args = parser.parse_args()

    pj_name = get_parameter("config.tmp", 2)

    NCL_Scan5(pj_name)
    
