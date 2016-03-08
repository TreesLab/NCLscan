#include <stdio.h>
#include <string>
#include <vector>
#include <map>

using namespace std;

// Copy alphabetic letters only.
char *AlphaCpy(char *des_str, const char *sou_str);

char *ToColorSpace(char *cs_seq, const char *ns_seq);
char *ToComplement(char *rseq, const char *seq);
char *ReverseStr(char *rseq, const char *seq);
void PrintSeq(const char *str, int width = 60, FILE *fp = stdout);

// ex. SeqOut.cpp
class Sequence: public string
{
	char *name;
	char *title;

	void Init();
	void Clear();
public:

	bool Get(FILE *fp = stdin);
	bool Get(const char *tit, const char *seq);

	inline char *Title()	{	return title;	}
	inline char *Name()	{	return name;	}

	void PrintSequence(int width = 60, FILE *fp = stdout);

	void Print(int width = 60, FILE *fp = stdout);
	inline void Print( FILE *fp )	{	Print(60, fp);	}

	Sequence()	{ Init(); }
	~Sequence()	{ Clear(); }
};

// ex. FastqOut
class Fastq
{
	char *name;
	char *seqline;
	char *plusline;
	char *qsline;

	void Init();
	void Clear();
public:

	bool Get(FILE *fp = stdin);
	void Print(FILE *fp = stdout);
	void PrintReverseStrand(FILE *fp = stdout);
	
	inline char *Name() {	return name;	}
	inline char *Seq()	{	return seqline;	}
	inline char *Plusline()	{	return plusline;	}
	inline char *QS()	{	return qsline;	}

	Fastq() { Init(); }
	~Fastq() { Clear(); }	
};

// ex. CsfastaOut.cpp
class Csfasta
{
	char *name;
	char *seqline;


	void Init();
	void Clear();
public:

	bool Get(FILE *fp = stdin);
	void Print(FILE *fp = stdout);
	
	inline char *Name() {	return name;	}
	inline char *Seq()	{	return seqline;	}

	Csfasta() { Init(); }
	~Csfasta() { Clear(); }	
};

// ex. ChimeraParser.cpp
class SeqPool
{
	Sequence *SeqArr;
	int numEntries;
	map <string, int> SeqMap;
	Sequence seq;

	void Init();
	void Clear();
public:

	void Add(FILE *fpFasta);
	void Add(const char *infilename);
	Sequence *GiveMe(const char *Name);
	Sequence *GiveMe(int i);

	SeqPool();
	~SeqPool();
};


