#include "passgen.h"

#include <stdlib.h>
#include <string.h>

//TODO: rewrite testing stub
unsigned cur_pass = 0;
char *passwords[10] = {
	"1111"
	, "qqqq"
	, "1234"
	, "5678"
	, "super"
	, "zxcvv"
	, "jojojo"
	, "0000"
	, "mnmnmn"
	, "zzzzzz"
};

void
passgen_init()
{
	return;
}

char *
passgen_get(void)
{
	if (cur_pass >= 10)
		return NULL; // error
	char *pass = malloc(10 * sizeof(char));
	strcpy(pass, passwords[cur_pass]);
	++cur_pass;
	return pass;
}
