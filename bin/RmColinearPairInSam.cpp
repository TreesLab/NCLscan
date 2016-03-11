#include <string.h>
#include "Common.h"
#include "Sam.h"

void PrintUsage();

int main(int argc, char **argv)
{
	int flag;
//	char str1[81], str2[81];

	if(argc != 1)
	{	PrintUsage();	return 1;	}	

	SamOutput theSam1;
	SamOutput theSam2;
	while( theSam1.Get() )
	{
		if( theSam1.IsHeadline() )
			continue;

		if( !theSam2.Get() )
			break;
		if( theSam2.IsHeadline() )
			continue;

		if( strcmp(theSam1.qName, theSam2.qName) != 0 )
			fprintf(stderr, "%s and %s do not form a pair.\n", theSam1.qName, theSam2.qName);		

		if( theSam1.Flag & 2 )
			continue;
		if( theSam2.Flag & 2 )
			continue;

		if( strcmp(theSam1.NRNM, "=") == 0 && theSam1.Pos > 0 && theSam1.MPos > 0 )
		{
			if( (theSam1.Flag == 97) && (theSam1.Pos < theSam1.MPos) && ((theSam1.MPos-theSam1.Pos) < 100000) )
				continue;
			if( (theSam1.Flag == 81) && (theSam1.Pos > theSam1.MPos) && ((theSam1.Pos-theSam1.MPos) < 100000) )
				continue;
		}

		if( strcmp(theSam2.NRNM, "=") == 0 && theSam2.Pos > 0 && theSam2.MPos > 0 )
		{
			if( (theSam2.Flag == 97) && (theSam2.Pos < theSam2.MPos) && ((theSam2.MPos-theSam2.Pos) < 100000) )
				continue;
			if( (theSam2.Flag == 81) && (theSam2.Pos > theSam2.MPos) && ((theSam2.Pos-theSam2.MPos) < 100000) )
				continue;
		}

		theSam1.Print();
		theSam2.Print();
	}
}

void PrintUsage()
{
	ErrMsg("Flag [Flag]\n\n");

	ErrMsg("[Flag]        INTEGER     A bitwise flag.\n\n");

	ErrMsg("           1 template having multiple segments in sequencing\n");
	ErrMsg("          10 each segment properly aligned according to the aligner\n");
	ErrMsg("         100 segment unmapped\n");
	ErrMsg("        1000 next segment in the template unmapped\n");
	ErrMsg("       10000 SEQ being reverse complemented\n");
	ErrMsg("      100000 SEQ of the next segment in the template being reversed\n");
	ErrMsg("     1000000 the first segment in the template\n");
	ErrMsg("    10000000 the last segment in the template\n");
	ErrMsg("   100000000 secondary alignment\n");
	ErrMsg("  1000000000 not passing quality controls\n");
	ErrMsg(" 10000000000 PCR or optical duplicate\n");
	ErrMsg("100000000000 supplementary alignment\n\n");

	ErrMsg("Example:\n");
	ErrMsg("cat A.sam | ./Flag 0011 > result.sam\n\n");
}
