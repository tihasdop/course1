#include <stdio.h>
#include <stdlib.h>

#include "zip.h"
#include "passgen.h"
#include "passwd_test.h"
#include "debug.h"

#define PROGRAM_NAME "ziphack"

//TODO: rewrite
int
main(int argc, char *argv[])
{
	if (argc != 2) {
		puts("Usage:\n\t" PROGRAM_NAME " <zip file>");
		exit(1);
	}

	FILE *zipfile = fopen(argv[1], "r");
	if (!zipfile) {
		printf("Can't open zip file: %s\n", argv[1]);
		exit(1);
	}

	unsigned files_count;
	struct file_brute *files = zip_parse(zipfile, &files_count);
	//TODO: error checking

	passgen_init();
	char *passwd;
	int correct;
	while ((passwd = passgen_get()) != NULL) {
		DEBUG(printf("\nDEBUG: current passwd=%s\n", passwd));
		correct = 1;
		for (unsigned i = 0; i < files_count; ++i) {
			if (! is_correct_passwd(passwd, files[i].enc_header, files[i].crc32)) {
				DEBUG(printf("DEBUG: passwd fail on file #%u\n", i));
				correct = 0;
				break;
			}
		}

		if (correct) {
			printf("'%s' is a correct password", passwd);
			exit(0);
		}

		free(passwd);
	}

	puts("\nA correct password hasn't founded.");

	//free(files);
	return 0;
}
