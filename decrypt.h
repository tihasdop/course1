#ifndef H__DECRYPT
#define H__DECRYPT

#include <stddef.h>

void decrypt(char *password, unsigned char data[], size_t data_len, unsigned char dec_data[]);

#endif
