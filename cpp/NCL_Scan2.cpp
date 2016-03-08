#include <stdio.h>
#include <stdlib.h>
#include "Common.h"
#include "Table.h"
#include "NCL_head.h"

void PrintUsage();

int main(int argc, char **argv)
{
	FILE *fp;
	char str[1024];

	CONFIG conf;

	char Read1[255];
	char Read2[255];
	char PName[255];

	if(argc != 2)
	{ PrintUsage();	return 1; }

	if(	( fp = fopen("config.tmp", "r") ) == NULL )
	{		
		ErrMsg("config.tmp can not be found!\n");
		ErrMsg("Please make sure that NCL_Scan1 has been finished properly.\n");
		return 1;	
	}

	fscanf(fp, "%s\n%s\n%s", Read1, Read2, PName);

	fclose(fp);

	if( ReadConfig("config.txt", &conf) != 0)
	{	ErrMsg("config.txt can not be read!\n"); return 1;	}

	sprintf(str, "cat %s.sam | ./RmBadMapping 30 0 %d | grep \"^@\" -v | ./RmColinearPairInSam > %s.unmapped.sam", PName, atoi(argv[1])+33, PName);	
	system(str);	// Remove Co-linear reads according to the SAM by novoalign

	sprintf(str, "cat %s.unmapped.sam | cut -f '1' | sort | uniq > %s.unmapped.sam.id", PName, PName);
	system(str);
	sprintf(str, "cat %s | ./FastqOut %s.unmapped.sam.id 1 > %s.unmapped_1.fastq", Read1, PName, PName);
	system(str);
	sprintf(str, "cat %s | ./FastqOut %s.unmapped.sam.id 1 > %s.unmapped_2.fastq", Read2, PName, PName);	
	system(str);
	sprintf(str, "./AssembleFastq %s.unmapped_1.fastq %s.unmapped_2.fastq > %s.unmapped.fa", PName, PName, PName);
	system(str);
	sprintf(str, "./blat %s %s.unmapped.fa %s.rG.psl", conf.rG, PName, PName);
	system(str);	
	sprintf(str, "cat %s.rG.psl | awk 'substr($14,0,2) == \"GL\"' | awk '($1+$3) >= 50' > %s.un.psl", PName, PName);
	system(str);
	sprintf(str, "cat %s.rG.psl | ./RemoveInList 10 %s.un.psl 10 > %s.rG.non_un.psl", PName, PName, PName);
	system(str);
	sprintf(str, "cat %s.rG.non_un.psl | ./PslChimeraFilter 30 5 > %s.chi0.bed", PName, PName);
	system(str);	
	sprintf(str, "cat %s.unmapped.fa | ./SeqOut %s.chi0.bed 4 > %s.unmapped.2.fa", PName, PName, PName);
	system(str);
	sprintf(str, "./blat %s %s.unmapped.2.fa %s.coding.2.psl", conf.rR, PName, PName);
	system(str);
	sprintf(str, "./blat %s %s.unmapped.2.fa %s.lncRNA.2.psl", conf.rN, PName, PName);
	system(str);
	sprintf(str, "./blat RepChrM.fa %s.unmapped.2.fa %s.chrM.2.psl", PName, PName);
	system(str);
	sprintf(str, "cat %s.coding.2.psl %s.lncRNA.2.psl %s.chrM.2.psl | awk '$12 < 10' | awk '($11-$13) < 10' > %s.colinear.psl", PName, PName, PName, PName);
	system(str);
	sprintf(str, "cat %s.chi0.bed | ./RemoveInList 4 %s.colinear.psl 10 > %s.chi.bed", PName, PName, PName);
	system(str);	
	sprintf(str, "cat %s.chi.bed | ./JunctionSite2BED %s %d %d > %s.preJS.bed", PName, conf.annotation, conf.rlen, (conf.fsize/2)+(conf.fsize/4)-30, PName );
	system(str);	// Re-alignment of unmapped reads by blat and locate chimera candidates from the blat results

	sprintf(str, "cat %s.preJS.bed | awk '{print $4 \"\\t\" $1 \",\" $2 \",\" $3 \",\" $6 \",\"}' > %s.preJS.info", PName, PName );
	system(str);
	sprintf(str, "cat %s.preJS.info | ./AssembleExons > %s.preJS.info2", PName, PName );
	system(str);
	sprintf(str, "cat %s.preJS.info2 | grep \".5p.\" | awk '{print $1 \"\\t\" $2 \"{(})0{(})\"}' > %s.JS.info_1", PName, PName );
	system(str);
	sprintf(str, "cat %s.preJS.info2 | grep \".3p.\" > %s.JS.info_2", PName, PName );
	system(str);
	sprintf(str, "./AssembleJSeq %s.JS.info_1 %s.JS.info_2 > %s.JS.info", PName, PName, PName );
	system(str);
	sprintf(str, "cat %s.JS.info | ./RmRedundance 2 > %s.JS.cleaned.info", PName, PName);
	system(str);	// Collect information of the chimera candidates

	sprintf(str, "./bedtools getfasta -fi %s -bed %s.preJS.bed -tab -name -s -fo %s.preJS.seq", conf.rG, PName, PName );
	system(str);
	sprintf(str, "cat %s.preJS.seq | ./AssembleExons > %s.preJS.seq2", PName, PName );
	system(str);
	sprintf(str, "cat %s.preJS.seq2 | grep \".5p.\" > %s.JS.seq_1", PName, PName );
	system(str);
	sprintf(str, "cat %s.preJS.seq2 | grep \".3p.\" > %s.JS.seq_2", PName, PName );
	system(str);
	sprintf(str, "./AssembleJSeq %s.JS.seq_1 %s.JS.seq_2 > %s.JS.seq12", PName, PName, PName );
	system(str);
	sprintf(str, "cat %s.JS.seq12 | ./RetainInList 1 %s.JS.cleaned.info 1 > %s.JS.seq", PName, PName, PName );
	system(str);
	sprintf(str, "cat %s.JS.seq | awk '{print \">\" $1 \"\\n\" $2}' > %s.JS.fa", PName, PName );
	system(str);	// Get short junction sequences of the chimera candidates

//	sprintf(str, "rm %s.unmapped_1.fastq %s.unmapped_2.fastq %s.un.psl", PName, PName, PName);
//	system(str);

	return 0;
}

void PrintUsage()
{
	fprintf(stderr, "Aligning unmapped reads using blat.\n\n");

	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "NCL_Scan2 [Quality]\n\n");

	fprintf(stderr, "[Quality]       Requirement of read quality scores.\n\n");
	fprintf(stderr, "Higher the value would reduce the time of blat alignemnt,\n");
	fprintf(stderr, "with the penalty of lower sensitivity.\n\n");
	
	fprintf(stderr, "Example:\n");
	fprintf(stderr, "NCL_Scan2 10\n");
}

