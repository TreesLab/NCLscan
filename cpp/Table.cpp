#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include "Common.h"
#include "Table.h"

const int SplitSize = 1024*1024*1024;
const char TempName[] = "TempFile.sort"; 
const char TempSubName[] = ".sortmp";

using namespace std;

char *ColCpy(char *str, char *pCol)
{
	int i;

	if( pCol != NULL )
	{
		for(i=0; !IsColEnd(pCol[i]); i++)
			str[i] = pCol[i];

		str[i] = '\0';
	}
	else
		strcpy(str, "");

	return str;
}

int NumberOfColumns(const char *str)
{
	int i;

	i = 1;

	while(str[i] != '\n' && str[i] != '\0' )
	{
		if( str[i] == '\t' )
			i++;
	}

	return i;
}

bool IsNumber(char *pCell)
{	
	while( !IsColEnd(pCell) )
	{
		if( *pCell >= 48 && *pCell <= 57 )
			return true;
		else if( *pCell >= 58 && *pCell <= 127 )
			return false;
		pCell++;
	}
	
	return false;
}

int LineSize(char *str)
{	
	int i;	
	for(i=0; i < MaxLineSize; i++)	
	{	
		if( IsLineEnd( str[i] ) )	
			return i;	
	}
	return 0;
}

// class TableRow
bool TableRow::Get(char *str)
{
	int i, count;

	if(	row != NULL )
	{
		Clear();
		Init();
	}

	count = 1;
	for( i=0; !IsLineEnd( str[i] ); i++ )
	{
		if( str[i] == '\t' )
			count++;
	}
	nCol = count;

	row = new char [i+1];
	pColumn = new char* [nCol];

	pColumn[0] = row;
	count = 1;
	for( i=0; !IsLineEnd( str[i] ); i++ )
	{
		if( str[i] == '\t' )
		{
			pColumn[count] = row + i + 1;
			count++;
		}

		row[i] = str[i];
	}

	row[i] = '\0';

	return true;
}

bool TableRow::Get(FILE *fp)
{
	char str[MaxLineSize+1];

	if( fgets(str, MaxLineSize, fp) != NULL )
		return Get(str);

	return false;
}

bool TableRow::PrintColumn(int i)
{
	if( i < nCol )
	{	
		char *pstr;
		for( pstr = pColumn[i]; !IsColEnd( pstr ); pstr++ )
			putchar(*pstr);
	}
	else
		return false;

	return true;
}

void TableRow::PrintSelectedColumns(const char *mask)
{
	int i;
	int lenmask;
	char *maskstr;
	char *pstr;

	if(nCol == 0)
		return;

	lenmask = strlen(mask);

	maskstr = new char [lenmask+1];
	strcpy(maskstr, mask);

	pstr = maskstr;
	while( !IsColEnd(pstr) )
		pstr++;

	while( pstr != mask && *pstr != '1' )
	{
		if( *pstr == '0' )
			*pstr = '\0';			
	
		pstr--;
	}
	while( pstr != maskstr && *(pstr-1) > 32 )
		pstr--;

	lenmask = LineSize(pstr);

	for( i=0; !IsLineEnd(pstr[i]); i++ )
	{
		if( pstr[i] == '1' )
		{
			PrintColumn(i);

			if( i == (lenmask - 1) )
				putchar('\n');			
			else
				putchar('\t');
		}
	}

	delete [] maskstr;
}

bool TableRow::CheckFormat(const char *sC, const char *comment)
{
	int i;
	char *pstr;

	if(nCol == 0)
		return false;

	pstr = Column(0);
	while( ( *pstr <= 32 ) && ( pstr != '\0' || pstr < Column(nCol-1) ) )
		pstr++;

	for( i=0; true; i++)
	{
		if( IsLineEnd( comment[i] ) && i > 0 )
			return false;
		else if( comment[i] != pstr[i] )
			break;
	}

	for( i=0; !IsLineEnd( sC[i] ) ; i++)
	{	
		switch( sC[i] )
		{
			case 'E':
			case 'e':
				if( nCol > i )
					return false;
				break;
			case 'A':
			case 'a':
				if( nCol < (i+1) )
					return false;
				break;
			case 'N':
			case 'n':
				if( nCol < (i+1) )
					return false;
				if( !IsNumber( Column(i) ) )
					return false;
				break;
			default:
				return false;
				break;
		}
	}
	return true;
}

int TableRow::ColCmp(const char *str, const int i)
{
	char colstr[MaxLineSize+1];
	
	ColCpy(colstr, Column(i));

	return strcmp(colstr, str);
}

void TableRow::Init()
{
	row = NULL;
	pColumn = NULL;
	nCol = 0;
}

void TableRow::Clear()
{
	if( row != NULL )
		delete [] row;

	if( pColumn != NULL )
		delete [] pColumn;
}

// Begin Class TableSort
struct KeyValue
{
	char *keyvalue;
	char *thisline;
};

class TableSort
{
	int column;
	char keytype;

	char *table;
	vector<KeyValue> keys;
	char voidstr[1];
	char tempfilename[32];	

	bool KeyCmp(KeyValue a, KeyValue b);
	bool SetKeytype(const char type);
	void SetKeyValue(char *pstr, KeyValue *pkey);
	void FileMerge( FILE *fp1, FILE *fp2 );
	void SplitFile( FILE *fp );
	char *GetFileName( int i );
	void LoadTable( FILE *fp, int col );
	void Sort();
	void Save( FILE *fp );

public:
	void Sort( const char *infilename, const int col, const char type, const char *outfilename);
	TableSort();
	~TableSort();
};

bool StrCmp(KeyValue a, KeyValue b)
{
	int i = -1;
	
	do
	{
		i++;		

		if(a.keyvalue[i] > b.keyvalue[i])
			return false;
		else if(a.keyvalue[i] < b.keyvalue[i])
			return true;			
		
	} while(a.keyvalue[i] >= 32 && b.keyvalue[i] >= 32);
	
	return false;
}

bool NumCmp(KeyValue a, KeyValue b)
{
	double A, B;

	A = atof(a.keyvalue);
	B = atof(b.keyvalue);

	return ( A < B );
}

void Fprint(FILE *fp, char *str)
{
	if( str != NULL && str[0] != '\0' && str[0] != '\n' )
		fprintf(fp, "%s\n", str);
}

bool TableSort::KeyCmp(KeyValue a, KeyValue b)
{
	switch(keytype)
	{
		case 'S':
		case 's':
			return StrCmp(a, b);
			break;
		case 'N':
		case 'n':
			return NumCmp(a, b);
			break;
	}
	
	return false;
}

void TableSort::SetKeyValue(char *pstr, KeyValue *pkey)
{
	int i;
	int count;

	(*pkey).thisline = pstr;	
	(*pkey).keyvalue = voidstr;

	if(pstr == NULL)
		return;

	count = 1;		
	for( i=0; pstr[i] != '\0' && pstr[i] != '\n'; i++ )
	{
		if( count == column )
		{
			(*pkey).keyvalue = pstr + i;
			break;
		}

		if(pstr[i] == '\t')
			count++;
	}
}

void TableSort::LoadTable(FILE *fp, int col)
{
	long fsize;
	char* pstr;
	KeyValue thiskey;

	if(table != NULL)
		delete [] table;
		
	column = col;
	// Get file size
	fseek(fp, 0L, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	table = new	char [fsize+1];
	pstr = table;	
	
	while( fgets(pstr, MaxLineSize, fp) != NULL )
	{
		SetKeyValue(pstr, &thiskey);

		pstr += LineSize(pstr);
		pstr[0] = '\0';
		pstr++;

		keys.push_back(thiskey);		
	}
}

void TableSort::Save(FILE *fp)
{
	unsigned int i;
	
	for( i=0; i < keys.size(); i++ )
	{
		if(keys.at(i).thisline[0] != '\0' && keys.at(i).thisline[0] != '\n')
			fprintf(fp, "%s\n", keys.at(i).thisline);
	}
}

void TableSort::Sort()
{
	switch(keytype)
	{
		case	'S':
		case	's':
			stable_sort(keys.begin(), keys.end() ,StrCmp);
			break;
		case	'N':
		case	'n':
			stable_sort(keys.begin(), keys.end() ,NumCmp);
			break;
	}
}

TableSort::TableSort()
{
	voidstr[0] = '\0';
	table = NULL;
}

TableSort::~TableSort()
{
	if( table != NULL )
		delete [] table;
}

char *TableSort::GetFileName( int i )
{
	return strcat( ItoA(i, tempfilename, 10), TempSubName );
}

void TableSort::FileMerge( FILE *fp1, FILE *fp2 )
{
	FILE *outfile;
	char str1[MaxLineSize+1], str2[MaxLineSize+1];
	KeyValue key1, key2;

	outfile = fopen(TempName, "w");

	if(fp2 == NULL)
	{
		while( fgets(str1, MaxLineSize, fp1) != NULL )
				fputs(str1, outfile);	

		fclose(outfile);
		return;		
	}

	SetKeyValue(NULL, &key1);			
	SetKeyValue(NULL, &key2);				

	while( fgets(str1, MaxLineSize, fp1) != NULL )
	{
		SetKeyValue(str1, &key1);			
		key1.thisline[LineSize(key1.thisline)] = '\0';

		while( KeyCmp(key2, key1) )
		{
			Fprint(outfile, key2.thisline);
			if( fgets(str2, MaxLineSize, fp2) != NULL )
			{
				SetKeyValue(str2, &key2);			
				key2.thisline[LineSize(key2.thisline)] = '\0';			
			}
			else
			{
				SetKeyValue(NULL, &key2);				
				break;	
			}
		}
		Fprint(outfile, key1.thisline);		
	}
	Fprint(outfile, key2.thisline);

	while( fgets(str2, MaxLineSize, fp2) != NULL )
		fputs(str2, outfile);	

	fclose(outfile);
}

void TableSort::SplitFile( FILE *fp )
{
	int fileindex;
	char str[MaxLineSize+1];
	FILE *outfile;
	int accsize;

	fileindex = 0;
	outfile = fopen(GetFileName(fileindex), "w");

	accsize = 0;
	while( fgets(str, MaxLineSize, fp) != NULL )
	{
		fputs(str, outfile);
		accsize += strlen(str);

		if( accsize > SplitSize )
		{
			accsize = 0;

			fclose(outfile);
			fileindex++;
			outfile = fopen(GetFileName(fileindex), "w");
		}
	}

	fclose(outfile);
}

bool TableSort::SetKeytype(const char type)
{
	switch(type)
	{
		case 'S':
		case 's':
			keytype = type;
			return true;
		case 'N':
		case 'n':
			keytype = type;
			return true;
	}
	return false;
}

void TableSort::Sort( const char *infilename, const int col, const char type, const char *outfilename)
{
	FILE *fp;
	FILE *fp1 = NULL, *fp2 = NULL;
	int fileindex;
	
	column = col;

	if(	SetKeytype(type) == false )	{	ErrMsg("Error: wrong type.\n");	return;	}

	if(	( fp = fopen(infilename, "r") ) == NULL ){	ErrMsg("Error: fopen failed.\n");	return;	}

	SplitFile(fp);
	fclose(fp);	

	for( fileindex = 0; (fp1 = fopen( GetFileName(fileindex) , "r")) != NULL; fileindex++ )
	{	
		FILE *outfile;
		outfile = fopen(TempName, "w");

		TableSort thetable;
	
		thetable.LoadTable(fp1, column);
		thetable.SetKeytype(keytype);
		thetable.Sort();
		thetable.Save(outfile);
		
		remove( GetFileName(fileindex) );
		rename( TempName, GetFileName(fileindex) );		
	
		fclose(outfile);
	} 

	while( (fp1 = fopen( GetFileName(1) , "r")) != NULL )
	{
		fclose(fp1);
		for( fileindex = 0; ( fp1 = fopen( GetFileName(fileindex*2) , "r") ) != NULL; fileindex++ )
		{
			fp2 = fopen( GetFileName(fileindex*2+1), "r" );	

			FileMerge(fp1, fp2);

			if(fp1 != NULL)
				fclose(fp1);
			if(fp2 != NULL)
				fclose(fp2);
			remove( GetFileName(fileindex*2) );
			remove( GetFileName(fileindex*2+1) );
			rename( TempName, GetFileName(fileindex) );
		}
	}

	if(fp1 != NULL)
		fclose(fp1);

	rename( GetFileName(0), outfilename );
}
// End Class TableSort

void SortTable(const char *infilename, const int col, const char type, const char *outfilename)
{
	TableSort thetable;
	thetable.Sort(infilename, col, type, outfilename);
}

bool IsSorted(char *infilename, int col, char type)
{
	FILE *fp;
	char str[MaxLineSize+1];
	char key1[MaxLineSize+1] = "", key2[MaxLineSize+1] = "";

	if(	( fp = fopen(infilename, "r") ) == NULL )	{	ErrMsg("Error: fopen failed.\n");	return false;	}

	while( fgets(str, MaxLineSize, fp) != NULL )
	{
		TableRow theRow(str);

		if( !theRow.IsColValid(col) )
		{	
			fclose(fp);
			return false;
		}
		
		ColCpy( key2, theRow.Column(col) );

		switch(type)
		{
			case 'S':
			case 's':
				if( strcmp(key1, key2) > 0 )
				{
					fclose(fp);
					return false;	
				}
				break;
			case 'N':
			case 'n':
				if( atoi(key2) > atoi(key1) )
				{
					fclose(fp);
					return false;
				}
				break;
			default:
				fclose(fp);
				return false;
				break;
		}			

		strcpy(key1, key2);	
	}
	
	fclose(fp);
	return true;
}
