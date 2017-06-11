/*
passgen.c -- is a part of the ziphack

Copyright (c) 2017 Alexander Tomilov

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "passgen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_PASS_LEN 100

char *(*passgen_get)(void);

char cur_pass[MAX_PASS_LEN];
char *lastc = &cur_pass[MAX_PASS_LEN - 1];

FILE *dic_file = NULL;

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

char *
pass_gen_get_dictionary(void)
{
	int c;
	char *cur = cur_pass;
	do {
		for (;;) {
			c = fgetc(dic_file);

			if (ferror(dic_file)) {
				printf("Error while reading dictionary file.\n");
				exit(1);
			} else if (c == '\n') {
				break;
			} else if (c != '\r') { // ignore '\r'
				if (cur == lastc) // truncate long strings
					break;
				*cur++ = c;
			} else if (c == EOF) {
				break;
			}
		}
	} while (cur == cur_pass); // loop if nothing readed
	*cur = '\0';
	return cur_pass;
}

void
passgen_init(enum passgen_option pg_opt, char *dic_filename)
{
	cur_pass[0] = '\0';
	switch(pg_opt) {
	case PgOptPrinted:
		passgen_get = &passgen_get_printed;
	break;

	case PgOptDigAndLet:
		passgen_get = &passgen_get_dig_and_let;
	break;

	case PgOptDictionary:
		passgen_get = &pass_gen_get_dictionary;
		dic_file = fopen(dic_filename, "r");
		if (!dic_file) {
			printf("Can't open dictionary file: '%s'.\n", dic_filename);
			exit(1);
		}
	break;

	default:
		assert(0);
	break;
	}

	return;
}
