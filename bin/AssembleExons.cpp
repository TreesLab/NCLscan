#include <string>
#include "Common.h"
#include "Table.h"

void PrintUsage();

using namespace std;

int main(int argc, char **argv)
{
	char preTitle[81] = "";
	char thisTitle[81] = "";
	char str[65530];
	string Seq;

	if(argc != 1)
	{	PrintUsage();	return 1;	}	

	for( TableRow theRow; theRow.Get(); )
	{
		if( theRow.nColumns() != 2)
			continue;

		theRow.ColCopy(thisTitle, 0);
		theRow.ColCopy(str, 1);

		if( strcmp(thisTitle, preTitle) == 0 )
			Seq += str;
		else 
		{
			printf("%s\t%s\n", preTitle, Seq.c_str());
			Seq = str;
			strcpy(preTitle, thisTitle);
		}			
	}

	printf("%s\t%s\n", thisTitle, Seq.c_str());

	return 0;
}

void PrintUsage()
{
	ErrMsg("Assemble the exons.\n\n");

	ErrMsg("Example:\n");
	ErrMsg("cat exons.seq | ./AssembleJSeq > Output.seq\n\n");
}
