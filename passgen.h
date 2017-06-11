#ifndef H__PASSGEN
#define H__PASSGEN

enum passgen_option {
	PgOptPrinted = 1,
	PgOptDigAndLet = 2,
	PgOptDictionary = 3
};

void passgen_init(enum passgen_option pg_opt, char *dic_filename);

extern char *(*passgen_get)(void);

#endif
