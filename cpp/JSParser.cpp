#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include "Common.h"
#include "Sam.h"

void PrintUsage();

using namespace std; 

int main(int argc, char **argv)
{
	char str[501];
	map <string, int> themap;

	if(argc != 4)
	{	PrintUsage();	return 1;	}	

	for( SamOutput theSam; theSam.Get(); )
	{
		int i;
		int qLen, rLen;
		char MD[1024];
		char *sufMD;
		int MDlen;

		if(theSam.LN != 0)
		{
			if( themap.find(theSam.SN) == themap.end() )
				themap[theSam.SN] = theSam.LN;
			continue;
		}

		if( themap.find(theSam.rName) != themap.end() )
			rLen = themap.find(theSam.rName)->second;

		qLen = strlen(theSam.Qual);

		if( strncmp( theSam.CIGAR, ItoA(qLen, str, 10), strlen(ItoA(qLen, str, 10)) ) != 0 )
			continue;

		strcpy(MD, theSam.MD);
		sufMD = MD;
		MDlen = strlen(MD);

		for(i=MDlen-1; i >= 0; i--)
		{
			if( !isdigit(MD[i]) )
			{
				sufMD = MD + i + 1;
				break;
			}			
		} 

		for(i=0; i<MDlen; i++)
		{
			if( !isdigit(MD[i]) )
			{
				MD[i] = '\0';
				break;
			} 
		}

		if( (theSam.Pos <= (rLen/2)-atoi(argv[2])+1) && (theSam.Pos > (rLen/2)-qLen+atoi(argv[2])) )
		{
			if( (atoi(MD) > atoi(argv[3])) && (atoi(sufMD) > atoi(argv[3])) && theSam.NM <= ((qLen*(100-atoi(argv[1])))/100) ) 
				theSam.Print();
		}
	}
}

void PrintUsage()
{
	ErrMsg("\n");
	ErrMsg("ChimeraParser [Identity] [Length] [Length]\n\n");

	ErrMsg("[Identity]      INTEGER     An identity between 0 and 100.\n");
	ErrMsg("[Length]        INTEGER     The minimal length of bases acrossing the boundary.\n\n");
	ErrMsg("[Length]        INTEGER     The minimal length of both sides.\n\n");

	ErrMsg("Example:\n");
	ErrMsg("cat Reads_vs_JS.sam | ./JSParser 95 10 5 > result.sam\n\n");
}
