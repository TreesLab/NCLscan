#include <string.h>
#include <stdio.h>
#include "Common.h"

using namespace std;

char *ItoA( int value, char* result) 
{
	sprintf(result, "%d", value);
	return result;
}

char *ItoA( int value, char* result, int base) 
{
	sprintf(result, "%d", value);
	return result;
}


char *StrRev(char *rstr, const char *str)
{
	int i;
	int len;

	len = strlen(str);

	rstr[len] = '\0';

	if(str[len-1] == '\n')
	{
		rstr[len-1] = '\n';
		len--;
	}


	for(i=0; i<len; i++)
		rstr[i] = str[len-i-1];

	return rstr;
}

int Popen(const char *com, const char *filename)
{
	FILE *fp;
	FILE *outfile;
	char buff[MaxLineSize];

	if(!(fp = popen(com, "r")))	
		{return 1;}		

	if(	( outfile = fopen(filename, "w") ) == NULL )
	{	ErrMsg("Popen error: can not open a file for writing!\n");	return -1;	}
	
	while(fgets(buff, sizeof(buff), fp)!=NULL)
	        fprintf(outfile, "%s", buff);

	pclose(fp);
	fclose(outfile);
	return 0;
}

int Binary2Int(const char *B)
{
	int base;
	int i;
	int len;
	int sum;

	len = strlen(B);

	base = 1;
	sum = 0;
	for(i=len-1; i>=0; i--)
	{
		switch(B[i])
		{
			case '1':
				sum += base;
				break;
			case '0':
				break;
			default:
				return -1;	
		}

		base = base * 2;		
	}
	
	return sum;
}
