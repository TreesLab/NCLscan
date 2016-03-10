#include "Common.h"
#include "Table.h"
#include <string.h>

void PrintUsage();

using namespace std; 

int main(int argc, char **argv)
{
	char str[81];
	char title_1[81] = "";
	char title_2[81] = "";
	int flag;
	char line_1[65535] = "";
	char quality[1024] = ""; 
	int i, c;
	int L, N, Q;
	int qlen;

	if(argc != 4)
	{	PrintUsage();	return 1;	}	

	L = atoi(argv[1]);
	N = atoi(argv[2]);
	Q = atoi(argv[3]);

	for( TableRow theRow1; theRow1.Get(); )
	{
		if( theRow1.nColumns() < 11)
		{
			theRow1.Print();
			continue;
		}


		TableRow theRow2;
		theRow2.Get();

		theRow1.ColCopy(title_1, 0);
		if( theRow2.ColCmp(title_1, 0) != 0 )
		{
			ErrMsg("Format error!\n");
			return -1;
		}

		theRow1.ColCopy(quality, 10);
		flag = atoi(theRow1.ColCopy(str, 1)); 
	
		if(strlen(quality) < L)
			continue;

		qlen = strlen(quality);
		
//		printf("1:%s\t%d\n", str, (flag & 0x10) );

		if(flag & 0x10)
		{
			for( i = qlen-1, c = 0; i >= (qlen-L); i--)
			{
				if(quality[i] < Q)
					c++;
			}
			if(c > N)
				continue;
		}
		else
		{
			for( i = 0, c = 0; i < L; i++)
			{
				if(quality[i] < Q)
					c++;
			}
			if(c > N)
				continue;
		}

		theRow2.ColCopy(quality, 10);
		flag = atoi(theRow2.ColCopy(str, 1)); 

		if(strlen(quality) < L)
			continue;

		qlen = strlen(quality);
	
//		printf("2:%s\t%d\n", str, (flag & 0x10) );

		if(flag & 0x10)
		{
			for( i = qlen-1, c = 0; i >= (qlen-L); i--)
			{
				if(quality[i] < Q)
					c++;
			}
			if(c > N)
				continue;
		}
		else
		{
			for( i = 0, c = 0; i < L; i++)
			{
				if(quality[i] < Q)
					c++;
			}
			if(c > N)
				continue;
		}

		theRow1.Print();
		theRow2.Print();
	}
}

void PrintUsage()
{
	ErrMsg("\n");
	ErrMsg("RmBadMapping [L] [N] [Q] \n\n");

	ErrMsg("[L]        INTEGER\n");
	ErrMsg("[N]        INTEGER\n");
	ErrMsg("[Q]        INTEGER\n");
	ErrMsg("For the first L bases, both mapped reads from a pair must have <= N bases below quality Q.\n\n"); 

	ErrMsg("Example:\n");
	ErrMsg("cat Input.sam | ./RmBadMapping 50 1 10 > result.sam\n\n");
}
