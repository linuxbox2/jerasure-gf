#include <string.h>
#include <stdlib.h>
#include "gal_stub.h"

static int gal_multiply_w32(gf_t *gf, int a, int b)
{
	return galois_single_multiply(a, b, gf->w);
}

static int gal_divide_w32(gf_t *gf, int a, int b)
{
	return galois_single_divide(a, b, gf->w);
}

static void gal_mult_region_w32(gf_t * gf, void *src, void *dest,
	int value, int bytes, int xor)
{
	switch(gf->w) {
	case 8:
		galois_w08_region_multiply(src,value,bytes,dest,xor);
		break;
	case 16:
		galois_w16_region_multiply(src,value,bytes,dest,xor);
		break;
	case 32:
		galois_w32_region_multiply(src,value,bytes,dest,xor);
		break;
	default:
		abort();
	}
}

static struct _op_single gal_op_multiply = {
	gal_multiply_w32,
};

static struct _op_single gal_op_divide = {
	gal_divide_w32,
};

static struct _op_region gal_op_multiply_region = {
	gal_mult_region_w32,
};

void gf_init_easy(gf_t *gf, int w)
{
	gf->w = w;
	memcpy(&gf->multiply, &gal_op_multiply, sizeof gal_op_multiply);
	memcpy(&gf->divide, &gal_op_divide, sizeof gal_op_divide);
	memcpy(&gf->multiply_region, &gal_op_multiply_region, sizeof gal_op_multiply_region);
}

void gf_multby_one(void *src, void *dest, unsigned bytes, int xor)
{
	if (xor) {
		galois_region_xor(src, dest, dest, bytes);
	} else {
		memcpy(dest, src, bytes);
	}
}
