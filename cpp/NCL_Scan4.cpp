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
	char fname_1[255];
	char fname_2[255];
	char allnames[32767] = "";
	int i;

	CONFIG conf;

	char Read1[255] = "";
	char Read2[255] = "";
	char PName[255] = "";

	fprintf(stderr, "NCL_Scan4 v0.2.5\n");

	int SpanRange;
	if(argc == 1)
	{ SpanRange = 50; }
	else if(argc == 2)
	{ SpanRange = atoi(argv[1]); }
	else
	{ PrintUsage();	return 1; }

	if( ReadConfig("config.txt", &conf) != 0)
	{	ErrMsg("config.txt can not be read!\n"); return 1;	}	

	if(	( fp = fopen("config.tmp", "r") ) == NULL )
	{		
		ErrMsg("config.tmp can not be found!\n");
		ErrMsg("Please make sure that NCL_Scan2 has been finished properly.\n");
		return 1;	
	}

	fscanf(fp, "%s\n%s\n%s", Read1, Read2, PName);

	fclose(fp);

	system("rm *.JS2.sam");

	sprintf(str, "./novoalign -r A 1 -t 0,1 -d %s.JS2.ndx -f %s.unmapped_1.fastq %s.unmapped_2.fastq --3Prime -o SAM > %s.JS2.sam", PName, PName, PName, PName);
	system(str);

	sprintf(str, "cat %s.JS2.sam | ./JSFilter 10 10 all.%s.1.result > all.%s.1.sam", PName, PName, PName);
	system(str);
	
	// hide temporarily for a new parameter.
	/*for(i=2; i < argc; i+=2)
	{
		strcpy(fname_1, argv[i-1]);
		RemoveFileSubName(fname_1);

		strcpy(fname_2, argv[i]);
		RemoveFileSubName(fname_2);

		sprintf(str, "./novoalign -r A 1 -t 0,1 -d %s.JS2.ndx -f %s.unmapped.fastq %s.unmapped.fastq --3Prime -o SAM > %d.JS2.sam", PName, fname_1, fname_2, (i+2)/2 );
		system(str);

		sprintf(str, "cat %d.JS2.sam | ./JSFilter 10 10 all.%s.%d.result > all.%s.%d.sam", (i+2)/2, PName, (i+2)/2, PName, (i+2)/2);
		system(str);
	}*/	

	system("cat *.JS2.sam > all.JS2.sam");

	//sprintf(str, "cat all.JS2.sam | ./JSFilter 10 50 all.%s.result > %s.ncl.sam", PName, PName);	
	sprintf(str, "cat all.JS2.sam | ./JSFilter 10 %d all.%s.result > %s.ncl.sam", SpanRange, PName, PName);	
	system(str);

	sprintf(str, "cat %s.JS2.cleaned.info | ./RetainInList 1 %s.JS2.Idx 1 > %s.JS2.result.info", PName, PName, PName);
	system(str);
	
	sprintf(str, "./GetInfo %s.JS2.result.info > %s.result.info", PName, PName);
	system(str);

	sprintf(str, "cat %s.result.info | sed 's/\\.[0-9]*\\t/\\t/g' > %s.2b.info", PName, PName);
	system(str);

	sprintf(str, "cat %s.2b.info | ./RetainInList 1 all.%s.result 1 > %s.3.info", PName, PName, PName);
	system(str);

	sprintf(str, "cat %s.3.info | ./InsertInList 1 all.%s.result 1 2 | ./InsertInList 1 all.%s.result 1 3 | ./InsertInList 1 all.%s.result 1 4 > %s.4.info", PName, PName, PName, PName, PName);
	system(str);

	char f_name[255];

	sprintf(str, "cat %s.4.info ", PName);
	sprintf(f_name, "all.%s.1.result", PName);
	for(i=2; (fp = fopen(f_name,"r")) != NULL; i++)
	{
		strcat(str, "| ./InsertInList 1 ");
		strcat(str, f_name);
		strcat(str, " 1 4 0 ");

		sprintf(f_name, "all.%s.%d.result", PName, i);	
		
		fclose(fp);
	}

	strcat(str, "> ");
	strcat(str, PName);
	strcat(str, ".preResult");
	system(str);	

	sprintf(str, "rm *.um3.fastq");
	system(str);
	sprintf(str, "rm *.um3.fa");
	system(str);

	sprintf(str, "cat %s.unmapped_1.fastq | ./FastqOut %s.ncl.sam 1 > %s_1.um3.fastq", PName, PName, PName);
	system(str);
	sprintf(str, "cat %s.unmapped_2.fastq | ./FastqOut %s.ncl.sam 1 > %s_2.um3.fastq", PName, PName, PName);
	system(str);
	sprintf(str, "./AssembleFastq %s_1.um3.fastq %s_2.um3.fastq > %s.um3.fa", PName, PName, PName);
	system(str);
	
	// hide temporarily for a new parameter.
	/*for(i=2; i < argc; i+=2)
	{
		strcpy(fname_1, argv[i-1]);
		RemoveFileSubName(fname_1);

		strcpy(fname_2, argv[i]);
		RemoveFileSubName(fname_2);

		sprintf(str, "cat %s.unmapped.fastq | ./FastqOut %s.ncl.sam 1 > %s.um3.fastq", fname_1, PName, fname_1);
		system(str);
		sprintf(str, "cat %s.unmapped.fastq | ./FastqOut %s.ncl.sam 1 > %s.um3.fastq", fname_2, PName, fname_2);
		system(str);
		sprintf(str, "./AssembleFastq %s.um3.fastq %s.um3.fastq > %s.um3.fa", fname_1, fname_2, fname_1);
		system(str);
	}*/

	sprintf(str, "cat *.um3.fa > all.%s.um3.fa", PName);
	system(str);

	sprintf(str, "rm *.um3.psl");
	system(str);	
	sprintf(str, "./blat %s all.%s.um3.fa -tileSize=9 -stepSize=9 -repMatch=32768 %s.rG.um3.psl", conf.rG, PName, PName);
	system(str);
	sprintf(str, "./blat %s all.%s.um3.fa -tileSize=9 -stepSize=9 -repMatch=32768 %s.coding.um3.psl", conf.rR, PName, PName);
	system(str);
	sprintf(str, "./blat %s all.%s.um3.fa -tileSize=9 -stepSize=9 -repMatch=32768 %s.lncRNA.um3.psl", conf.rN, PName, PName);
	system(str);
	sprintf(str, "./blat RepChrM.fa all.%s.um3.fa -tileSize=9 -stepSize=9 -repMatch=32768 %s.chrM.um3.psl", PName, PName);
	system(str);
	sprintf(str, "cat *.um3.psl | awk '$12 < 5' | awk '($11-$13) < 5' > all.%s.um3.colinear.psl", PName);
	system(str);

	system("rm *.JS2b.sam");

	sprintf(str, "cat %s.JS2.sam | ./RemoveInList 1 all.%s.um3.colinear.psl 10 | awk 'sqrt($9*$9) <= %d' > %s.JS2b.sam", PName, PName, conf.fsize, PName);
	system(str);

	sprintf(str, "cat %s.JS2b.sam | ./JSFilter 10 10 all.%s.1b.result > all.%s.1b.sam", PName, PName, PName);
	system(str);
	
	// hide temporarily for a new parameter.
	/*for(i=2; i < argc; i+=2)
	{
		sprintf(str, "cat %d.JS2.sam | ./RemoveInList 1 %s.colinear.psl 10 > %d.JS2b.sam", (i+2)/2, PName, (i+2)/2);
		system(str);

		sprintf(str, "cat %d.JS2b.sam | ./JSFilter 10 10 all.%s.%db.result > all.%s.%db.sam", (i+2)/2, PName, (i+2)/2, PName, (i+2)/2);
		system(str);
	}*/	

	system("cat *.JS2b.sam > all.JS2b.sam");

	//sprintf(str, "cat all.JS2b.sam | ./JSFilter 10 50 all.%s.b.result > %s.result.sam", PName, PName);	
	sprintf(str, "cat all.JS2b.sam | ./JSFilter 10 %d all.%s.b.result > %s.result.sam", SpanRange, PName, PName);	
	system(str);

	sprintf(str, "cat %s.2b.info | ./RetainInList 1 all.%s.b.result 1 > %s.3b.info", PName, PName, PName);
	system(str);

	sprintf(str, "cat %s.3b.info | ./InsertInList 1 all.%s.b.result 1 2 | ./InsertInList 1 all.%s.b.result 1 3 | ./InsertInList 1 all.%s.b.result 1 4 > %s.4b.info", PName, PName, PName, PName, PName);
	system(str);

	sprintf(str, "cat %s.4b.info ", PName);
	sprintf(f_name, "all.%s.1b.result", PName);
	for(i=2; (fp = fopen(f_name,"r")) != NULL; i++)
	{
		strcat(str, "| ./InsertInList 1 ");
		strcat(str, f_name);
		strcat(str, " 1 4 0 ");

		sprintf(f_name, "all.%s.%db.result", PName, i);	
		
		fclose(fp);
	}

	strcat(str, "| ./RmRedundance 1 | awk '$12 != 0' > ");
	strcat(str, PName);
	strcat(str, ".result.tmp");
	system(str);	

	//fprintf(stderr, "\n\n");
	//fprintf(stderr, "The result will be written to %s.result\n", PName);
	//fprintf(stderr, "See %s.result.sam for the final alignment result.\n\n", PName);

	return 0;
}

void PrintUsage()
{
	fprintf(stderr, "Usage:\n\n");
	fprintf(stderr, "    NCL_Scan4 \n\n");
	fprintf(stderr, "    or \n\n");
	fprintf(stderr, "    NCL_Scan4 [spanning range]\n\n");
	
	fprintf(stderr, "Example: \n");
	fprintf(stderr, "NCL_Scan4 \n");
	fprintf(stderr, "NCL_Scan4 50 \n\n");
}


void oldPrintUsage()
{
	fprintf(stderr, "Aligning reads (including first data set) to junction sequences by single end manner.\n\n");

	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "NCL_Scan4 [fastq [fastq]]\n\n");

	fprintf(stderr, "Example:\n");
	fprintf(stderr, "NCL_Scan4 Data2.1.fastq Data2.2.fastq Data3.fastq\n");
}

