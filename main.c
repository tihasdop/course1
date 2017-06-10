#include <stdio.h>
#include <stdlib.h>

#include "zip.h"
#include "passgen.h"
#include "passwd_test.h"
#include "debug.h"

#define PROGRAM_NAME "ziphack"

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		puts("Usage:\n\t" PROGRAM_NAME " <zip file>");
		exit(1);
	}

	char *file_name = argv[1]; //TODO: -d support
	FILE *zipfile = fopen(file_name, "r");
	if (!zipfile) {
		printf("Can't open zip file: %s\n", argv[1]);
		exit(1);
	}

	//TODO: max files count?
	unsigned files_count;
	struct file_brute *files = zip_parse(zipfile, &files_count);

	passgen_init(PgOptDigAndLet);
	char *passwd;
	int correct;
	while ((passwd = passgen_get()) != NULL) {
		DEBUG(printf("\nDEBUG: check passwd=%s\n", passwd));
		correct = 1;
		for (unsigned i = 0; i < files_count; ++i) {
			if (!header_test(passwd, files[i].enc_header, files[i].chkbyte)
				|| !unzip_test(file_name, passwd))
			{
				DEBUG(printf("DEBUG: passwd fail on file #%u\n", i));
				correct = 0;
				break;
			}
		}

		if (correct) {
			printf("'%s' is a correct password\n", passwd);
			break;
		}

		//free(passwd);
	}

	if (!correct)
		puts("\nA correct password hasn't found.");

	free(files);
	return 0;
}
