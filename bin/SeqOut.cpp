#include <string.h>
#include "Common.h"
#include "Table.h"
#include "Seq.h"
#include "Map.h"

void PrintUsage();

int main(int argc, char **argv)
{
	Str2BoolMap theMap;

	if(argc != 3)
	{	PrintUsage();	return 1;	}	

	theMap.AddIntoMap( argv[1], atoi(argv[2])-1 );

	for( Sequence seq; seq.Get(); )
	{
		if( theMap.IsInMap(seq.Name()) )
			seq.Print();
	}

	return 0;
}

void PrintUsage()
{
	ErrMsg("Output the listed sequences.\n\n");

	ErrMsg("SeqOut [List] [Key]\n\n");

	ErrMsg("[List]          FILE        The filename of a table as the list.\n");
	ErrMsg("[Key]           INTEGER     The key column in the list.\n");

	ErrMsg("Example:\n");
	ErrMsg("cat est.fa | ./SeqOut list.txt 1 > result.fa\n\n");
}
