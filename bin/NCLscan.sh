#! /usr/bin/env bash

fq_1=$1
fq_2=$2
pj_name=$3
quality=$4
bwa_thread=$5
span_range=$6

echo "========== Start ==========="
date
echo "============================"
./NCL_Scan0.py -idx "bwa_AllRef.fa" -t $bwa_thread $fq_1 $fq_2 $pj_name
./NCL_Scan1 $pj_name".bwa.unmapped_1.fastq" $pj_name".bwa.unmapped_2.fastq" $pj_name
./NCL_Scan2 $quality
./NCL_Scan3
./NCL_Scan4 $span_range
./NCL_Scan5.py

echo "=========== Stop ============"
date
echo "============================="


