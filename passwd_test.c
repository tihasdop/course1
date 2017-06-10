#include "passwd_test.h"

#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <unzip.h>
#include "decrypt.h"

//REMOVE IT it's for testing
#include "debug.h"

const int header_len = 12;

int
unzip_test(char *path, char *password)
{
	char *argv[5];
	UzpInit uzpi;

	argv[0] = "unzip";
	argv[1] = "-t";

	size_t password_size = strlen(password) + 1;
	char *pwbuf = malloc(sizeof(char) * (password_size + 2);
	pwbuf[0] = '-'; pwbuf[1] = 'P';
	memcpy(pwbuf + 2, password, sizeof(char) * password_size);

	argv[2] = pwbuf;
	argv[3] = path;
	argv[4] = NULL;

	uzpi.msgfn = UzpMessageNull;
	uzpi.structlen = ; //TODO

	if (UzpAltMain(4, argv, &uzpi) == EXIT_SUCCESS)
		return 0;

	return 1;
}

int
header_test(char *password, unsigned char *header, unsigned char chkbyte)
{
	unsigned char dec_header[header_len];

	decrypt(password, header, header_len, dec_header);

	return dec_header[11] == chkbyte;
}
