#ifndef H__ZIP
#define H__ZIP

#include <stdio.h>

struct file_brute {
	unsigned crc32;
	unsigned char enc_header[12];
};

struct file_brute *zip_parse(FILE *zipfile, unsigned *files_count);

#endif
