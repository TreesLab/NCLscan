#include <string.h>
#include "Common.h"
#include "Table.h"
#include "Seq.h"

void PrintUsage();

double Similarity(const char *seq1, const char *seq2, const int len)
{
	int i, c;
	
	c=0;
	for( i=0; i < len; i++ )
	{
		if(seq1[i] != seq2[i])
			c++;
	}

	return 1.0 - (double(c)/double(len));
}

void PrintBetter(char *str, const char *seq1, const char *qs1, const char *seq2, const char *qs2, const int len)
{
	int i, c, s;
	
	c=0;
	s=0;
	for( i=0; i < len; i++ )
	{
		if(seq1[i] != seq2[i])
		{
			c++;
			if(qs1[i] > qs2[i])
				s--;
			else
				s++;
		}
	}

	if(s < 0)
		sprintf(str, "%s%s", seq1, seq2+len);
	else
		sprintf(str, "%s", seq2);
}

bool IsNuc(const char c)
{
	if( c == 'a' || c == 'A')
		return true;
	if( c == 'g' || c == 'G')
		return true;
	if( c == 'c' || c == 'C')
		return true;
	if( c == 't' || c == 'T')
		return true;	

	return false;
}

int Assemble(char *str, const char *seq1, const char *q1, const char *seq2, const char *q2)
{
	int len1;
	int len2;
	
	int i;
	char *pstr;

	char s1[32768];
	char s2[32768];
	
	char ss2[32768] = "";
	char qq2[32768];

	pstr = str;

	strcpy(s1, seq1);
	strcpy(s2, seq2);
	
	len1 = strlen(s1);
	len2 = strlen(s2);
	
	if( s1[len1-1] == '\n'){
		s1[len1-1] = '\0';
		len1--;
	}
	if( s2[len2-1] == '\n'){
		s2[len2-1] = '\0';
		len2--;
	}
	
	for( i=len1-1; i >= 10; i--)
	{
		if( !IsNuc(s1[i]) )
			s1[i] = '\0';
		if( q1[i] <= '#' )
			s1[i] = '\0';
	} 
	
	for( i=len2-11; i>=0; i--)
	{
		if( !IsNuc(s2[i]) ){
			strcpy(ss2, s2+i+1);
			strcpy(qq2, q2+i+1);
			break;
		}
		if( q2[i] <= '#' ){
			strcpy(ss2, s2+i+1);
			strcpy(qq2, q2+i+1);
			break;
		}
	}
	
	len1 = strlen(s1);
	len2 = strlen(ss2);
	
	if( len2 == 0){
		strcpy(ss2, s2);
		strcpy(qq2, q2);
		len2 = strlen(ss2);
	}
	
	if(len1 <= len2){
		for(i=0; i < len1; i++)
		{
			if( Similarity(s1+i, ss2, len1-i) >= 0.95 )
			{
				PrintBetter(pstr, s1+i, q1+i, ss2, qq2, len1-i);
				break;
			}

			sprintf(pstr, "%c", s1[i]);
			pstr++;
		}
		
		if(i == len1)
			sprintf(pstr, "%s", ss2);
			
	} else {
		for(i=0; i < len1; i++)
		{
			if (i >= len1-len2)
			{
				if( Similarity(s1+i, ss2, len1-i) >= 0.95 )
				{
					PrintBetter(pstr, s1+i, q1+i, ss2, qq2, len1-i);
					break;
				}
			}

			sprintf(pstr, "%c", s1[i]);
			pstr++;
		}
		
		if(i == len1)
			sprintf(pstr, "%s", ss2);
		
	}
	
	return len2+i;
}


int main(int argc, char **argv)
{
	FILE *fp1, *fp2;
	char str1[32760];
	char str2[32760];
	char str22[32760];
	char str3[32760];
	char s1[32760];
	char s2[32760];
	char q1[32760];
	char q2[32760];

	int len;

	if(argc != 3)	
	{	PrintUsage();	return 1;	}	

	if(	( fp1 = fopen(argv[1], "r") ) == NULL )
	{	PrintUsage();	return 1;	}	

	if(	( fp2 = fopen(argv[2], "r") ) == NULL )
	{	PrintUsage();	return 1;	}	

	for( Fastq fastq1; fastq1.Get(fp1); )
	{
		Fastq fastq2;
		fastq2.Get(fp2);

		if( strcmp(fastq1.Name(), fastq2.Name()) != 0 )
			ErrMsg("Sequence names do not match.\n");
	
		strcpy(s1, fastq1.Seq());
		strcpy(str1, fastq2.Seq());	
		strcpy(q1, fastq1.QS());
		strcpy(q2, fastq2.QS());		

		if( strlen(s1) > 100 ){
			s1[100] = '\0';
			q1[100] = '\0';
		}
		if( strlen(str1) > 100 ){
			str1[100] = '\0';
			q2[100] = '\0';
		}

		strcpy(s2, ToComplement(str2, str1));

		len = strlen(fastq1.Seq()) - 1;
		if( len < (strlen(fastq2.Seq()) - 1 ) )
			len = strlen(fastq2.Seq()) - 1;

		if( Assemble(str1, s1, q1, s2, ReverseStr(str2, q2)) <= len )
		{
			printf(">%s\n%s\n", fastq2.Name(), ToComplement(str2, str1));
		}	
		else
		{
			Assemble(str1, fastq1.Seq(), fastq1.QS(), ToComplement(str2, fastq2.Seq()), ReverseStr(str3, fastq2.QS()));
			printf(">%s\n%s\n", fastq2.Name(), ToComplement(str2, str1));
		}
 	}

	fclose(fp1);
	fclose(fp2);

	return 0;
}

void PrintUsage()
{
	ErrMsg("Assemble the Sequence from paired end fastq files.\n\n");

	ErrMsg("Example:\n");
	ErrMsg("./AssembleFastq R1.fastq R2.fastq > result.fa\n");
}
