#ifndef H__PASSWD_TEST
#define H__PASSWD_TEST

#include <stddef.h>

int is_correct_passwd(char *password, unsigned char *header, unsigned crc);

#endif
