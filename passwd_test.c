#include "passwd_test.h"

#include <string.h>
#include "decrypt.h"
#include "debug.h"

const int header_len = 12;

int
is_correct_passwd(char *password, unsigned char *header, unsigned crc)
{
	unsigned char dec_header[header_len];

	decrypt(password, header, header_len, dec_header);

	DEBUG(printf("DEBUG: header last byte=%hhu\tcrc last byte=%hhu\n", dec_header[11], (unsigned char)(crc)));
	return dec_header[11] == (unsigned char)(crc & 0xff);
}
