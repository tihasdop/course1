#include "passwd_test.h"

#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "unzip_minizip.h"
#include "decrypt.h"

//REMOVE IT it's for testing
#include "debug.h"

const int header_len = 12;

int
unzip_test(char *path, char *password)
{
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
	if (unzGetCurrentFileInfo(zipfile, &file_info, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK) {
		printf("Error while unzip test! (file info reading)\n");
		exit(1);
	}
	DEBUG(printf("DEBUG: uncomp_size=%lu\n", file_info.uncompressed_size));

	if (unzOpenCurrentFilePassword(zipfile, password) != UNZ_OK) {
		printf("Error while unzip test! (file opening)\n");
		exit(1);
	}

	unsigned char *dumb_buf = malloc(file_info.uncompressed_size);
	if (unzReadCurrentFile(zipfile, dumb_buf, file_info.uncompressed_size) != file_info.uncompressed_size) {
		printf("Error while unzip test! (file reading)\n");
		exit(1);
	}
	free(dumb_buf);

	if (unzCloseCurrentFile(zipfile) != UNZ_CRCERROR) {
		result = 1;
	} else {
		result = 0;
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
