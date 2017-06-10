#ifndef H__PASSWD_TEST
#define H__PASSWD_TEST

#include <stddef.h>

int unzip_test(char *path, char *password);
int header_test(char *password, unsigned char *header, unsigned char chkbyte);

#endif
