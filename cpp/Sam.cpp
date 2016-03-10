#include <stdio.h>
#include <string.h>
#include "Sam.h"
#include "Common.h"
#include "Table.h"

bool SamOutput::Get(char *str)
{
	int i;
	TableRow theRow;
	int nCol;

	Clear();
	Init();

	nCol = sscanf(str, "%s %d %s %d %d %s %s %d %d %s %s", 
			qName, &Flag, rName, &Pos, &MapQ, CIGAR, NRNM, &MPos, &ISize, Seq, Qual );

	if( theRow.Get(str) == false )
		return false;

	LN = 0;
	strcpy( SN, "");

	if( str[0] == '@' )
		headline = true;
	else
		headline = false;

	if( ( str[0] == '@' ) || ( nCol == EOF ) || ( nCol < 11 ) )
	{
		empty = true;
		strcpy( PG, "");
		strcpy( MD, "");
		strcpy( CS, "");
		strcpy( CQ, "");
		strcpy( CC, "");
		strcpy( XA, "");
		strcpy( XE, "");

		for( i = 0; theRow.Column(i) != NULL; i++ )
		{
			if( strncmp(theRow.Column(i), "SN", 2 ) == 0 )
				ColCpy( SN, theRow.Column(i) + 3 );
			else if( strncmp(theRow.Column(i), "LN", 2 ) == 0 )
				LN = atoi( theRow.Column(i) + 3 );
		}

		return true;
	}

	for( i = 0; theRow.Column(i) != NULL; i++ )
	{
		if( strncmp(theRow.Column(i), "PG", 2 ) == 0 )
			ColCpy( PG, theRow.Column(i) + 5 );
		else if( strncmp(theRow.Column(i), "AS", 2 ) == 0 )
			AS = atoi( theRow.Column(i) + 5 );
		else if( strncmp(theRow.Column(i), "NM", 2 ) == 0 )
			NM = atoi( theRow.Column(i) + 5 );
		else if( strncmp(theRow.Column(i), "NH", 2 ) == 0 )
			NH = atoi( theRow.Column(i) + 5 );
		else if( strncmp(theRow.Column(i), "IH", 2 ) == 0 )
			IH = atoi( theRow.Column(i) + 5 );
		else if( strncmp(theRow.Column(i), "HI", 2 ) == 0 )
			HI = atoi( theRow.Column(i) + 5 );
		else if( strncmp(theRow.Column(i), "MD", 2 ) == 0 )
			ColCpy( MD, theRow.Column(i) + 5 );
		else if( strncmp(theRow.Column(i), "CS", 2 ) == 0 )
			ColCpy( CS, theRow.Column(i) + 5 );
		else if( strncmp(theRow.Column(i), "CQ", 2 ) == 0 )
			ColCpy( CQ, theRow.Column(i) + 5 );
		else if( strncmp(theRow.Column(i), "CM", 2 ) == 0 )
			CM = atoi( theRow.Column(i) + 5 );
		else if( strncmp(theRow.Column(i), "CC", 2 ) == 0 )
			ColCpy( CC, theRow.Column(i) + 5 );
		else if( strncmp(theRow.Column(i), "CP", 2 ) == 0 )
			CP = atoi( theRow.Column(i) + 5 );
		else if( strncmp(theRow.Column(i), "XA", 2 ) == 0 )
			ColCpy( XA, theRow.Column(i) + 5 );
		else if( strncmp(theRow.Column(i), "XE", 2 ) == 0 )
			ColCpy( XE, theRow.Column(i) + 5 );
		else if( strncmp(theRow.Column(i), "Z3", 2 ) == 0 )
			Z3 = atoi( theRow.Column(i) + 5 );
	} 
	
	if( (Flag&0x0010) == 0 )
		strand = 0;
	else
		strand = 1;

	empty = false;
	return true;
}

bool SamOutput::Get(FILE *fp)
{
	char str[MaxSamLen+1];

	if( fgets(str, MaxLineSize, fp) == NULL )
		return false;

	return Get(str);
}

int SamOutput::SprintEss(char *str)
{
	if( IsEmpty() )
	{
		strcpy( str, "" );
		return 0;
	}

	return(	sprintf(str, "%s\t%d\t%s\t%d\t%d\t%s\t%s\t%d\t%d\t%s\t%s", 
			qName, Flag, rName, Pos, MapQ, CIGAR, NRNM, MPos, ISize, Seq, Qual ) );
}

void SamOutput::Print()
{
	if( IsEmpty() )
		return;

	printf("%s\t%d\t%s\t%d\t%d\t%s\t%s\t%d\t%d\t%s\t%s", 
			qName, Flag, rName, Pos, MapQ, CIGAR, NRNM, MPos, ISize, Seq, Qual );

	if( PG[0] != '\0' )
		printf("\tPG:Z:%s", PG);
	if( AS != Invalid )
		printf("\tAS:i:%d", AS);
	if( NM != Invalid )
		printf("\tNM:i:%d", NM);
	if( NH != Invalid )
		printf("\tNH:i:%d", NH);
	if( IH != Invalid )
		printf("\tIH:i:%d", IH);
	if( HI != Invalid )
		printf("\tHI:i:%d", HI);
	if( MD[0] != '\0' )
		printf("\tMD:Z:%s", MD);
	if( CS[0] != '\0' )
		printf("\tCS:Z:%s", CS);
	if( CQ[0] != '\0' )
		printf("\tCQ:Z:%s", CQ);
	if( CM != Invalid )
		printf("\tCM:i:%d", CM);
	if( CC[0] != '\0' )
		printf("\tCC:Z:%s", CC);
	if( CP != Invalid )
		printf("\tCP:i:%d", CP);
	if( XA[0] != '\0' )
		printf("\tXA:Z:%s", XA);
	if( XE[0] != '\0' )
		printf("\tXE:Z:%s", XE);
	
	printf("\n");
}

void SamOutput::Init()
{
	strcpy( qName, "" );
	Flag = Invalid;
	strcpy( rName, "" );
	Pos = Invalid;
	MapQ = Invalid;
	strcpy( CIGAR, "" );
	strcpy( NRNM, "" );
	MPos = Invalid;
	ISize = Invalid;
	strcpy( Seq, "" );
	strcpy( Qual, "" );
	
	strcpy( PG, "" );
	AS = Invalid;
	NM = Invalid;
	NH = Invalid;
	IH = Invalid;
	HI = Invalid;
	strcpy( MD, "" );
	strcpy( CS, "" );
	strcpy( CQ, "" );
	CM = Invalid;
	strcpy( CC, "" );
	CP = Invalid;
	strcpy( XA, "" );
	strcpy( XE, "" );
	Z3 = Invalid;

	strand = 0;
	empty = true;
}
