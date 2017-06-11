#include "passwd_test.h"

#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "unzip_minizip.h"
#include "decrypt.h"

#include "debug.h"

#define DUMB_BUF_SIZE 100

const int header_len = 12;

int
unzip_test(char *path, char *password)
{
	printf("'%s' is a possible password\n", password);
	int result;

	char *full_path = realpath(path, NULL);
	unzFile zipfile = unzOpen(full_path);
	free(full_path);

	if (!zipfile) {
		printf("Error while unzip test! (zip opening)\n");
		exit(1);
	}

	if (unzGoToFirstFile(zipfile) != UNZ_OK) {
		printf("Error while unzip test! (file searching)\n");
		exit(1);
	}

	unz_file_info file_info;
	//REMOVE IT
	char filename[100];
	if (unzGetCurrentFileInfo(zipfile, &file_info, filename, 100, NULL, 0, NULL, 0) != UNZ_OK) {
		printf("Error while unzip test! (file info reading)\n");
		exit(1);
	}
	DEBUG(printf("DEBUG: uncomp_size=%lu\n", file_info.uncompressed_size));

	if (unzOpenCurrentFilePassword(zipfile, password) != UNZ_OK) {
		printf("Error while unzip test! (file opening)\n");
		exit(1);
	}

	unsigned char dumb_buf[DUMB_BUF_SIZE];
	int res;
	do {
		res = unzReadCurrentFile(zipfile, dumb_buf, DUMB_BUF_SIZE);
		if (res < 0)
			break;
	} while (res > 0);

	if ((unzCloseCurrentFile(zipfile) == UNZ_CRCERROR) || (res < 0)) { // if error
		result = 0;
	} else {
		result = 1;
	}
	DEBUG(printf("DEBUG: result=%d\n", result));

	unzClose(zipfile);
	return result;
}

int
header_test(char *password, unsigned char *header, unsigned char chkbyte)
{
	unsigned char dec_header[header_len];

	decrypt(password, header, header_len, dec_header);

	return dec_header[11] == chkbyte;
}
