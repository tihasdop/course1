#ifndef H__CRC32
#define H__CRC32

#include <stddef.h>

unsigned crc32__(unsigned old_crc, unsigned char c);
unsigned crc32_arr(const unsigned char * buf, size_t len);

#endif
