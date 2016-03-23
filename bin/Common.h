#include <stdio.h>
#include <time.h>
#define MaxLineSize	65535

// Print error message.
inline void ErrMsg(const char *str)
{	fprintf(stderr, "%s", str);	}

// Convert an INT into a STRING.
char *ItoA(int value, char* result);
char *ItoA(int value, char* result, int base);

// Reverse a string
char *StrRev(char *rstr, const char *str);

// Print processing times.
inline void Times()
{	fprintf(stderr, "Processing times: %d seconds.\n", (int)(clock()/CLOCKS_PER_SEC) );	}

// System call and write the result into a file
int Popen(const char *com, const char *filename);

int Binary2Int(const char *B);
