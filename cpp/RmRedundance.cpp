#include "Common.h"
#include "Table.h"
#include "Map.h"

void PrintUsage();

int main(int argc, char **argv)
{
	Str2BoolMap theMap;
	char str[MaxLineSize+1];

	if(argc != 2)
	{	PrintUsage();	return 1;	}	

	for( TableRow theRow; theRow.Get(); )
	{
		ColCpy( str, theRow.Column(atoi(argv[1])-1) );		

		if( theMap.IsInMap(str) == false )
		{
			theRow.Print();
			theMap.Add(str);
		}
	}

	return 0;
}

void PrintUsage()
{
	ErrMsg("Remove redundant entries.\n\n");

	ErrMsg("RmRedundance [Key]\n\n");

	ErrMsg("[Key]          INTEGER     The key column in the table.\n\n");

	ErrMsg("cat Input | RmRedundance 1 > Output\n\n");
}
