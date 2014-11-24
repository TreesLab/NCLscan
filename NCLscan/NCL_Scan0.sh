#! /usr/bin/env bash

fq_1=$1
fq_2=$2
pj_name=$3

#echo 'NCL_Scan0 v1.0'

# fastuniq
#./fastuniq -i <(echo "$fq_1 $fq_2"|sed -r 's/ /\n/g') -o $pj_name"_fu_1.fastq" -p $pj_name"_fu_2.fastq"

# BWA
#./bwa aln "bwa_AllRef.fa" $pj_name"_fu_1.fastq" > $pj_name"_fu_1.sai"
#./bwa aln "bwa_AllRef.fa" $pj_name"_fu_2.fastq" > $pj_name"_fu_2.sai"
#./bwa sampe "bwa_AllRef.fa" $pj_name"_fu_1.sai" $pj_name"_fu_2.sai" $pj_name"_fu_1.fastq" $pj_name"_fu_2.fastq" > $pj_name".bwa.sam"

# get unmapped
#samtools view -S -F 2 -f 1 $pj_name".bwa.sam" > $pj_name".bwa.unmapped.sam"
#cat $fq_1 | ./FastqOut <(cut -f '1' $pj_name".bwa.unmapped.sam") 1 > $pj_name".bwa.unmapped_1.fastq"
#cat $fq_2 | ./FastqOut <(cut -f '1' $pj_name".bwa.unmapped.sam") 1 > $pj_name".bwa.unmapped_2.fastq"



echo 'NCL_Scan0 v1.0 (no fastuniq)'

# BWA
./bwa aln "bwa_AllRef.fa" $fq_1 > $pj_name"_fu_1.sai"
./bwa aln "bwa_AllRef.fa" $fq_2 > $pj_name"_fu_2.sai"
./bwa sampe "bwa_AllRef.fa" $pj_name"_fu_1.sai" $pj_name"_fu_2.sai" $fq_1 $fq_2 > $pj_name".bwa.sam"

# get unmapped
samtools view -S -F 2 -f 1 $pj_name".bwa.sam" > $pj_name".bwa.unmapped.sam"
cat $fq_1 | ./FastqOut <(cut -f '1' $pj_name".bwa.unmapped.sam") 1 > $pj_name".bwa.unmapped_1.fastq"
cat $fq_2 | ./FastqOut <(cut -f '1' $pj_name".bwa.unmapped.sam") 1 > $pj_name".bwa.unmapped_2.fastq"


