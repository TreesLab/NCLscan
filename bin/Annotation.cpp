#include <stdio.h>
#include "Common.h"
#include "Table.h"
#include "Annotation.h" 

#define MaxIndex 5124568

Mini_GTF::Mini_GTF(FILE *fp)
{
	int i, g, t, e;
	char str[81];
	A =  new mini_GTF[MaxIndex];

	count = 0;
	i=0;
	g=0;
	t=0;
	e=0;
	for( TableRow theRow; theRow.Get(fp); )
	{
		A[i].feature = 0;

		if(strcmp(theRow.ColCopy(str, 2), "gene") == 0)
		{
			A[i].feature = gene; 
			g++;
		}
		else if(strcmp(theRow.ColCopy(str, 2), "transcript") == 0)
		{
			A[i].feature = transcript; 
			t++;
		}
		else if(strcmp(theRow.ColCopy(str, 2), "exon") == 0)
		{
			A[i].feature = exon; 
			e++;
		}
		else
			continue;

		if(A[i].feature != 0)
		{
			if( theMap.IndexTheMap(theRow.ColCopy(str,0)) == -1 )
			{
				fprintf(stderr, "Reading annotations on %s.\n", theRow.ColCopy(str,0));
				theMap.Add(theRow.ColCopy(str,0));
				index[theMap.IndexTheMap(theRow.ColCopy(str,0))] = i;
			}
			A[i].seqname = theMap.IndexTheMap(theRow.ColCopy(str,0));
			A[i].start = theRow.ColToI(3);
			A[i].end = theRow.ColToI(4); 
			A[i].strand = theRow.Column(6)[0];

			i++;
		}

		if( i >= (MaxIndex-1) )
		{
			ErrMsg("The GTF file contains too many lines.\n");
		}
		
	}	

	fprintf(stderr, "Read %d genes, %d transcripts and %d exons from the gtf file.\n", g, t, e);
}

void Mini_GTF::PrintInBedByNearbyJunctions(const char *seqname, int pos, char strand, bool type, int length, const char *name, int distance)
{
	int i;

	int i_seq;

	i_seq = theMap.IndexTheMap(seqname);

	for( i = index[i_seq]; i_seq == A[i].seqname; i++ )
	{
		if( A[i].feature == transcript )
		{
			if( A[i].start >= pos )
				break;
		}
		else if( A[i].feature == exon )
		{
			if( A[i].start <= pos && A[i].end >= pos && A[i].strand == strand )
			{	
				if( strand == '+' && type == FivePrimeEnd )
				{
					if( (pos - A[i].start) <= distance )
					{							
						//printf("i:%d pos:%d distance:%d\n",i, pos, distance);
						Str = "";
						mStr = "";

						PrintInBed5Plus(seqname, A[i].start, i, length, name, ++count);

						if( !boolMap.IsInMap(mStr.c_str()) ) 
						{
							boolMap.Add(mStr.c_str());
							printf("%s", Str.c_str());
						}

						sPrintInBedByNearbyJunctions(seqname, A[i-1].end, strand, type, length, name, distance - ( pos - A[i].start ), i-1);
					}	
				}
				else if( strand == '+' && type == ThreePrimeEnd )
				{
					if( (A[i].end - pos) <= distance )
					{
						//printf("i:%d pos:%d distance:%d\n",i, pos, distance);
						Str = "";
						mStr = "";
					
						PrintInBed3Plus(seqname, A[i].end, i, length, name, ++count);

						if( !boolMap.IsInMap(mStr.c_str()) ) 
						{
							boolMap.Add(mStr.c_str());
							printf("%s", Str.c_str());
						}

						sPrintInBedByNearbyJunctions(seqname, A[i+1].start, strand, type, length, name, distance - (A[i].end - pos), i+1);
					}	
				}
				else if( strand == '-' && type == FivePrimeEnd )
				{
					if( (A[i].end - pos) <= distance )
					{
						//printf("i:%d pos:%d distance:%d\n",i, pos, distance);
						Str = "";
						mStr = "";

						PrintInBed5Minus(seqname, A[i].end, i, length, name, ++count);

						if( !boolMap.IsInMap(mStr.c_str()) ) 
						{
							boolMap.Add(mStr.c_str());
							printf("%s", Str.c_str());
						}

						sPrintInBedByNearbyJunctions(seqname, A[i-1].start, strand, type, length, name, distance - (A[i].end - pos), i-1);
					}	

				}
				else if( strand == '-' && type == ThreePrimeEnd )
				{
					if( (pos - A[i].start) <= distance )
					{
						//printf("i:%d pos:%d distance:%d\n",i, pos, distance);
						Str = "";
						mStr = "";

						PrintInBed3Minus(seqname, A[i].start, i, length, name, ++count);

						if( !boolMap.IsInMap(mStr.c_str()) ) 
						{
							boolMap.Add(mStr.c_str());
							printf("%s", Str.c_str());
						}

						sPrintInBedByNearbyJunctions(seqname, A[i+1].end, strand, type, length, name, distance - (pos - A[i].start), i+1);
					}	
				}

			} 
		}		
	}
}

void Mini_GTF::sPrintInBedByNearbyJunctions(const char *seqname, int pos, char strand, bool type, int length, const char *name, int distance, int i)
{
	if( A[i].feature == exon )
	{
		if( A[i].start <= pos && A[i].end >= pos && A[i].strand == strand )
		{	
			if( strand == '+' && type == FivePrimeEnd )
			{
				if( (pos - A[i].start) <= distance )
				{							
					//printf("i:%d pos:%d distance:%d\n",i, pos, distance);
					Str = "";
					mStr = "";

					PrintInBed5Plus(seqname, A[i].start, i, length, name, ++count);
	
					if( !boolMap.IsInMap(mStr.c_str()))
					{
						boolMap.Add(mStr.c_str());
						printf("%s", Str.c_str());
					}

					sPrintInBedByNearbyJunctions(seqname, A[i-1].end, strand, type, length, name, distance - ( pos - A[i].start ), i-1);
				}	
			}
			else if( strand == '+' && type == ThreePrimeEnd )
			{
				if( (A[i].end - pos) <= distance )
				{
					//printf("i:%d pos:%d distance:%d\n",i, pos, distance);
					Str = "";
					mStr = "";

					PrintInBed3Plus(seqname, A[i].end, i, length, name, ++count);

					if( !boolMap.IsInMap(mStr.c_str()))
					{
						boolMap.Add(mStr.c_str());
						printf("%s", Str.c_str());
					}

					sPrintInBedByNearbyJunctions(seqname, A[i+1].start, strand, type, length, name, distance - (A[i].end - pos), i+1);
				}	
			}
			else if( strand == '-' && type == FivePrimeEnd )
			{
				if( (A[i].end - pos) <= distance )
				{
					//printf("i:%d pos:%d distance:%d\n",i, pos, distance);
					Str = "";
					mStr = "";

					PrintInBed5Minus(seqname, A[i].end, i, length, name, ++count);

					if( !boolMap.IsInMap(mStr.c_str()))
					{
						boolMap.Add(mStr.c_str());
						printf("%s", Str.c_str());
					}

					sPrintInBedByNearbyJunctions(seqname, A[i-1].start, strand, type, length, name, distance - (A[i].end - pos), i-1);
				}	

			}
			else if( strand == '-' && type == ThreePrimeEnd )
			{
				if( (pos - A[i].start) <= distance )
				{
					//printf("i:%d pos:%d distance:%d\n",i, pos, distance);
					Str = "";
					mStr = "";

					PrintInBed3Minus(seqname, A[i].start, i, length, name, ++count);

					if( !boolMap.IsInMap(mStr.c_str()))
					{
						boolMap.Add(mStr.c_str());
						printf("%s", Str.c_str());
					}

					sPrintInBedByNearbyJunctions(seqname, A[i+1].end, strand, type, length, name, distance - (pos - A[i].start), i+1);
				}	
			}
		} 
	}
}

void Mini_GTF::PrintInBedByLength(const char *seqname, int pos, char strand, bool type, int length, const char *name)
{
	int i;

	int i_seq;
	 
	i_seq = theMap.IndexTheMap(seqname);

	for( i = index[i_seq]; i_seq == A[i].seqname; i++ )
	{
		if( A[i].feature == transcript )
		{
			if( A[i].start >= pos )
				break;
		}
		else if( A[i].feature == exon )
		{
			if( A[i].start <= pos && A[i].end >= pos && A[i].strand == strand )
			{	
				if( strand == '+' && type == FivePrimeEnd )
				{
					PrintInBed5Plus(seqname, pos, i, length, name, count);
					count++;
				}
				else if( strand == '+' && type == ThreePrimeEnd )
				{
					PrintInBed3Plus(seqname, pos, i, length, name, count);
					count++;
				}
				else if( strand == '-' && type == FivePrimeEnd )
				{
					PrintInBed5Minus(seqname, pos, i, length, name, count);
					count++;
				}
				else if( strand == '-' && type == ThreePrimeEnd )
				{
					PrintInBed3Minus(seqname, pos, i, length, name, count);
					count++;
				}
			} 
		}		
	}
}

void Mini_GTF::PrintInBed5Plus(const char *seqname, int pos, int i, int length, const char *name, int id)
{
	int ThisStart, ThisEnd;

	if(A[i].feature != exon)
	{
		if(length > 0)
		{
			ThisStart = A[i-1].end + 1;
			ThisEnd = ThisStart + length - 1;
//			sprintf(strbuf, "%s\t%d\t%d\t%s.%d\t%d\t%c\n", seqname, ThisStart-1, ThisEnd, name, id, id, A[i-1].strand);
			sprintf(strbuf, "%s\t%d\t%d\t%s.%d\t%d\t%c\n", seqname, ThisStart-1, ThisEnd, name, id, id, A[i-1].strand);

			Str += strbuf;
			sprintf(strbuf, "%s%d%d%s%c", seqname, ThisStart-1, ThisEnd, name, A[i-1].strand);
			mStr += strbuf;
		}

		return;
	}

	if(A[i].end < pos)
	{
		fprintf(stderr, "Error: %s:%d in the current transcript is not within an exon.\n", seqname, pos);
		PrintInBed5Plus(seqname, A[i+1].start, i+1, length, name, id);
		return;
	}

	if(A[i].start > pos)
	{
		fprintf(stderr, "Error: %s:%d in the current transcript is not within an exon.\n", seqname, pos);
		PrintInBed5Plus(seqname, A[i].start, i, length, name, id);
		return;
	}

	ThisStart = pos;

	if( ( A[i].end - ThisStart + 1 ) >= length )
	{
		ThisEnd = ThisStart + length - 1;
//		printf(strbuf, "%s\t%d\t%d\t%s.%d\t%d\t%c\n", seqname, ThisStart-1, ThisEnd, name, id, id, A[i].strand);		

		sprintf(strbuf, "%s\t%d\t%d\t%s.%d\t%d\t%c\n", seqname, ThisStart-1, ThisEnd, name, id, id, A[i].strand);
		Str += strbuf;
		sprintf(strbuf, "%s%d%d%s%c", seqname, ThisStart-1, ThisEnd, name, A[i].strand);
		mStr += strbuf;

		return;
	}

	length -= ( A[i].end - ThisStart + 1 );	

	ThisEnd = A[i].end;
//	printf(strbuf, "%s\t%d\t%d\t%s.%d\t%d\t%c\n", seqname, ThisStart-1, ThisEnd, name, id, id, A[i].strand);

//			printf("%s", strbuf);

	sprintf(strbuf, "%s\t%d\t%d\t%s.%d\t%d\t%c\n", seqname, ThisStart-1, ThisEnd, name, id, id, A[i].strand);
	Str += strbuf;
	sprintf(strbuf, "%s%d%d%s%c", seqname, ThisStart-1, ThisEnd, name, A[i].strand);
	mStr += strbuf;

	PrintInBed5Plus(seqname, A[i+1].start, i+1, length, name, id);
	return;
}

void Mini_GTF::PrintInBed3Plus(const char *seqname, int pos, int i, int length, const char *name, int id)
{
	int ThisStart, ThisEnd;

	if(A[i].feature != exon)
	{
		if(length > 0)
		{
			ThisEnd = A[i+1].start - 1;
			ThisStart = ThisEnd - length + 1;
			sprintf( strbuf, "%s\t%d\t%d\t%s.%d\t%d\t%c\n", seqname, ThisStart-1, ThisEnd, name, id, id, A[i+1].strand);
			Str += strbuf;
			sprintf(strbuf, "%s%d%d%s%c", seqname, ThisStart-1, ThisEnd, name, A[i+1].strand);
			mStr += strbuf;
		}

		return;
	}

	if(A[i].end < pos)
	{
		fprintf(stderr, "Error: %s:%d in the current transcript is not within an exon.\n", seqname, pos);
		PrintInBed3Plus(seqname, A[i].end, i, length, name, id);
		return;
	}

	if(A[i].start > pos)
	{
		fprintf(stderr, "Error: %s:%d in the current transcript is not within an exon.\n", seqname, pos);
		PrintInBed3Plus(seqname, A[i-1].end, i-1, length, name, id);
		return;
	}

	ThisEnd = pos;

	if( ( ThisEnd - A[i].start + 1 ) >= length )
	{
		ThisStart = ThisEnd - length + 1;
		sprintf( strbuf, "%s\t%d\t%d\t%s.%d\t%d\t%c\n", seqname, ThisStart-1, ThisEnd, name, id, id, A[i].strand);
		Str += strbuf;
		sprintf( strbuf, "%s%d%d%s%c", seqname, ThisStart-1, ThisEnd, name, A[i].strand);
		mStr += strbuf;

		return;
	}

	length -= ( ThisEnd - A[i].start + 1 );	
	ThisStart = A[i].start;

	PrintInBed3Plus(seqname, A[i-1].end, i-1, length, name, id);

	sprintf(strbuf, "%s\t%d\t%d\t%s.%d\t%d\t%c\n", seqname, ThisStart-1, ThisEnd, name, id, id, A[i].strand);
	Str += strbuf;
	sprintf( strbuf, "%s%d%d%s%c", seqname, ThisStart-1, ThisEnd, name, A[i].strand);
	mStr += strbuf;

	return;
}

void Mini_GTF::PrintInBed5Minus(const char *seqname, int pos, int i, int length, const char *name, int id)
{
	int ThisStart, ThisEnd;

	if(A[i].feature != exon)
	{
		if(length > 0)
		{
			ThisEnd = A[i-1].start - 1;
			ThisStart = ThisEnd - length + 1;
			sprintf(strbuf, "%s\t%d\t%d\t%s.%d\t%d\t%c\n", seqname, ThisStart-1, ThisEnd, name, id, id, A[i-1].strand);
			Str += strbuf;
			sprintf(strbuf, "%s%d%d%s%c", seqname, ThisStart-1, ThisEnd, name, A[i-1].strand);
			mStr += strbuf;
		}

		return;
	}

	if(A[i].end < pos)
	{
		PrintInBed5Minus(seqname, A[i].end, i, length, name, id);
		fprintf(stderr, "Error: %s:%d in the current transcript is not within an exon.\n", seqname, pos);
		return;
	}

	if(A[i].start > pos)
	{
		PrintInBed5Minus(seqname, A[i+1].end, i+1, length, name, id);
		fprintf(stderr, "Error: %s:%d in the current transcript is not within an exon.\n", seqname, pos);
		return;
	}

	ThisEnd = pos;

	if( ( ThisEnd - A[i].start + 1 ) >= length )
	{
		ThisStart = ThisEnd - length + 1;
		sprintf(strbuf, "%s\t%d\t%d\t%s.%d\t%d\t%c\n", seqname, ThisStart-1, ThisEnd, name, id, id, A[i].strand);
		Str += strbuf;
		sprintf(strbuf, "%s%d%d%s%c", seqname, ThisStart-1, ThisEnd, name, A[i].strand);
		mStr += strbuf;
		return;
	}

	length -= ( ThisEnd - A[i].start + 1 );	
	ThisStart = A[i].start;

	sprintf(strbuf, "%s\t%d\t%d\t%s.%d\t%d\t%c\n", seqname, ThisStart-1, ThisEnd, name, id, id, A[i].strand);
	Str += strbuf;
	sprintf(strbuf, "%s%d%d%s%c", seqname, ThisStart-1, ThisEnd, name, A[i].strand);
	mStr += strbuf;

	PrintInBed5Minus(seqname, A[i+1].end, i+1, length, name, id);

	return;
}

void Mini_GTF::PrintInBed3Minus(const char *seqname, int pos, int i, int length, const char *name, int id)
{
	int ThisStart, ThisEnd;

	if(A[i].feature != exon)
	{
		if(length > 0)
		{
			ThisStart = A[i+1].end + 1;
			ThisEnd = ThisStart + length - 1;
			sprintf(strbuf, "%s\t%d\t%d\t%s.%d\t%d\t%c\n", seqname, ThisStart-1, ThisEnd, name, id, id, A[i-1].strand);
			Str += strbuf;
			sprintf(strbuf, "%s%d%d%s%c", seqname, ThisStart-1, ThisEnd, name, A[i-1].strand);		
			mStr += strbuf;
		}

		return;
	}

	if(A[i].end < pos)
	{
		PrintInBed3Minus(seqname, A[i-1].start, i-1, length, name, id);
		fprintf(stderr, "Error: %s:%d in the current transcript is not within an exon.\n", seqname, pos);
		return;
	}

	if(A[i].start > pos)
	{
		PrintInBed3Minus(seqname, A[i].start, i, length, name, id);
		fprintf(stderr, "Error: %s:%d in the current transcript is not within an exon.\n", seqname, pos);
		return;
	}

	ThisStart = pos;

	if( ( A[i].end - ThisStart + 1 ) >= length )
	{
		ThisEnd = ThisStart + length - 1;
		sprintf(strbuf, "%s\t%d\t%d\t%s.%d\t%d\t%c\n", seqname, ThisStart-1, ThisEnd, name, id, id, A[i].strand);
		Str += strbuf;
		sprintf(strbuf, "%s%d%d%s%c", seqname, ThisStart-1, ThisEnd, name, A[i].strand);		
		mStr += strbuf;

		return;
	}

	length -= ( A[i].end - ThisStart + 1 );	
	ThisEnd = A[i].end;

	PrintInBed3Minus(seqname, A[i-1].start, i-1, length, name, id);
	
	sprintf(strbuf, "%s\t%d\t%d\t%s.%d\t%d\t%c\n", seqname, ThisStart-1, ThisEnd, name, id, id, A[i].strand);
	Str += strbuf;
	sprintf(strbuf, "%s%d%d%s%c", seqname, ThisStart-1, ThisEnd, name, A[i].strand);		
	mStr += strbuf;

	return;
}
