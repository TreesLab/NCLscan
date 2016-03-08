struct CONFIG
{
	char rG[255];
	char rR[255];
	char rN[255];
	char annotation[255];
	int rlen;
	int fsize;
};

int ReadConfig(const char *str, CONFIG *p);
