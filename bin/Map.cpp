#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include "Common.h"
#include "Table.h"
#include "Map.h"

using namespace std;

void Str2BoolMap::Add(const char *str)
{
	if( themap.find(str) == themap.end() )
		themap[str] = true;
}

void Str2BoolMap::AddIntoMap(const char *filename, int col)
{
	FILE *fp;
	char str[MaxLineSize+1];

	if( ( fp = fopen(filename, "r") ) == NULL )	return;

	while(1)
	{
		TableRow theRow;
		if( theRow.Get(fp) == false )
			break;
		theRow.ColCopy(str, col);
		
		if( themap.find(str) == themap.end() )
			themap[str] = true;
	}
	
	fclose(fp);	
}



bool Str2BoolMap::IsInMap(const char *str)
{
	if( themap.find(str) != themap.end() )
		return true;
	return false;
}


void Str2IntMap::Add(const char *str)
{
	static int i = 0;

	if( themap.find(str) == themap.end() )
	{
		themap[str] = i;
		i++;
	}
}

int Str2IntMap::AddIntoMap(const char *filename, int col)
{
	FILE *fp;
	int i = 0;
	char str[MaxLineSize+1];

	if(	( fp = fopen(filename, "r") ) == NULL )	return i;

	i = 0;
	while(1)
	{
		TableRow theRow;
		if( theRow.Get(fp) == false )
			return i;
		theRow.ColCopy(str, col);

		if( themap.find(str) == themap.end() )
		{
			themap[str] = i;
			i++;
		}
	}	

	fclose(fp);
	return i;
}

int Str2IntMap::IndexTheMap(const char *str)
{
	if( themap.find(str) != themap.end() )
		return themap.find(str)->second;
	return -1;
}

int Str2StrMap::AddIntoMap(const char *filename, int key_col, int data_col)
{
	FILE *fp;
	int i = 0;
	char str4key[MaxLineSize+1];
	char str4data[MaxLineSize+1];

	if(	( fp = fopen(filename, "r") ) == NULL )	return i;

	i = 0;
	while(1)
	{
		TableRow theRow;
		if( theRow.Get(fp) == false )
			return i;
		theRow.ColCopy(str4key, key_col);
		theRow.ColCopy(str4data, data_col);

		if( themap.find(str4key) == themap.end() )
		{
			themap[str4key] = str4data;
			i++;
		}
	}	

	fclose(fp);
	return i;
}

char* Str2StrMap::At(const char *str, char *buf)
{
	strcpy( buf, themap[str].c_str() );
	return buf;
}

