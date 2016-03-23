#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"
#include "Table.h"
#include "Map.h"

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
	char opath[4096];
	char tmp1[4096] = "";
	char tmp2[4096] = "";
	char tmp3[4096] = "";
	int pathLen;
	int i;

	if(argc != 2)
	{ PrintUsage();	return 1; }

	// Get the path of input file
	strcpy(opath, argv[1]);
	pathLen = strlen(opath);
	for(i=pathLen-1;i>=0;i--)
	{
		if(opath[i] == '/')
		{
			opath[i] = '\0';
			break;
		}
	}

	if(i == -1) // in current directory
	{
		sprintf(tmp1, "Tmp1.getinfo");
		sprintf(tmp2, "Tmp2.getinfo");
		sprintf(tmp3, "Tmp3.getinfo");
	}
	else
	{
		sprintf(tmp1, "%s/Tmp1.getinfo", opath);
		sprintf(tmp2, "%s/Tmp2.getinfo", opath);
		sprintf(tmp3, "%s/Tmp3.getinfo", opath);
	}

	//printf("%s\n", tmp1);
	//printf("%s\n", tmp2);
	//printf("%s\n", tmp3);

	sprintf(str, "cat %s | sed 's/{(})/\t/g' | awk '{print $2}' | sed 's/,/\t/g' > %s", argv[1], tmp1);
	system(str);
	sprintf(str, "cat %s | sed 's/{(})/\t/g' | awk '{print $3}' > %s", argv[1], tmp2);
	system(str);
	sprintf(str, "cat %s | sed 's/{(})/\t/g' | awk '{print $4}' | sed 's/,/\t/g' > %s", argv[1], tmp3);
	system(str);

	if(	( fp0 = fopen(argv[1], "r") ) == NULL )
	{	PrintUsage();	return 1;	}	
	if(	( fp1 = fopen(tmp1, "r") ) == NULL )
	{	PrintUsage();	return 1;	}	
	if(	( fp2 = fopen(tmp2, "r") ) == NULL )
	{	PrintUsage();	return 1;	}	
	if(	( fp3 = fopen(tmp3, "r") ) == NULL )
	{	PrintUsage();	return 1;	}	

	TableRow C0;
	TableRow C1;
	TableRow C2;
	TableRow C3;
	Str2BoolMap theMap;	

	while( C0.Get(fp0) )
	{
		C1.Get(fp1);
		C2.Get(fp2);
		C3.Get(fp3);

		strcpy(info, "");
	
//		sprintf(str, "%s\t", C0.ColCopy(buf0, 0));
//		strcat(info, str);

		if( C1.Column(C1.nColumns()-2)[0] == '+' ) 
			sprintf(str, "%s\t%d\t+\t", C1.ColCopy(buf0, C1.nColumns()-5), C1.ColToI(C1.nColumns()-3));			
		else if( C1.Column(C1.nColumns()-2)[0] == '-' )
			sprintf(str, "%s\t%d\t-\t", C1.ColCopy(buf0, C1.nColumns()-5), C1.ColToI(C1.nColumns()-4)+1);			
		else
		{	ErrMsg("Format error!\n");	return 1;	}	
		strcat(info, str);			

		if( C3.Column(3)[0] == '+' ) 
			sprintf(str, "%s\t%d\t+\t", C3.ColCopy(buf0, 0), C3.ColToI(1)+1);			
		else if( C3.Column(3)[0] == '-' )
			sprintf(str, "%s\t%d\t-\t", C3.ColCopy(buf0, 0), C3.ColToI(2));			
		else
		{	ErrMsg("Format error!\n");	return 1;	}	
		strcat(info, str);			

		sprintf(str, "%d", C2.ColToI(0));
		strcat(info, str);			

		if( theMap.IsInMap(info) != true )
		{
			printf("%s\t%s\n", C0.ColCopy(buf0, 0), info);
			theMap.Add(info);
		}
	}

	sprintf(str, "rm -f %s %s %s", tmp1, tmp2, tmp3);
	system(str);

	return 0;
}

void PrintUsage()
{
	fprintf(stderr, "Example:\n");
	fprintf(stderr, "GetINfo A.info\n");
}

