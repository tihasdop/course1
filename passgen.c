#include "passgen.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_PASS_LEN 100

char *(*passgen_get)(void);
char cur_pass[MAX_PASS_LEN];
char *lastc = &cur_pass[MAX_PASS_LEN - 2];

char *
passgen_get_printed(void)
{
	char *c;
	for (c = cur_pass; *c; ++c) {
		if (*c < '~') {
			++(*c);
			return cur_pass;
		} else {
			*c = ' ';
		}
	}

	if (c == lastc)
		return NULL;

	*(c + 1) = '\0';
	for (c = cur_pass; *c; ++c)
		*c = ' ';

	return cur_pass;
}

char *
passgen_get_dig_and_let(void)
{
	char *c;
	for (c = cur_pass; *c; ++c) {
		if ((*c < '9') ||
			((*c >= 'A') && (*c < 'Z')) ||
			((*c >= 'a') && (*c < 'z')))
		{
			++(*c);
			return cur_pass;
		} else if (*c == '9') {
			*c = 'A';
			return cur_pass;
		} else if (*c == 'Z') {
			*c = 'a';
			return cur_pass;
		} else if (*c == 'z') {
			*c = '0';
		} else {
			assert(0);
		}
	}
	if (c == lastc)
		return NULL;

	*(c + 1) = '\0';
	for (c = cur_pass; *c; ++c)
		*c = '0';
	*c = '0';

	return cur_pass;
}

void
passgen_init(unsigned char pg_opt)
{
	switch(pg_opt) {
	case PgOptPrinted:
		passgen_get = &passgen_get_printed;
		cur_pass[0] = ' ';
		cur_pass[1] = '\0';
	break;

	case PgOptDigAndLet:
		passgen_get = &passgen_get_dig_and_let;
		cur_pass[0] = '0';
		cur_pass[1] = '\0';
	break;

	default:
		assert(0);
	break;
	}

	return;
}
