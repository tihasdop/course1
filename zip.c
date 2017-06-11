/*
zip.c -- is a part of the ziphack

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

#include "zip.h"

#include <stdlib.h>

#include "debug.h"

const int eocd_sig = 0x06054b50;
const unsigned file_entries_count_max = 5;

void
fread_m(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t res = fread(ptr, size, nmemb, stream);
	if (res != nmemb) {
		printf("Error while reading! res=%lu nmemb=%lu ftell=%li\n", res, nmemb, ftell(stream));
		exit(1);
	}
}

unsigned
get_cd_offset(FILE *zipfile)
{
	// 22 is a minimal size of EOCD (End of central directory record)
	fseek(zipfile, -22, SEEK_END);

	unsigned tmp;
	fread_m(&tmp, sizeof(tmp), 1, zipfile);
	while (tmp != eocd_sig) {
		fseek(zipfile, - sizeof(tmp) - 1L, SEEK_CUR);
		fread_m(&tmp, sizeof(tmp), 1, zipfile);
	}

	// seek for 'Offset of start of central directory'
	fseek(zipfile, 16 - sizeof(tmp), SEEK_CUR);
	fread_m(&tmp, sizeof(tmp), 1, zipfile);

	return tmp;
}

unsigned *
get_file_offsets(FILE *zipfile, unsigned cd_offset, unsigned *files_count)
{
	unsigned cur_file = 0;
	unsigned tmp, len2next;
	unsigned short tmps;

	// seek for start of central directory (after signature)
	fseek(zipfile, cd_offset + sizeof(tmp), SEEK_SET);

	unsigned *files = malloc(sizeof(unsigned) * file_entries_count_max);

	do {
		if (cur_file >= file_entries_count_max) {
			break; // if max files count reached
		}

		// seek for 'file name length'
		fseek(zipfile, 28 - 4, SEEK_CUR);
		// read 'file name length'
		fread_m(&tmps, sizeof(tmps), 1, zipfile);
		len2next  = tmps;
		// read 'extra field length'
		fread_m(&tmps, sizeof(tmps), 1, zipfile);
		len2next += tmps;
		// read 'file comment length'
		fread_m(&tmps, sizeof(tmps), 1, zipfile);
		len2next += tmps;

		// seek for 'offset of local file header'
		fseek(zipfile, 42 - 34, SEEK_CUR);
		// read 'offset of local file header'
		fread_m(&(files[cur_file]), sizeof(files[0]), 1, zipfile);

		// seek for next file entry
		fseek(zipfile, len2next, SEEK_CUR);

		fread_m(&tmp, sizeof(tmp), 1, zipfile);
		DEBUG(printf("DEBUG: cur_file=%u\toffset=%u\n", cur_file, files[cur_file]));

		++cur_file;
	} while (tmp != eocd_sig);

	*files_count = cur_file;
	return files;
}

void
get_encryption_header(FILE *zipfile, unsigned file_offset, struct file_brute *file)
{
	unsigned len2header;
	unsigned short tmp, flag;

	// seek for 'general purpose bit flag'
	fseek(zipfile, file_offset + 6, SEEK_SET);
	fread_m(&flag, sizeof(flag), 1, zipfile);

	if (flag & 0x08) {
		// extend header format
		// chkbyte from 'file last modification time'
		fseek(zipfile, 10 - 8, SEEK_CUR);
		fread_m(&tmp, sizeof(tmp), 1, zipfile);
		file->chkbyte = tmp >> 8;
		fseek(zipfile, 26 - 12, SEEK_CUR);
	} else {
		// chkbyte from 'crc-32'
		unsigned crc;
		fseek(zipfile, 14 - 8, SEEK_CUR);
		fread_m(&crc, sizeof(crc), 1, zipfile);
		file->chkbyte = crc >> 24;
		fseek(zipfile, 26 - 18, SEEK_CUR);
	}

	// read 'file name length'
	fread_m(&tmp, sizeof(tmp), 1, zipfile);
	len2header  = tmp;
	// read 'extra field length'
	fread_m(&tmp, sizeof(tmp), 1, zipfile);
	len2header += tmp;

	// seek for encryption header
	fseek(zipfile, len2header, SEEK_CUR);
	fread_m(file->enc_header, sizeof(file->enc_header[0]), 12, zipfile);
}

struct file_brute *
zip_parse(FILE *zipfile, unsigned *files_count)
{
	unsigned cd_offset = get_cd_offset(zipfile);

	unsigned *file_offsets = get_file_offsets(zipfile, cd_offset, files_count);

	DEBUG(printf("DEBUG: files_count=%u\n", *files_count));

	struct file_brute *files = malloc(sizeof(struct file_brute) * *files_count);
	for (unsigned i = 0; i < *files_count; ++i) {
		get_encryption_header(zipfile, file_offsets[i], &files[i]);
	}

	free(file_offsets);
	return files;
}
