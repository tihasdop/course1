/*
decrypt.c -- is a part of the ziphack

Copyright (c) 2017 Alexander Tomilov

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "decrypt.h"

#include "crc32.h"

const unsigned init_keys[] = {
	305419896, 591751049, 878082192
};

void
update_keys(unsigned *keys, unsigned char c)
{
	keys[0]  = crc32__(keys[0], c);
	keys[1]  = (keys[1] + (unsigned char)keys[0]) * 134775813 + 1;
	keys[2]  = crc32__(keys[2], keys[1] >> 24);
}

unsigned char
decrypt_byte(unsigned key2)
{
	unsigned short tmp = (key2) | 2;
	return (unsigned char)((tmp * (tmp ^ 1)) >> 8);
}

void
decrypt(char *password, unsigned char data[], size_t data_len, unsigned char dec_data[])
{
	unsigned keys[3];
	keys[0] = init_keys[0];
	keys[1] = init_keys[1];
	keys[2] = init_keys[2];

	char c;
	while ((c = *password++)) {
		update_keys(keys, c);
	}

	unsigned i;
	for (i = 0; i < (data_len - 1); ++i) {
		dec_data[i] = data[i] ^ decrypt_byte(keys[2]);
		update_keys(keys, dec_data[i]);
	}
	dec_data[i] = data[i] ^ decrypt_byte(keys[2]);
}
