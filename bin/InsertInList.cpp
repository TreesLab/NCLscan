#include "Common.h"
#include "Table.h"
#include "Map.h"
#include <string.h>

void PrintUsage();

int main(int argc, char **argv)
{
	Str2StrMap theMap;
	char str[MaxLineSize+1];
	char buf[MaxLineSize+1];
	char *token = NULL;

	if(argc < 5 || argc > 6)
	{	PrintUsage();	return 1;	}	

	theMap.AddIntoMap( argv[2], atoi(argv[3])-1, atoi(argv[4])-1);
		
	for( TableRow theRow; theRow.Get(); )
	{
		ColCpy( str, theRow.Column(atoi(argv[1])-1) );		

		token = strtok( str, ",; " );
		while(token != NULL)
		{
			theMap.At(token, buf);	
			if( buf[0] != '\0' )
			{
				theRow.PrintWithoutNewline();
				printf("\t%s\n", buf);
				
				break;
			}
			else if( argc == 6 )
			{
				theRow.PrintWithoutNewline();
				printf("\t%s\n", argv[5]);
			}
			
			token = strtok(NULL, ",; ");
		}
		token = NULL;
	}

	return 0;
}

void PrintUsage()
{
	ErrMsg("Insert a column into a row with a common key.\n\n");

	ErrMsg("InsertInList [Key1] [List] [Key2] [Column] [Empty]\n\n");

	ErrMsg("[Key1]          INTEGER     The key column in the table.\n");
	ErrMsg("[List]          FILE        The filename of a table as the list.\n");
	ErrMsg("[Key2]          INTEGER     The key column in the list.\n");
	ErrMsg("[Column]        INTEGER     The column to be inserted.\n");
	ErrMsg("[Empty]         INTEGER     The string to be inserted in empty column.\n\n");

	ErrMsg("cat table1 | ./InsertInList 2 table2 1 2 N/A > OutputTable\n\n");
}
