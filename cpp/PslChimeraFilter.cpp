#include <stdlib.h>
#include "Common.h"
#include "Table.h"

struct PSL
{
	int match;

	char qName[255];
	int qStart;
	int qEnd;
	int qSize;	
	char strand;
	char tName[255];
	int tStart;
	int tEnd;
};

struct BED
{
	int match;

	char seqname[255];
	int start;
	int end;
	char name[255];

	char strand;

	int qStart;
	int qEnd;

	int SecondBest_match;
	int SecondBest_qStart;
	int SecondBest_qEnd;

	int qSize;
};

void initBED(BED *B)
{
	B->match = 0;
	B->seqname[0] = '\0';
	B->start = 0;
	B->end = 0;
	B->strand = ' ';
	B->qStart = 1;
	B->qEnd = 0;
	B->SecondBest_match = 0;
	B->SecondBest_qStart = 1;
	B->SecondBest_qEnd = 0;
	B->qSize = 0;
}

void PrintResult(BED *b5, BED *b3, int minMatches, int minDif)
{
	if(b5->match == 0 || b3->match == 0 )
		return;

	if(b5->match < minMatches || b3->match < minMatches )
		return;
	
	if( ((b5->match - b5->SecondBest_match) < minDif) || ((b3->match - b3->SecondBest_match) < minDif) )
		return;

	if( (b3->qStart - b5->qStart + 1) < 20 )
		return;

	if( (b3->qEnd - b5->qEnd + 1) < 20 )
		return;

	if( strcmp(b5->seqname, b3->seqname) == 0 && (b5->strand == b3->strand) )
	{
		if(b5->strand == '+')
		{
			if( (b3->start-b5->end > 0) && (b3->start-b5->end < 2000000) )
				return;
		}
		else
		{
			if( (b5->start-b3->end > 0) && (b5->start-b3->end < 2000000) )
				return;
		}
	}

	printf("%s\t%d\t%d\t%s\t0\t%c\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", b5->seqname, b5->start, b5->end, b5->name, b5->strand, 
		b5->match, b5->qStart-1, b5->qEnd, b5->SecondBest_match, b5->SecondBest_qStart-1, b5->SecondBest_qEnd, b5->qSize);
	printf("%s\t%d\t%d\t%s\t0\t%c\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", b3->seqname, b3->start, b3->end, b3->name, b3->strand, 
		b3->match, b3->qStart-1, b3->qEnd, b3->SecondBest_match, b3->SecondBest_qStart-1, b3->SecondBest_qEnd, b3->qSize);
}

bool ReadPsl(PSL *psl, TableRow *T)
{
	if(T->nColumns() != 21)
		return false;	
	
	psl->match = T->ColToI(0) + T->ColToI(2);

	if(psl->match == 0 )
		return false;

	psl->strand = T->Column(8)[0];

	T->ColCopy(psl->qName, 9);
	psl->qSize = T->ColToI(10);
	psl->qStart = T->ColToI(11) + 1;
	psl->qEnd = T->ColToI(12);

	T->ColCopy(psl->tName, 13);
	psl->tStart = T->ColToI(15) + 1;
	psl->tEnd = T->ColToI(16);

	return true;
}

void PrintUsage();

int main(int argc, char **argv)
{
	PSL psl;
	char thisName[255] = "";
	char preName[255] = "";
	BED FiveEnd;
	BED ThreeEnd;
	bool linear;
	int minMatches;
	int minDif;

	ErrMsg("PslChimeraFilter v0.4\n\n");

	if(argc != 3)
	{	PrintUsage();	return 1;	}

	minMatches = atoi(argv[1]);
	minDif = atoi(argv[2]);

	initBED(&FiveEnd);
	initBED(&ThreeEnd);

	linear = false;
	for( TableRow theRow; theRow.Get(); )
	{
		if( ReadPsl(&psl, &theRow) == false )
			continue;
		
		strcpy(thisName, psl.qName);

		if( strcmp(thisName, preName) != 0 )
		{
			if(linear == false)
				PrintResult(&FiveEnd, &ThreeEnd, minMatches, minDif);

			initBED(&FiveEnd);
			initBED(&ThreeEnd);

			linear = false;
		}

		strcpy(preName, thisName);

		if( psl.qStart <= 10 && (psl.qSize-psl.qEnd+1) <= 10 )	
			linear = true;

		if( psl.qStart <= 10 )
		{
			if( psl.match >= FiveEnd.match )
			{
				FiveEnd.SecondBest_match = FiveEnd.match;
				FiveEnd.SecondBest_qStart = FiveEnd.qStart;
				FiveEnd.SecondBest_qEnd = FiveEnd.qEnd;

				FiveEnd.match = psl.match;
				FiveEnd.qSize = psl.qSize;

				strcpy(FiveEnd.seqname, psl.tName); 

				FiveEnd.start = psl.tStart - 1;
				FiveEnd.end = psl.tEnd;

				strcpy(FiveEnd.name, psl.qName);
				FiveEnd.strand = psl.strand;
				FiveEnd.qStart = psl.qStart;
				FiveEnd.qEnd = psl.qEnd;
			}
			else if( psl.match >= FiveEnd.SecondBest_match )
			{
				FiveEnd.SecondBest_match = psl.match;
				FiveEnd.SecondBest_qStart = psl.qStart;
				FiveEnd.SecondBest_qEnd = psl.qEnd;
			}
		}			
		
		if( (psl.qSize-psl.qEnd+1) <= 10 )
		{
			if( psl.match >= ThreeEnd.match )
			{
				ThreeEnd.SecondBest_match = ThreeEnd.match;
				ThreeEnd.SecondBest_qStart = ThreeEnd.qStart;
				ThreeEnd.SecondBest_qEnd = ThreeEnd.qEnd;

				ThreeEnd.match = psl.match;
				ThreeEnd.qSize = psl.qSize;

				strcpy(ThreeEnd.seqname, psl.tName); 

				ThreeEnd.start = psl.tStart - 1;
				ThreeEnd.end = psl.tEnd;

				strcpy(ThreeEnd.name, psl.qName);
				ThreeEnd.strand = psl.strand;
				ThreeEnd.qStart = psl.qStart;
				ThreeEnd.qEnd = psl.qEnd;
			}
			else if( psl.match >= ThreeEnd.SecondBest_match )
			{
				ThreeEnd.SecondBest_match = psl.match;
				ThreeEnd.SecondBest_qStart = psl.qStart;
				ThreeEnd.SecondBest_qEnd = psl.qEnd;
			}

		}
	}
	PrintResult(&FiveEnd, &ThreeEnd, minMatches, minDif);

	return 0;
}

void PrintUsage()
{
	ErrMsg("PslChimeraFilter [Matches] [Gap to 2nd best hit]\n\n");

	ErrMsg("[Matches]             INTEGER     The minimal number of matches allowed in a reliable alignment.\n\n");
	ErrMsg("[Gap to 2nd best hit] INTEGER     The matches of the best alignment should be greater than the second by a given value.\n\n");

	ErrMsg("The results are listed in pairs.\n");
	ErrMsg("The first line of a pair represents the position of its 5' end,\n");
	ErrMsg("second line the 3' end.\n\n");
//	ErrMsg("The 5th column of the resulting BED indicates the number of multiple hits.\n\n");

	ErrMsg("Example:\n");
	ErrMsg("cat Input.psl | ./PslChimeraFilter 30 5 > Output.bed\n\n");
}
