#ifndef _H_GAL_STUB
#define _H_GAL_STUB 1
typedef struct galois_stub gf_t;
struct galois_stub {
	int w;
	struct _op_single {
		int (*w32)(gf_t*, int, int);
		int (*w64)(gf_t*, int, int);
	} multiply, divide;
	struct _op_region {
		void (*w32)(gf_t*, void *, void *, int, int, int);
		void (*w64)(gf_t*, void *, void *, int, int, int);
	} multiply_region;
};
void gf_multby_one(void *, void *, unsigned, int);
#endif /* _H_GAL_STUB */
