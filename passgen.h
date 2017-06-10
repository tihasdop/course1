#ifndef H__PASSGEN
#define H__PASSGEN

enum passgen_option {
//	PgOptAll = 0,
	PgOptPrinted = 1,
	PgOptDigAndLet = 2
};

void passgen_init(unsigned char pg_opt);

extern char *(*passgen_get)(void);

#endif
