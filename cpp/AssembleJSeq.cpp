#include <stdio.h>
#include <string.h>
#include "Common.h"
#include "Table.h"

struct SEQ
{
	char name[255];
	char seq[2048];
};

bool LoadSEQ(SEQ *S, FILE *fp) 
{
	TableRow theRow;

	if( !theRow.Get(fp) )
		return false;

	if( theRow.nColumns() < 2 )
		return false;

	theRow.ColCopy(S->name, 0);
	theRow.ColCopy(S->seq, 1);

	return true;
}

void PrintUsage();

int main(int argc, char **argv)
{
	int i, j;
	int i5, i3;
	char str[32767];
	FILE *fp_5p;
	FILE *fp_3p;

	SEQ SEQs1[1024];
	SEQ SEQs2[1024];

	if(argc != 3)
	{	PrintUsage();	return 1;	}	

	if(	( fp_5p = fopen("RetainInList", "r") ) == NULL )
	{		
		ErrMsg("This program can not work along.\n It must work with the problem RetainInList.\n\n");
		return 1;	
	}	

	sprintf(str, "cat %s | sed 's/.5p./\t/g' | awk '{print $1 \"\\t\" $3}' > temp5p0.tmp", argv[1]);
	system(str);
	sprintf(str, "cat %s | sed 's/.3p./\t/g' | awk '{print $1 \"\\t\" $3}' > temp3p0.tmp", argv[2]);
	system(str);
	system("cat temp5p0.tmp | ./RetainInList 1 temp3p0.tmp 1 > temp5p.tmp");
	system("cat temp3p0.tmp | ./RetainInList 1 temp5p0.tmp 1 > temp3p.tmp");

	if(	( fp_5p = fopen("temp5p.tmp", "r") ) == NULL )
	{	PrintUsage();	return 1;	}	

	if(	( fp_3p = fopen("temp3p.tmp", "r") ) == NULL )
	{	PrintUsage();	return 1;	}	

	for(i=0; i<1024; i++)
	{
		strcpy(SEQs1[i].name, "");
		strcpy(SEQs1[i].seq, "");
		strcpy(SEQs2[i].name, "");
		strcpy(SEQs2[i].seq, "");
	}

	if( !LoadSEQ(&SEQs1[0] ,fp_5p) ) 
		ErrMsg("Format Error!\n");
	if( !LoadSEQ(&SEQs2[0] ,fp_3p) ) 
		ErrMsg("Format Error!\n");

	while(feof(fp_5p) == 0 || feof(fp_3p) == 0)
	{
		for(i5=1; LoadSEQ(&SEQs1[i5], fp_5p); i5++)
		{
			if( strcmp(SEQs1[i5].name, SEQs1[0].name) != 0 )
				break;
		}

		for(i3=1; LoadSEQ(&SEQs2[i3], fp_3p); i3++)
		{
			if( strcmp(SEQs2[i3].name, SEQs2[0].name) != 0 )
				break;
		}

		for(i=0; i<i5; i++)
		{
			for(j=0; j<i3; j++)
			{
				if( strcmp(SEQs1[i].name, SEQs2[j].name) != 0 )
					ErrMsg("Format Error!\n");

				printf("%s.%d\t%s%s\n", SEQs1[i].name, i*i3+j, SEQs1[i].seq, SEQs2[j].seq);
			}
		}

		strcpy(SEQs1[0].name, SEQs1[i5].name);	
		strcpy(SEQs1[0].seq, SEQs1[i5].seq);	
		strcpy(SEQs2[0].name, SEQs2[i3].name);	
		strcpy(SEQs2[0].seq, SEQs2[i3].seq);		
	}

	fclose(fp_5p);
	fclose(fp_3p);
	
//	Times();
	return 0;
}

void PrintUsage()
{
	ErrMsg("Assemble the junction sequences from two parts.\n");
	ErrMsg("This program can not work along.\n It must work with the problem RetainInList.\n\n");
	
	ErrMsg("Example:\n");
	ErrMsg("./AssembleJSeq A.5p.seq A.3p.seq > Output.fa\n\n");
}
