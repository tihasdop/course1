/*
main.c -- is a part of the ziphack

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zip.h"
#include "passgen.h"
#include "passwd_test.h"
#include "debug.h"

#define PROGRAM_NAME "ziphack"

char *help = "Usage:\n\t" PROGRAM_NAME " [-d <dictionary file>] [-P] <zip file>\n\n"
	"\t-d -- use passwords dictionary\n"
	"\t-P -- use all printed ASCII chars as possible in password\n";

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		fputs(help, stdout);
		exit(1);
	}

	char *filename = NULL;
	char *dic_filename = NULL;
	int pass_mode = PgOptDigAndLet;

	for (unsigned i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			if (!strcmp(argv[i], "-d")) {
				if (pass_mode == PgOptDigAndLet) {
					if (i < (argc - 1)) {
						pass_mode = PgOptDictionary;
						dic_filename = argv[++i];
					} else {
						printf("'-d' found, but a dictionary's filename wasn't.\n");
						exit(1);
					}
				} else {
					printf("'-d' found, but pass_mode already defined.\n");
					exit(1);
				}
			} else if (!strcmp(argv[i], "-P")) {
				if (pass_mode == PgOptDigAndLet) {
					pass_mode = PgOptPrinted;
				} else {
					printf("'-P' found, but pass_mode already defined.\n");
					exit(1);
				}
			} else {
				printf("Unknown option: '%s'\n", argv[i]);
				exit(1);
			}
		} else {
			if (!filename) {
				filename = argv[i];
			} else {
				printf("A excess file name argument had been found: '%s'.\n", argv[i]);
				exit(1);
			}
		}
	}


	FILE *zipfile = fopen(filename, "r");
	if (!zipfile) {
		printf("Can't open zip file: %s\n", filename);
		exit(1);
	}


	unsigned files_count;
	struct file_brute *files = zip_parse(zipfile, &files_count);

	passgen_init(pass_mode, dic_filename);

	char *passwd;
	int correct;
	while ((passwd = passgen_get()) != NULL) {
		DEBUG(printf("check passwd=%s\n", passwd));
		correct = 1;
		for (unsigned i = 0; i < files_count; ++i) {
			if (!header_test(passwd, files[i].enc_header, files[i].chkbyte))
			{
				DEBUG(printf("DEBUG: passwd fail on file #%u\n", i));
				correct = 0;
				break;
			}
		}
		if (correct && unzip_test(filename, passwd)) {
			printf("'%s' is a correct password\n", passwd);
			break;
		}
	}

	if (!correct)
		puts("\nA correct password hasn't found.");

	free(files);
	return 0;
}
