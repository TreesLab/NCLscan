#include <string.h>
#include "Common.h"
#include "Table.h"
#include "Seq.h"
#include "Map.h"

void PrintUsage();

int main(int argc, char **argv)
{
	Str2BoolMap theMap;
	FILE *infile, *outfile;

	if(argc == 5)	
	{
		if(	( infile = fopen(argv[1], "r") ) == NULL )
		{	PrintUsage();	return 1;	}	
		if(	( outfile = fopen(argv[4], "w") ) == NULL )
		{	PrintUsage();	return 1;	}	

		theMap.AddIntoMap( argv[2], atoi(argv[3])-1 );
	}	
	else if(argc == 3)
	{
		infile = stdin;
		outfile = stdout;
		theMap.AddIntoMap( argv[1], atoi(argv[2])-1 );
	}
	else
	{	PrintUsage();	return 1;	}	
	for( Fastq fastq; fastq.Get(infile); )
	{
		if( theMap.IsInMap(fastq.Name()) )
			fastq.Print(outfile);
	}

	fclose(infile);
	fclose(outfile);
	return 0;
}

void PrintUsage()
{
	ErrMsg("Output the listed sequences.\n\n");

	ErrMsg("FastqOut [List] [Key]\n");
	ErrMsg("FastqOut [Input] [List] [Key] [Output]\n\n");

	ErrMsg("[Input]         STRING      A FASTQ format file.\n");
	ErrMsg("[List]          FILE        The filename of a table as the list.\n");
	ErrMsg("[Key]           INTEGER     The key column in the list.\n");
	ErrMsg("[Output]        STRING      \n\n");

	ErrMsg("Example:\n");
	ErrMsg("cat SOLiD.fastq | ./FastqOut list.txt 1 > result.fastq\n");
	ErrMsg("FastqOut SOLiD.fastq list.txt 1 result.fastq\n\n");
}
