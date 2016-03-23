#include "Common.h"
#include "Table.h"

void PrintUsage();

int main(int argc, char **argv)
{
	int i, len;	

	char str[MaxLineSize+1];

	if(argc != 2)
	{	PrintUsage();	return 1;	}	

	for( TableRow theRow; theRow.Get(); )
	{
		ColCpy( str, theRow.Column(atoi(argv[1])-1) );		

		len = strlen(str);

		for( i=len-1; i>=0; i--)
		{
			if(str[i] == '.')
			{
				str[i] = '\0';
				break;
			}
		}

		printf("%s\n", str);
	}

	return 0;
}

void PrintUsage()
{
	ErrMsg("Example: \n");
	ErrMsg("cat Input.sam | GetNameB4Dot 4 > Output\n\n");
}
