#include <string.h>
#include <stdlib.h>
#include "Common.h"
#include "Table.h"
#include "Annotation.h"

void PrintUsage();

int main(int argc, char **argv)
{
	FILE *fp = NULL;
	char str1[255], str2[255];
	bool direction;
	int loci;
	char strand;
	int extend;
	int qSize;
	int qStart;
	int qEnd;

	ErrMsg("JunctionSite2BED v0.3\n\n");

	if(argc != 4)
	{	PrintUsage();	return 1;	}	

	if(	( fp = fopen(argv[1], "r") ) == NULL )
	{	PrintUsage();	return 1;	}	

	Mini_GTF Annotation(fp);

	TableRow theRow1;
	TableRow theRow2;
	while( theRow1.Get() && theRow2.Get() )
	{
		if( (theRow1.nColumns() < 6) || (theRow2.nColumns() < 6) )
		{
			ErrMsg("Error: the number of columns < 6.\n");
			ErrMsg("The input must be in BED format with at least 6 columns.\n");
			ErrMsg("Ex.	chr1	1331	1331	name1	0	+\n");
			ErrMsg("	chr2	1234	1235	name1	0	-\n");
			continue;
		}

		if( strcmp(theRow1.ColCopy(str1, 3), theRow1.ColCopy(str2, 3)) != 0 )
		{
			ErrMsg("Error: input does not appear in pairs.\n");
			return 1;
		}

		theRow1.ColCopy(str2, 3);
		strcat(str2, ".5p");

		strand = *theRow1.Column(5);

		if(strand == '+')
			loci = atoi(theRow1.Column(2)) - 30;
		else
			loci = atoi(theRow1.Column(1)) + 30;

		qSize = theRow2.ColToI(12);
		qStart = theRow2.ColToI(7);	// the opposite qStart
		qEnd = theRow2.ColToI(8);

		if( ((qSize/2) - 5) > qStart )
			extend = qStart + 30;
		else
			extend = atoi(argv[3]) - (qSize-qStart); 

		extend -= theRow1.ColToI(8);

		if(extend < 50)
			extend = 50;
			
		Annotation.PrintInBedByNearbyJunctions(theRow1.ColCopy(str1, 0), loci, strand, ThreePrimeEnd, atoi(argv[2]), str2, extend);

		theRow2.ColCopy(str2, 3);
		strcat(str2, ".3p");
		loci = atoi(theRow2.Column(2));
		strand = *theRow2.Column(5);

		if(strand == '+')
			loci = atoi(theRow2.Column(1)) + 30;
		else
			loci = atoi(theRow2.Column(2)) - 30;

		qSize = theRow1.ColToI(12);	
		qStart = theRow1.ColToI(7);	// the opposite qStart
		qEnd = theRow1.ColToI(8);

		if( qEnd > (qSize/2) + 5 )
			extend = qSize - qEnd + 31;
		else
			extend = atoi(argv[3]) - qEnd + 1; 

		extend -= theRow2.ColToI(12)-theRow2.ColToI(7);

		if(extend < 50)
			extend = 50;

		Annotation.PrintInBedByNearbyJunctions(theRow2.ColCopy(str1, 0), loci, strand, FivePrimeEnd, atoi(argv[2]), str2, extend);

/*		if( *theRow.Column(4) == '5' )
			direction = FivePrimeEnd;
		else if( *theRow.Column(4) == '3' )
			direction = ThreePrimeEnd;		
		else
			ErrMsg("Error: the 5th column must be 0 or 1.\n");
*/
//		Annotation.PrintInBedByLength(theRow.ColCopy(str1, 0), 1457466, *theRow.Column(5), direction, 3500, theRow.ColCopy(str2, 3));
//		Annotation.PrintInBedByLength(theRow.ColCopy(str1, 0), atoi(theRow.Column(2)), *theRow.Column(5), direction, 3500, theRow.ColCopy(str2, 3));
//		Annotation.PrintInBedByNearbyJunctions(theRow.ColCopy(str1, 0), atoi(theRow.Column(2)), *theRow.Column(5), direction, atoi(argv[2]), theRow.ColCopy(str2, 3), atoi(argv[3]));
	}

	fclose(fp);
	return 0;
}

void PrintUsage()
{
	ErrMsg("Get annotation of exonic regions beginnig from exons colsed to a given position.\n\n");
//	ErrMsg("The input should be sorted by the 3rd column.\n\n");

	ErrMsg("Example:\n");
	ErrMsg("cat Pos.bed | ./JunctionSite2BED gencode.v18.annotation.gtf 500 300\n\n");
}  
