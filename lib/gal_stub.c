#include <string.h>
#include "gal_stub.h"

static int gal_multiply_w32(gf_t *gf, int a, int b)
{
	return galois_single_multiply(a, b, gf->w);
}

static int gal_divide_w32(gf_t *gf, int a, int b)
{
	return galois_single_divide(a, b, gf->w);
}

static struct _op_single gal_op_multiply = {
	gal_multiply_w32,
};

static struct _op_single gal_op_divide = {
	gal_divide_w32,
};

void gf_init_easy(gf_t *gf, int w)
{
	gf->w = w;
	memcpy(&gf->multiply, &gal_op_multiply, sizeof gal_op_multiply);
	memcpy(&gf->divide, &gal_op_divide, sizeof gal_op_divide);
}

void gf_multby_one(void *src, void *dest, unsigned bytes, int xor)
{
	if (xor) {
		galois_region_xor(src, dest, dest, bytes);
	} else {
		memcpy(dest, src, bytes);
	}
}
