#! /usr/bin/env python2
import sys
import re
import logging
from operator import itemgetter
from collections import defaultdict
from itertools import chain


logger = logging.getLogger(__name__)


def append_Z3_tag():
    for line in sys.stdin:
        line = line.rstrip('\n')

        if line.startswith('@'):
            print(line)

        else:
            sam_data = line.split('\t')
            pos = sam_data[3]
            cigar = sam_data[5]

            if cigar == '*':
                print(line)

            else:
                Z3_tag = generate_Z3_tag(pos, cigar)
                print('\t'.join(sam_data + [Z3_tag]))


def generate_Z3_tag(pos, cigar):
    pos = int(pos)
    cigar = CIGAR(cigar)

    ref_consumes_items = itemgetter('M', 'D', 'N', '=', 'X')(cigar)
    logger.debug(ref_consumes_items)

    ref_consumes = sum(chain.from_iterable(ref_consumes_items))
    logger.debug(ref_consumes)

    Z3 = pos + ref_consumes - 1
    logger.debug("%d + %d - 1 = %d", pos, ref_consumes, Z3)

    return "Z3:i:{}".format(Z3)


class CIGAR:
    """Use to parse the CIGAR string."""

    _CIGAR_PAT = re.compile(r'(([0-9]+)([MIDNSHP=X]))')

    def __init__(self, cigar_str):
        self._cigar_str = cigar_str
        self._parse()

    def _parse(self):
        parsed_result = re.findall(self._CIGAR_PAT, self._cigar_str)
        
        self._cigar_list = [cigar for cigar, _, _ in parsed_result]
        
        self._cigar_dict = defaultdict(list)
        for _, num, op in parsed_result:
            self._cigar_dict[op].append(int(num))

        logger.debug(self._cigar_str)
        logger.debug(parsed_result)
        logger.debug(self._cigar_list)
        logger.debug(self._cigar_dict)

    def __getitem__(self, key):
        if isinstance(key, int):
            return self._cigar_list[key]

        elif isinstance(key, str):
            return self._cigar_dict.get(key, [])

        else:
            pass

    def __repr__(self):
        return str(self._cigar_list)

    def __str__(self):
        return self._cigar_str


if __name__ == "__main__":
    if (len(sys.argv) > 1) and ('--debug' in sys.argv):
        logger.setLevel(logging.DEBUG)

        fh = logging.FileHandler('append_Z3_tag.log', mode='w')
        formatter = logging.Formatter("%(asctime)s (%(levelname)s) %(message)s")
        fh.setFormatter(formatter)

        logger.addHandler(fh)

    append_Z3_tag()
