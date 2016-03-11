#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "Common.h"
#include "Seq.h"
using namespace std;

char *AlphaCpy(char *des_str, const char *sou_str)
{
	int i;
	char *pstr;
	
	pstr = des_str;

	for(i=0; sou_str[i] != '\0'; i++)
	{
		if( isalpha(sou_str[i]) )
		{
			*pstr = sou_str[i];
			pstr++;
		}
	}
	*pstr = '\0';	

	return des_str;
}

char *ToColorSpace(char *cs_seq, const char *ns_seq)
{
	int i;
	
	cs_seq[0] = '\0';
	if( ns_seq[0] < 65 || ns_seq[0] > 122 )
		return cs_seq;

	for(i=0; (ns_seq[i+1] >= 65 && ns_seq[i+1] <= 122); i++)
	{
		switch(ns_seq[i])
		{
			case 'A':
			case 'a':
				switch(ns_seq[i+1])
				{
					case 'A':
					case 'a':
						cs_seq[i] = '0';
						break;	
					case 'C':
					case 'c':
						cs_seq[i] = '1';
						break;
					case 'G':
					case 'g':
						cs_seq[i] = '2';
						break;
					default:
						cs_seq[i] = '3';
				}
				break;

			case 'C':
			case 'c':
				switch(ns_seq[i+1])
				{
					case 'A':
					case 'a':
						cs_seq[i] = '1';
						break;	
					case 'C':
					case 'c':
						cs_seq[i] = '0';
						break;
					case 'G':
					case 'g':
						cs_seq[i] = '3';
						break;
					default:
						cs_seq[i] = '2';
				}				
				break;

			case 'G':
			case 'g':
				switch(ns_seq[i+1])
				{
					case 'A':
					case 'a':
						cs_seq[i] = '2';
						break;	
					case 'C':
					case 'c':
						cs_seq[i] = '3';
						break;
					case 'G':
					case 'g':
						cs_seq[i] = '0';
						break;
					default:
						cs_seq[i] = '1';
				}				
				break;
			
			default:
				switch(ns_seq[i+1])
				{
					case 'A':
					case 'a':
						cs_seq[i] = '3';
						break;	
					case 'C':
					case 'c':
						cs_seq[i] = '2';
						break;
					case 'G':
					case 'g':
						cs_seq[i] = '1';
						break;
					default:
						cs_seq[i] = '0';
				}
		}
	}
	cs_seq[i] = '\0';	

	return cs_seq;
}

char *ReverseStr(char *rseq, const char *seq)
{
	int i;
	int len;

	len = strlen(seq);
	
	rseq[len] = '\0';

	if( seq[len-1] == '\n' )
	{
		rseq[len-1] = '\n';		
		len--;
	}

	for(i=0; i<len; i++)
		rseq[len-i-1] = seq[i];
	
	return rseq;
}


char *ToComplement(char *rseq, const char *seq)
{
	int i;
	int len;

	len = strlen(seq);
	
	rseq[len] = '\0';

	if( seq[len-1] == '\n' )
	{
		rseq[len-1] = '\n';		
		len--;
	}



	for(i=0; i<len; i++)
	{
		switch(seq[i])
		{
			case 'A':
				rseq[len-i-1] = 'T';
				break;
			case 'a':
				rseq[len-i-1] = 't';
				break;

			case 'G':
				rseq[len-i-1] = 'C';
				break;
			case 'g':
				rseq[len-i-1] = 'c';
				break;

			case 'C':
				rseq[len-i-1] = 'G';
				break;
			case 'c':
				rseq[len-i-1] = 'g';
				break;

			case 'T':
				rseq[len-i-1] = 'A';
				break;
			case 't':
				rseq[len-i-1] = 'a';
				break;

			case 'R':
				rseq[len-i-1] = 'Y';
				break;
			case 'r':
				rseq[len-i-1] = 'y';
				break;

			case 'Y':
				rseq[len-i-1] = 'R';
				break;
			case 'y':
				rseq[len-i-1] = 'r';
				break;

			case 'K':
				rseq[len-i-1] = 'M';
				break;
			case 'k':
				rseq[len-i-1] = 'm';
				break;

			case 'M':
				rseq[len-i-1] = 'K';
				break;
			case 'm':
				rseq[len-i-1] = 'k';
				break;

			case 'S':
				rseq[len-i-1] = 'W';
				break;
			case 's':
				rseq[len-i-1] = 'w';
				break;

			case 'W':
				rseq[len-i-1] = 'S';
				break;
			case 'w':
				rseq[len-i-1] = 's';
				break;

			case 'B':
				rseq[len-i-1] = 'V';
				break;
			case 'b':
				rseq[len-i-1] = 'v';
				break;

			case 'V':
				rseq[len-i-1] = 'B';
				break;
			case 'v':
				rseq[len-i-1] = 'b';
				break;

			case 'D':
				rseq[len-i-1] = 'H';
				break;
			case 'd':
				rseq[len-i-1] = 'h';
				break;

			case 'H':
				rseq[len-i-1] = 'D';
				break;
			case 'h':
				rseq[len-i-1] = 'd';
				break;

			default:
				rseq[len-i-1] = seq[i];
				break;
		}
	}
	
	return rseq;
}

int NameLen(const char *title)
{
	int i;

	for(i=1; isgraph(title[i]); i++);
	
	return i-1;
}

char *CpyName(char *str, const char *title)
{
	int i;
	char *pstr;

	pstr = str;

	for(i=1; isgraph(title[i]); i++, pstr++)
		*pstr = title[i];
	
	*pstr = '\0';

	return str;
}

bool Sequence::Get(FILE *fp)
{
	char str[MaxLineSize+1];

	if( title != NULL )
	{
		Clear();
		Init();
	}

	while( fgets(str, MaxLineSize, fp) != NULL )
	{
		char c;

		if( str[0] == '>' )
		{
			title = new char [strlen(str)+1];						
			strcpy(title, str);

			name = new char [NameLen(title)+1];				
			CpyName(name, title);

			assign("");
		}
		else if( title != NULL )
		{
			AlphaCpy(str, str);
			append(str);
		}

		c = fgetc(fp);
		ungetc(c, fp);
		if( c == '>' || c == EOF )
			break;
	}

	if( title != NULL )
		return true;

	return false;
}

bool Sequence::Get(const char *tit, const char *seq)
{
	char str[MaxLineSize+1];
	int titlen;

	if( title != NULL )
	{
		Clear();
		Init();
	}

	titlen = strlen(tit);

	if( tit[0] == '>' )	
	{				
		title = new char [strlen(tit)+2];	
		strcpy(title, tit);
		title[titlen] = '\n';
		title[titlen+1] = '\0';
	}
	else
	{
		title = new char [strlen(tit)+3];	
		title[0] = '>';
		strcpy(title+1, tit);
		title[titlen+1] = '\n';
		title[titlen+2] = '\0';
	}

	name = new char [NameLen(title)+1];				
	CpyName(name, title);
	assign("");

	AlphaCpy(str, seq);
	append(str);

	if( title != NULL )
		return true;

	return false;
}

void PrintSeq(const char *str, int width, FILE *fp)
{
	int i;

	for(i=0; str[i] != '\0'; i++)
	{
		fputc(str[i], fp);
		if( ((i+1)%width) == 0 )
			fputc('\n', fp);
	}
	if( i%width )
		fputc('\n', fp);
}

void Sequence::Print(int width, FILE *fp)
{
	fputs(title, fp);
	PrintSequence(width, fp);
}

void Sequence::PrintSequence(int width, FILE *fp)
{
	PrintSeq(c_str(), width, fp);
}

void Sequence::Init()
{
	title = NULL;	
	name = NULL;
}

void Sequence::Clear()
{
	if(	title != NULL )
		delete [] title;
	if(	name != NULL )
		delete [] name;
}

bool Fastq::Get(FILE *fp)
{
	char str[MaxLineSize+1];
	int len;

	if( name != NULL )
	{
		Clear();
		Init();
	}

	if( fgets(str, MaxLineSize, fp) != NULL )
	{
		if( str[0] == '@' )
		{
			name = new char [strlen(str)+1];

			CpyName(name, str);

			len = strlen(name);
			if(name[len-2] == '/')
			{
				if(name[len-1] == '1' || name[len-1] == '2')
					name[len-2] = '\0';
			}

			if( fgets(str, MaxLineSize, fp) == NULL )	{	Clear(); Init(); return false;	}
			seqline = new char [strlen(str)+1];
			strcpy(seqline, str);

			if( fgets(str, MaxLineSize, fp) == NULL )	{	Clear(); Init(); return false;	}
			if( str[0] != '+' )	{	Clear(); Init(); return false;	}
			plusline = new char [strlen(str)+1];
			strcpy(plusline, str);

			if( fgets(str, MaxLineSize, fp) == NULL )	{	Clear(); Init(); return false;	}
			qsline = new char [strlen(str)+1];
			strcpy(qsline, str);
		}
	}

	if( name != NULL )
		return true;

	return false;
}

void Fastq::Print(FILE *fp)
{
	fprintf( fp, "@%s\n", name );
	fputs( seqline, fp );
	fputs( plusline, fp );
	fputs( qsline, fp );
}

void Fastq::PrintReverseStrand(FILE *fp)
{
	char str[MaxLineSize+1];

	fprintf( fp, "@%s\n", name);	
	fputs( ToComplement(str, seqline), fp );
	fputs( plusline, fp );
	fputs( StrRev(str, qsline), fp );
}

void Fastq::Init()
{
	name = NULL;
	seqline = NULL;
	plusline = NULL;
	qsline = NULL;
}

void Fastq::Clear()
{
	if( name != NULL )
		delete [] name;
	if( seqline != NULL )
		delete [] seqline;
	if( plusline != NULL )
		delete [] plusline;
	if( qsline != NULL )
		delete [] qsline;
}

bool Csfasta::Get(FILE *fp)
{
	char str[MaxLineSize+1];

	if( name != NULL )
	{
		Clear();
		Init();
	}

	if( fgets(str, MaxLineSize, fp) != NULL )
	{
		if( str[0] == '>' )
		{
			name = new char [strlen(str)+1];
			CpyName(name, str);

			if( fgets(str, MaxLineSize, fp) == NULL )	{	Clear(); Init(); return false;	}
			seqline = new char [strlen(str)+1];
			strcpy(seqline, str);
		}
	}

	if( name != NULL )
		return true;

	return false;
}

void Csfasta::Print(FILE *fp)
{
	fprintf( fp, ">%s\n", name );
	fputs( seqline, fp );
}

void Csfasta::Init()
{
	name = NULL;
	seqline = NULL;
}

void Csfasta::Clear()
{
	if( name != NULL )
		delete [] name;
	if( seqline != NULL )
		delete [] seqline;
}

void SeqPool::Add(FILE *fpFasta)
{
	Clear();
	Init();

	Sequence seq;
	for( numEntries = 0; seq.Get(fpFasta); numEntries++);

	fseek(fpFasta, 0L, SEEK_SET);

	SeqArr = new Sequence [numEntries+1];

	for( numEntries = 0; SeqArr[numEntries].Get(fpFasta); numEntries++)
	{
		SeqMap[SeqArr[numEntries].Name()] = numEntries;
	}
}

void SeqPool::Add(const char *infilename)
{
	FILE *fp = NULL;

	if(	( fp = fopen(infilename, "r") ) == NULL ){	ErrMsg("Error: fopen failed.\n");	return;	}	

	Add(fp);	

	if( fp != NULL )
		fclose(fp);
}

void SeqPool::Init()
{
	numEntries = 0;
	SeqArr = NULL;
}

void SeqPool::Clear()
{
	if( SeqArr != NULL )
		delete [] SeqArr;	
}

SeqPool::SeqPool()
{
	Init();
}

SeqPool::~SeqPool()
{
	Clear();
}

Sequence *SeqPool::GiveMe(int i)
{
	if(i < numEntries)
		return &SeqArr[i];

	return NULL;
}

Sequence *SeqPool::GiveMe(const char *Name)
{
	int i;

	if( SeqMap.find(Name) != SeqMap.end() )
	{
		i = SeqMap.find(Name)->second;
		return &SeqArr[i];
	}

	return NULL;
}


