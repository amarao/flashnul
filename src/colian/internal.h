claCfg_t* claInit( int OptListNum, int maxFreeOpt );
int ClassOpt( char* line );
int OptNameCmp( const char* sample, const char* name, int case_ignore_flag );
int GetOptionID(
		const char* line,
		const claOptName_t* OptList,
		const int OptListNum,
		int type
);

int GetShortOptOdd( const char letter, claOptName_t* OptList, const int OptListNum );

int SetArguments(const char* line, claOptName_t* optN, claCfgItem_t* optV );

int GetLine( FILE* from, char** target, int *target_size );

void CropComment( char* line );

char* SkipSpaces(const char* line);

char* SkipEquiv(const char* line);

void CropCommentAndCRLF(char* line);

int LineToOpt(claCfg_t* cfg,claOptName_t* opt, int optn, char* parse_line, int area );
