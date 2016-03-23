#include "Map.h"
#include <string.h>

#define gene 'g'
#define transcript 't'
#define exon 'e'
#define FivePrimeEnd true
#define ThreePrimeEnd false

struct mini_GTF
{
	int seqname;
	char feature;
	int start;
	int end;
	char strand;
};

class Mini_GTF
{
	int count;
	mini_GTF *A;
	Str2IntMap theMap;
	int index[32768];

	Str2BoolMap boolMap;
	char strbuf[1024];
	string Str;
	string mStr;
	
private:
	void PrintInBed5Plus(const char *seqname, int pos, int i, int length, const char *name, int id);
	void PrintInBed3Plus(const char *seqname, int pos, int i, int length, const char *name, int id);
	void PrintInBed5Minus(const char *seqname, int pos, int i, int length, const char *name, int id);
	void PrintInBed3Minus(const char *seqname, int pos, int i, int length, const char *name, int id);
	void sPrintInBedByNearbyJunctions(const char *seqname, int pos, char strand, bool type, int length, const char *name, int distance, int i);
public:
	void PrintInBedByNearbyJunctions(const char *seqname, int pos, char strand, bool type, int length, const char *name, int distance);
	void PrintInBedByLength(const char *seqname, int pos, char strand, bool type, int length, const char *name);
	// Type: FivePrimeEnd/ThreePrimeEnd. Type of the given position, 5' end or 3' end.  

	Mini_GTF(FILE *fp);
	~Mini_GTF() {};
};
