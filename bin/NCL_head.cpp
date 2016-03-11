#include <stdio.h>
#include <stdlib.h>
#include "Common.h"
#include "Table.h"
#include "NCL_head.h"

int ReadConfig(const char *str, CONFIG *conf)
{
	FILE *fp;
	TableRow theRow;

	if(	( fp = fopen(str, "r") ) == NULL )
		return 1;	

	if( !theRow.Get(fp) )
		return 1;
	theRow.ColCopy( conf->rG, 0);

	if( !theRow.Get(fp) )
		return 1;
	theRow.ColCopy( conf->rR, 0);

	if( !theRow.Get(fp) )
		return 1;
	theRow.ColCopy( conf->rN, 0);

	if( !theRow.Get(fp) )
		return 1;
	theRow.ColCopy( conf->annotation, 0);

	if( !theRow.Get(fp) )
		return 1;
	conf->rlen = theRow.ColToI(0);

	if( !theRow.Get(fp) )
		return 1;
	conf->fsize = theRow.ColToI(0);

	fclose(fp);

	return 0;
}

