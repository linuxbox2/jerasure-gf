
/* This uses procedures from the Galois Field arithmetic library */

#ifndef _H_JERASURE
#define _H_JERASURE 1
#ifdef WITH_GF
#include "gf_complete.h"
typedef unsigned int gdata_u;
typedef int gdata;
#else
typedef unsigned long gdata_u;
typedef long gdata;
#include "galois.h"
#include "gal_stub.h"
#endif
struct jerasure_context {
	int w;
	gf_t gf[1];
};
#endif /* _H_JERASURE */
