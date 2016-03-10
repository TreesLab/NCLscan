#include <stdio.h>
#include <stdlib.h>
#include "Common.h"
#include "Table.h"
#include "NCL_head.h"


void RemoveFileSubName(char *str)
{
	int i;
	
	for( i = strlen(str); i > 0; i--)
	{
		if( str[i] == '.' )
		{
			str[i] = '\0';
			return;
		}
	}
	
	return;
}

void PrintUsage();

int main(int argc, char **argv)
{
	FILE *fp;
	char str[32768];
	char fname[255];
	char allnames[32767] = "";
	int i;

	CONFIG conf;

	char Read1[255] = "";
	char Read2[255] = "";
	char PName[255] = "";

	fprintf(stderr, "NCL_Scan3 v0.5\n");

	if( ReadConfig("config.txt", &conf) != 0)
	{	ErrMsg("config.txt can not be read!\n"); return 1;	}	

	if(	( fp = fopen("config.tmp", "r") ) == NULL )
	{		
		ErrMsg("config.tmp can not be found!\n");
		ErrMsg("Please make sure that NCL_Scan2 has been finished properly.\n");
		return 1;	
	}

	printf("%s\t%s\n", argv[0], argv[1]);
	fscanf(fp, "%s\n%s\n%s", Read1, Read2, PName);

	fclose(fp);

	sprintf(str, "./novoindex %s.JS.ndx %s.JS.fa", PName, PName );
	system(str);

	system("rm *.JS.sam");

	if( argc == 1 )
	{
		sprintf(str, "./novoalign -r A 1 -g 99 -x 99 -d %s.JS.ndx -f %s.unmapped_1.fastq -o SAM > %s_1.JS.sam", PName, PName, PName );
		system(str);
		sprintf(str, "./novoalign -r A 1 -g 99 -x 99 -d %s.JS.ndx -f %s.unmapped_2.fastq -o SAM > %s_2.JS.sam", PName, PName, PName );
		system(str);
	}

	for(i=1; i < argc; i++)
	{
		strcpy(fname, argv[i]);
		RemoveFileSubName(fname);

		sprintf(str, "./novoalign -r A 1 -g 99 -x 99 -d %s.JS.ndx -f %s.unmapped.fastq -o SAM > %s.%s.JS.sam", PName, fname, fname, PName );
		system(str);
	}			// Align all reads to short junction sequences of the chimera candidates

	sprintf(str, "cat *.JS.sam > all.%s.JS.sam", PName);
	system(str);
	sprintf(str, "cat all.%s.JS.sam | ./JSParser 95 10 10 > all.%s.JS.Parsered.sam", PName, PName);
	system(str);
	sprintf(str, "cat %s.JS.cleaned.info | ./RetainInList 1 all.%s.JS.Parsered.sam 3 > tmp.info", PName, PName);
	system(str);
	sprintf(str, "./GetInfo tmp.info > %s.info", PName);
	system(str);	// Collect information of chimera candidates with junction supporting reads

	sprintf(str, "cat %s.JS.fa | ./SeqOut %s.info 1 > %s.JS.clear.fa", PName, PName, PName);
	system(str);
	sprintf(str, "./blat %s %s.JS.clear.fa %s.JS.GRCh37.psl", conf.rG, PName, PName);
	system(str);
	sprintf(str, "cat %s.JS.GRCh37.psl | awk '((($11/2)-$12) > 50) && (($13-($11/2)) > 50) && ($1/($13-$12) > 0.8)' > %s.JS.GRCh37.2.psl", PName, PName);
	system(str);
	sprintf(str, "cat %s.JS.GRCh37.2.psl | awk '{print $10}' | sed 's/\\.[0-9]*$//g' > %s.linearJS", PName, PName);
	system(str);
	sprintf(str, "cat %s.info | sed 's/\\.[0-9]*\\t/\\t/g' > %s.tmp.info", PName, PName);
	system(str);
	sprintf(str, "cat %s.tmp.info | ./RmRedundance 1 | awk '{print $1}' > %s.tmp2.info", PName, PName);
	system(str);
	sprintf(str, "cat %s.tmp2.info | ./RemoveInList 1 %s.linearJS 1 > %s.2.info", PName, PName, PName);
	system(str);
	sprintf(str, "cat %s.unmapped.fa | ./SeqOut %s.2.info 1 > %s.2.info.fa", PName, PName, PName);
	system(str);
	sprintf(str, "./blat %s %s.2.info.fa -tileSize=9 -stepSize=9 -repMatch=32768 %s.2.info.GRCh37.psl", conf.rG, PName, PName);
	system(str);
	sprintf(str, "cat %s.2.info.GRCh37.psl | ./PslChimeraFilter 30 1 > %s.2.chi.bed", PName, PName);
	system(str);
	sprintf(str, "cat all.%s.JS.Parsered.sam | ./GetNameB4Dot 3 > temp.list", PName);
	system(str);
	sprintf(str, "cat %s.chi.bed | ./RetainInList 4 temp.list 1 > %s.chi2.bed", PName, PName);
	system(str);
	sprintf(str, "cat %s.chi2.bed | ./RetainInList 4 %s.2.chi.bed 4 > %s.chi3.bed", PName, PName, PName);	
	system(str);	// Preform more sensitive blat alignment 

	sprintf(str, "cat %s.chi3.bed | ./JunctionSite2BED %s %d %d > %s.PreJS2.bed", PName, conf.annotation, conf.fsize, (conf.fsize/2)+(conf.fsize/4)-30, PName);
	system(str);
	sprintf(str, "cat %s.PreJS2.bed | awk '{print $4 \"\\t\" $1 \",\" $2 \",\" $3 \",\" $6 \",\"}' > %s.PreJS2.info", PName, PName);
	system(str);
	sprintf(str, "cat %s.PreJS2.info | ./AssembleExons > %s.PreJS2.info2", PName, PName);
	system(str);
	sprintf(str, "cat %s.PreJS2.info2 | grep \".5p.\" | awk '{print $1 \"\\t\" $2 \"{(})0{(})\"}' > %s.JS2.info_1", PName, PName);
	system(str);
	sprintf(str, "cat %s.PreJS2.info2 | grep \".3p.\" > %s.JS2.info_2", PName, PName);
	system(str);
	sprintf(str, "./AssembleJSeq %s.JS2.info_1 %s.JS2.info_2 > %s.JS2.info", PName, PName, PName);
	system(str);
	sprintf(str, "cat %s.JS2.info | ./RmRedundance 2 > %s.JS2.cleaned.info", PName, PName);
	system(str);
	sprintf(str, "./GetKey %s.JS2.cleaned.info > %s.JS2.preIdx", PName, PName);
	system(str);

	sprintf(str, "cat %s.info | awk '{print $1 \"\\t\" $2 \"-\" $3 $4 \":\" $5 \"-\" $6 $7 \":\" $8}' > %s.JS1.preIdx", PName, PName);
	system(str);

	sprintf(str, "cat %s.JS1.preIdx | sed 's/\\.[0-9]*\\t/\\t/g' | ./RmRedundance 1 > %s.JS1.Idx", PName, PName);
	system(str);

	sprintf(str, "cat %s.JS2.preIdx | ./RetainInList 2 %s.JS1.Idx 2 > %s.JS2.Idx", PName, PName, PName);
	system(str);

	sprintf(str, "./bedtools getfasta -fi %s -bed %s.PreJS2.bed -tab -name -s -fo %s.PreJS2.seq", conf.rG, PName, PName);
	system(str);

	sprintf(str, "cat %s.PreJS2.seq | ./AssembleExons > %s.PreJS2.seq2", PName, PName);
	system(str);

	sprintf(str, "cat %s.PreJS2.seq2 | grep \".5p.\" > %s.JS2.seq_1", PName, PName);
	system(str);

	sprintf(str, "cat %s.PreJS2.seq2 | grep \".3p.\" > %s.JS2.seq_2", PName, PName);
	system(str);

	sprintf(str, "./AssembleJSeq %s.JS2.seq_1 %s.JS2.seq_2 > %s.JS2.seq", PName, PName, PName);
	system(str);

	sprintf(str, "cat %s.JS2.seq | awk '{print \">\" $1 \"\\n\" $2}' > %s.JS2.prefa", PName, PName);
	system(str);

	sprintf(str, "cat %s.JS2.prefa | ./SeqOut %s.JS2.Idx 1 > %s.JS2.fa", PName, PName, PName);
	system(str);

	sprintf(str, "./novoindex %s.JS2.ndx %s.JS2.fa", PName, PName);
	system(str);

	return 0;
}

void PrintUsage()
{
	fprintf(stderr, "Aligning reads (including first data set) to junction sequences by single end manner.\n\n");

	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "NCL_Scan3 [fastq [fastq]]\n\n");

	fprintf(stderr, "Example:\n");
	fprintf(stderr, "NCL_Scan3 Data2.1.fastq Data2.2.fastq Data3.fastq\n");
}

