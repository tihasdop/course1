#ifndef H__DEBUG
#define H__DEBUG

#include <stdio.h>

//#define DEBUG_OFF

#ifdef DEBUG_OFF
#define DEBUG(P) \
	do {} while (0)
#else
#define DEBUG(P) \
	do { \
		P; \
	} while (0)
#endif

#endif
