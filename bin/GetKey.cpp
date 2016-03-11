#include <stdio.h>
#include <stdlib.h>
#include "Common.h"
#include "Table.h"
#include "Map.h"
#include "NCL_head.h"

void PrintUsage();

int main(int argc, char **argv)
{
	FILE *fp0;
	FILE *fp1;
	FILE *fp2;
	FILE *fp3;
	char str[4096];
	char info[32767];
	char buf0[255];
	char buf1[255];
	char buf2[255];
	char buf3[255];

	if(argc != 2)
	{ PrintUsage();	return 1; }

	sprintf(str, "cat %s | sed 's/{(})/\t/g' | awk '{print $2}' | sed 's/,/\t/g' > Tmp1.getkey", argv[1]);	
	system(str);
	sprintf(str, "cat %s | sed 's/{(})/\t/g' | awk '{print $3}' > Tmp2.getkey", argv[1]);	
	system(str);
	sprintf(str, "cat %s | sed 's/{(})/\t/g' | awk '{print $4}' | sed 's/,/\t/g' > Tmp3.getkey", argv[1]);
	system(str);

	if(	( fp0 = fopen(argv[1], "r") ) == NULL )
	{	PrintUsage();	return 1;	}	
	if(	( fp1 = fopen("Tmp1.getkey", "r") ) == NULL )
	{	PrintUsage();	return 1;	}	
	if(	( fp2 = fopen("Tmp2.getkey", "r") ) == NULL )
	{	PrintUsage();	return 1;	}	
	if(	( fp3 = fopen("Tmp3.getkey", "r") ) == NULL )
	{	PrintUsage();	return 1;	}	

	TableRow C0;
	TableRow C1;
	TableRow C2;
	TableRow C3;

	while( C0.Get(fp0) )
	{
		C1.Get(fp1);
		C2.Get(fp2);
		C3.Get(fp3);

		strcpy(info, "");
	
		if( C1.Column(C1.nColumns()-2)[0] == '+' ) 
			sprintf(str, "%s-%d+:", C1.ColCopy(buf0, C1.nColumns()-5), C1.ColToI(C1.nColumns()-3));			
		else if( C1.Column(C1.nColumns()-2)[0] == '-' )
			sprintf(str, "%s-%d-:", C1.ColCopy(buf0, C1.nColumns()-5), C1.ColToI(C1.nColumns()-4)+1);			
		else
		{	ErrMsg("Format error!\n");	return 1;	}	
		strcat(info, str);			

		if( C3.Column(3)[0] == '+' ) 
			sprintf(str, "%s-%d+:", C3.ColCopy(buf0, 0), C3.ColToI(1)+1);			
		else if( C3.Column(3)[0] == '-' )
			sprintf(str, "%s-%d-:", C3.ColCopy(buf0, 0), C3.ColToI(2));			
		else
		{	ErrMsg("Format error!\n");	return 1;	}	
		strcat(info, str);			

		sprintf(str, "%d", C2.ColToI(0));
		strcat(info, str);			

		printf("%s\t%s\n", C0.ColCopy(buf0, 0), info);
	}	
	system("rm Tmp1.getkey Tmp2.getkey Tmp3.getkey");

	return 0;
}

void PrintUsage()
{
	fprintf(stderr, "Example:\n");
	fprintf(stderr, "GetKey A.info\n");
}

