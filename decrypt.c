#include "decrypt.h"

#include "crc32.h"

const int init_keys[] = {
	305419896, 591751049, 878082192
};

void
update_keys(unsigned keys[], unsigned char c)
{
	keys[0]  = crc32(keys[0], c);
	keys[1]  = (keys[1] + (unsigned char)keys[0]) * 134775813 + 1;
	keys[2]  = crc32(keys[2], keys[1] >> 24);
}

unsigned char
decrypt_byte(unsigned key2)
{
	unsigned short tmp = (unsigned short)key2 | 2;
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
		update_keys(keys, data[i]);
	}
	dec_data[i] = data[i] ^ decrypt_byte(keys[2]);
}
