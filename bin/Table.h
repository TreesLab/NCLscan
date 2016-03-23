#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

/*-------------------------------------------------------------------------------------------------------
	Table
-------------------------------------------------------------------------------------------------------*/

// Copy the string in a column.
char *ColCpy(char *str, char *pCol);

// Test if encounter the end of a column.
inline bool IsColEnd(char *str)
{	if(*str == '\0' || *str == '\n' || *str == '\t')	return true;	return false;	}
inline bool IsColEnd(char c)
{	if(c == '\0' || c == '\n' || c == '\t')	return true;	return false;	}

// Test if encounter the end of a line.
inline bool IsLineEnd(char *str)
{	if(*str == '\0' || *str == '\n') 	return true;	return false;	}
inline bool IsLineEnd(char c)
{	if(c == '\0' || c == '\n')	return true;	return false;	}

// Test if a STRING can be truned into a number.
bool IsNumber(char *pCell);

// Test if a table is sorted. Ex. IsSorted.cpp
bool IsSorted(char *infilename, int col, char type);

// Return the size of a STRING;
int LineSize(char *str);

// Print a column.
inline void PrintColumn(char *pCol)
{	while( !IsColEnd( pCol ) )	{	putchar(*pCol);	pCol++;	}	}

// Sort a table stably by the content of ith column. Ex. SortTable.cpp
void SortTable(const char *infilename, const int col, const char type, const char *outfilename);

// A ROW of a table. Ex. FormatTable.cpp, SelectColumn.cpp
class TableRow
{
	char *row;
	char **pColumn;
	int nCol;

	void Init();
	void Clear();
public:

	bool Get(FILE *fp = stdin);
	bool Get(char *str);

	// The number of colomns.
	inline int nColumns()	{ return nCol; }

	// Test if ith column is valid.
	inline bool IsColValid(int i)
	{	if( i >= 0 && i<nCol ) return true;	return false;	}

	// Return ith column.
	inline char *Column(int i)	{ if( i < nCol )	return pColumn[i];	return NULL;	}
	inline int ColToI(int i) { if( i < nCol )	return atoi(pColumn[i]);	return INT_MIN;	}

	// Copy ith column to a string.
	char *ColCopy(char *str, int i)	{	return ColCpy(str, Column(i));	}

	// Compare ith column with a string.
	int ColCmp(const char *str, const int i);

	// Print a column.
	bool PrintColumn(int i);
	
	// Print this row. Ex. FormatTable.cpp
	inline void Print()	{ puts(row); }
	inline void Copy(char *str)	{ strcpy(str, row); }
	inline void Print(FILE *fp)	{ fprintf(fp, "%s\n", row); }
	inline void PrintWithoutNewline() { printf("%s", row); }
	inline void PrintWithoutNewline(FILE *fp)	{ fprintf(fp, "%s", row); }


	// Print the row with selected columns, selected by a STRING of {'0','1'}^*.
	// Ex. FormatTable.cpp, SelectColumns.cpp
	void PrintSelectedColumns(const char *mask);

	// Check the format of each column with a STRING of {'N','A','E'}^*. N: Number. A: Non-empty. E: End.
	// Ex. FormatTable.cpp
	bool CheckFormat(const char *sC, const char *comment);

	TableRow(char *str)	{ Init();	Get(str); }
	TableRow() { Init(); }
	~TableRow() { Clear(); }
};

/*-------------------------------------------------------------------------------------------------------
	Vestiges or Untested
	Use Carafully
-------------------------------------------------------------------------------------------------------*/

// Return the number of columns of a STRING.
int NumberOfColumns(const char *str);
