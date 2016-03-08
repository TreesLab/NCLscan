#include <stdio.h>
#include <limits.h>

#define MaxIdSize	256
#define MaxReadSize	512
#define	MaxSamLen	65536
#define Invalid		(INT_MIN/2)

class SamOutput
{
	bool strand;	
	bool empty;
	bool headline;

	void Init();
	void Clear() {	;	}
public:
	// Essentail columns of SAM format
	char	qName[MaxIdSize+1];
	int		Flag;
	char	rName[MaxIdSize+1];
	int		Pos;
	int		MapQ;
	char	CIGAR[MaxReadSize+1];
	char	NRNM[MaxIdSize+1];
	int		MPos;
	int		ISize;
	char	Seq[MaxReadSize+1];
	char	Qual[MaxReadSize+1];
	
	// Optional fields of SAM format
	char	PG[MaxIdSize+1];
	int		AS;
	int		NM;
	int		NH;
	int		IH;
	int		HI;
	char	MD[MaxReadSize+1];
	char	CS[MaxReadSize+1];
	char	CQ[MaxReadSize+1];
	int		CM;
	char	CC[MaxIdSize+1];
	int		CP;
	char	XA[MaxReadSize+1];
	char	XE[MaxReadSize+1];
	int		Z3;

	char	SN[MaxIdSize+1];
	int	LN;
	
	// Founctions	
	 
	inline bool IsHeadline() { return headline; }
	bool Get(FILE *fp = stdin);
	bool Get(char *str);
	void Print();

	// String-print essentail columns 
	int SprintEss(char *str);

	inline bool IsPlusStrand()	{	if( strand == 0 ) return true;	return false; }
	inline bool IsEmpty()	{	return empty;	}
};
