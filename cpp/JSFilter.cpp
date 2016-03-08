#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <limits.h>
#include "Common.h"
#include "Sam.h"

char *KillSubname(char *str1, const char *str2)
{
	int i, len;

	len = strlen(str2);
	strcpy(str1, str2);

	for(i=len-1; i>=0; i--)
	{
		if(str1[i] == '.')
		{
			str1[i] = '\0';
			return str1;
		}
	}

	return str1;
}

struct Alignment
{
	char name[255];
	int len;
	int leftest;
	int rightest;
	int nReads;
};

bool IsGoodRight(const char *CIGAR, const char *MD, int len)
{
	int i, l;
	char str[255];
	char *m;

	l = strlen(CIGAR);

	for(i=l-1; i>=0; i--)
	{
		if( CIGAR[i] < '0' || CIGAR[i] > '9' )
		{
			if( CIGAR[i] == 'M' )
				break;
			else
				return false;
		}			
	}

	l = strlen(MD);
	strcpy(str, MD);
	m = str;

	for(i=l-1; i>=0; i--)
	{
		if( str[i] < '0' || str[i] > '9' )
		{
			m = str+i+1;
			break;
		}
	}

	if( atoi(m) < len )
		return false;

	return true;
}

bool IsGoodLeft(const char *CIGAR, const char *MD, int len)
{
	int i, l;
	char str[255];

	l = strlen(CIGAR);

	for(i=0; i<l; i++)
	{
		if( CIGAR[i] < '0' || CIGAR[i] > '9' )
		{
			if( CIGAR[i] == 'M' )
				break;
			else
				return false;
		}			
	}

	l = strlen(MD);
	strcpy(str, MD);

	for(i=0; i<l; i++)
	{
		if( str[i] < '0' || str[i] > '9' )
		{
			str[i] = '\0';
			break;
		}
	}

	if( atoi(str) < len )
		return false;

	return true;
}

void PrintUsage();

using namespace std; 

int main(int argc, char **argv)
{
	char str[32767];
	int i;
	int jLen, aLen;
	FILE *fp;

	map <string, int> themap;
	Alignment *A = new Alignment[200000];

	for(i=0; i<200000; i++)
	{
		strcpy(A[i].name, "");
		A[i].len = 0;
		A[i].leftest = 0;
		A[i].rightest = 0;
		A[i].nReads = 0;
	}

	if(argc != 4)
	{	PrintUsage();	return 1;	}	
	
	jLen = atoi(argv[1]);
	aLen = atoi(argv[2]);

	if(	( fp = fopen(argv[3], "w") ) == NULL )
	{	ErrMsg("Can not open file for writing.\n");		return 1;	}	

	i = 0;
	for( SamOutput theSam1; theSam1.Get(); )
	{
		int r1, r2;
		SamOutput theSam2;

		if(theSam1.LN != 0)
		{
			if( themap.find(theSam1.SN) == themap.end() )
			{
				themap[theSam1.SN] = i;
				
				strcpy( A[i].name, theSam1.SN);
				A[i].len = theSam1.LN;
				A[i].leftest = (theSam1.LN+1)/2;
				A[i].rightest = (theSam1.LN+1)/2;
				i++;
			}
			continue;
		}

		if( theSam1.IsHeadline() )
			continue;

		if( !theSam2.Get() )
		{
			ErrMsg("Format Error! Not paired-end!\n");
			return -1;
		}

		if( (theSam1.Flag & 0x0c) || (theSam2.Flag & 0x0c) )
			continue;

		if( (theSam1.Flag & 0x02) == 0 || (theSam2.Flag & 0x02) == 0 )
			continue;

		if( theSam1.Z3 == Invalid || theSam2.Z3 == Invalid )
		{
			ErrMsg("Format Error! Z3 is not available!\n");
			return -1;
		}

		if( themap.find(theSam1.rName) != themap.end() )
			r1 = themap.find(theSam1.rName)->second;

		if( themap.find(theSam2.rName) != themap.end() )
			r2 = themap.find(theSam2.rName)->second;

		if(r1 != r2)
			ErrMsg("Format Error!\n");	

		//if(theSam1.Pos < theSam2.Z3) // bug?
		if(theSam1.Pos < theSam2.Pos) // fix
		{
			if( ((A[r1].len)/2)-theSam1.Pos+1 < jLen )
				continue;
			if( theSam2.Z3-((A[r1].len)/2) < jLen )
				continue;
			
			if( !IsGoodLeft(theSam1.CIGAR, theSam1.MD, jLen) )
				continue;			
			if( !IsGoodRight(theSam2.CIGAR, theSam2.MD, jLen) )
				continue;

			if( theSam1.Pos < A[r1].leftest )
				A[r1].leftest = theSam1.Pos;
			if( theSam2.Z3 > A[r1].rightest )
				A[r1].rightest = theSam2.Z3;
		}
		else
		{
			if( ((A[r1].len)/2)-theSam2.Pos+1 < jLen )
				continue;
			if( theSam1.Z3-((A[r1].len)/2) < jLen )
				continue;

			if( !IsGoodLeft(theSam2.CIGAR, theSam2.MD, jLen) )
				continue;			
			if( !IsGoodRight(theSam1.CIGAR, theSam1.MD, jLen) )
				continue;

			if( theSam2.Pos < A[r1].leftest )
				A[r1].leftest = theSam2.Pos;
			if( theSam1.Z3 > A[r1].rightest )
				A[r1].rightest = theSam1.Z3;
		}

		A[r1].nReads++;

		theSam1.Print();
		theSam2.Print();
	}


	Alignment *A2 = new Alignment[200000];
	map <string, int> themap2;
	int c, d;

	for(i=0; i<200000; i++)
	{
		strcpy(A2[i].name, "");
		A2[i].len = 0;
		A2[i].leftest = INT_MAX;
		A2[i].rightest = 0;
		A2[i].nReads = 0;
	}

	c=0;
	for(i=0; A[i].len != 0; i++)
	{
		if( themap2.find(KillSubname(str, A[i].name) ) == themap2.end() )
		{
			themap2[KillSubname(str, A[i].name)] = c;
			strcpy( A2[c].name, KillSubname(str, A[i].name) );
			A2[c].len += A[i].len;
			c++;
		}

		d = themap2.find(KillSubname(str, A[i].name))->second;

		if( A2[d].leftest > A[i].leftest )
			A2[d].leftest = A[i].leftest;

		if( A2[d].rightest < A[i].rightest )
			A2[d].rightest = A[i].rightest; 

		A2[d].nReads += A[i].nReads;
	}

	for(i=0; A2[i].len != 0; i++)
	{
		int lLen, rLen;
	
		lLen = (A2[i].len/2)+1 - A2[i].leftest;
		rLen = A2[i].rightest - (A2[i].len/2);

		if( lLen < aLen || rLen < aLen )
			continue;
		
		fprintf(fp, "%s\t%d\t%d\t%d\n", A2[i].name, lLen, rLen, A2[i].nReads);
	}


	fclose(fp);

	delete [] A;
	delete [] A2;
}

void PrintUsage()
{
	ErrMsg("\n");
	ErrMsg("JSFilter [J. length] [A. length] [Result]\n\n");

	ErrMsg("[J. length]        INTEGER     The valid length of cross-junction alignemnts.\n");
	ErrMsg("[A. length]        INTEGER     The minimal length of alignemnts in both sides.\n");
	ErrMsg("[Result]           STRING      The file name that reporting results.\n\n");

	ErrMsg("Example: (for all)\n");
	ErrMsg("cat all.A.sam | ./JSFilter 10 50 A.result > A.filtered.sam \n\n");
	ErrMsg("Example: (for single data set)\n");
	ErrMsg("cat a1.sam | ./JSFilter 10 10 a1.result > a1.filtered.sam\n\n");

}
