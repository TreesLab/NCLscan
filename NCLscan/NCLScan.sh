#! /usr/bin/env bash

fq_1=$1
fq_2=$2
pj_name=$3
quality=$4

echo "========== Start ==========="
date
echo "============================"
./NCL_Scan0.sh $fq_1 $fq_2 $pj_name
./NCL_Scan1 $pj_name".bwa.unmapped_1.fastq" $pj_name".bwa.unmapped_2.fastq" $pj_name
./NCL_Scan2 $quality
./NCL_Scan3
./NCL_Scan4
./get_GeneName_1.1.py

echo "=========== Stop ============"
date
echo "============================="


