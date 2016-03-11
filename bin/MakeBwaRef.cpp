#include <stdio.h>
#include <stdlib.h>
#include "Common.h"
#include "Table.h"
#include "NCL_head.h"

void PrintUsage();

int main(int argc, char **argv)
{
	FILE *fp;
	char str[4096];

	CONFIG conf;

	if(argc != 1)
	{ PrintUsage();	return 1; }

	if( ReadConfig("config.txt", &conf) != 0)
	{	ErrMsg("config.txt can not be read!\n"); return 1;	}	

	system("echo chrM > chrM.list");

	sprintf(str, "cat %s | ./SeqOut chrM.list 1 > chrM.fa", conf.rG);	
	system(str);
	system("cat chrM.fa | grep -v \"^>\" > chrM.seq");
	system("head -n 1 chrM.fa > chrM.title");
	system("cat chrM.title chrM.seq chrM.seq > RepChrM.fa");
	sprintf(str, "cat %s %s %s RepChrM.fa > bwa_AllRef.fa", conf.rG, conf.rR, conf.rN);
	system(str);

	system("rm chrM.list chrM.fa chrM.seq chrM.title");

	system("./bwa index bwa_AllRef.fa");

	return 0;
}

void PrintUsage()
{
	fprintf(stderr, "Make the reference and index for bwa.\n");
	fprintf(stderr, "Three files of reference sequences are needed:\n");
	fprintf(stderr, "1. ref. genome coding transcripts (ex gencode.v19.pc_transcripts.fa)\n");
	fprintf(stderr, "2. ref. coding transcripts Ref.  (ex. GRCh37.p13.genome.fa)\n");
	fprintf(stderr, "3. ref. non-coding transcripts (ex gencode.v19.lncRNA_transcripts.fa)\n");
	fprintf(stderr, "The reference sequences can be downloaded from GENCODE (http://www.gencodegenes.org/)\n");
	fprintf(stderr, "If Gencode 19 is not used for the reference sequences,\n"); 
	fprintf(stderr, "the context of config.txt should be modified accordingly.\n\n");

	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "./MakeBwaRef\n\n");


}

