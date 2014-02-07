/* Examples/jerasure_06.c
Jerasure - A C/C++ Library for a Variety of Reed-Solomon and RAID-6 Erasure Coding Techniques

Revision 1.2A
May 24, 2011

James S. Plank
Department of Electrical Engineering and Computer Science
University of Tennessee
Knoxville, TN 37996
plank@cs.utk.edu

Copyright (c) 2011, James S. Plank
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

 - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.

 - Neither the name of the University of Tennessee nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.


 */
    
/*
	revised by S. Simmerman
	2/25/08  
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jerasure.h"
#include "examples.h"

usage(char *s)
{
  fprintf(stderr, "usage: jerasure_06 k m w packetsize\n");
  fprintf(stderr, "Does a simple Cauchy Reed-Solomon coding example in GF(2^w).\n");
  fprintf(stderr, "       \n");
  fprintf(stderr, "       k+m must be < 2^w.  Packetsize must be a multiple of sizeof(gdata)\n");
  fprintf(stderr, "       It sets up a Cauchy distribution matrix and encodes k devices of w*packetsize bytes.\n");
  fprintf(stderr, "       After that, it decodes device 0 by using jerasure_make_decoding_bitmatrix()\n");
  fprintf(stderr, "       and jerasure_bitmatrix_dotprod().\n");
  fprintf(stderr, "       \n");
  fprintf(stderr, "This demonstrates: jerasure_bitmatrix_encode()\n");
  fprintf(stderr, "                   jerasure_bitmatrix_decode()\n");
  fprintf(stderr, "                   jerasure_print_bitmatrix()\n");
  fprintf(stderr, "                   jerasure_make_decoding_bitmatrix()\n");
  fprintf(stderr, "                   jerasure_bitmatrix_dotprod()\n");
  if (s != NULL) fprintf(stderr, "\n%s\n\n", s);
  exit(1);
}

int main(int argc, char **argv)
{
  gdata l;
  int k, w, i, j, m, psize, x;
  int *matrix, *bitmatrix;
  char **data, **coding;
  int *erasures, *erased;
  int *decoding_matrix, *dm_ids;
  struct jerasure_context *ctx;
  
  if (argc != 5) usage(NULL);
  if (sscanf(argv[1], "%d", &k) == 0 || k <= 0) usage("Bad k");
  if (sscanf(argv[2], "%d", &m) == 0 || m <= 0) usage("Bad m");
  if (sscanf(argv[3], "%d", &w) == 0 || w <= 0 || w > 32) usage("Bad w");
  if (w < 30 && (k+m) > (1 << w)) usage("k + m is too big");
  if (sscanf(argv[4], "%d", &psize) == 0 || psize <= 0) usage("Bad packetsize");
  if(psize%sizeof(gdata) != 0) usage("Packetsize must be multiple of sizeof(gdata)");

  ctx = jerasure_make_context(w);
  matrix = talloc(int, m*k);
  for (i = 0; i < m; i++) {
    for (j = 0; j < k; j++) {
      matrix[i*k+j] = galois_single_divide(1, i ^ (m + j), w);
    }
  }
  bitmatrix = jerasure_matrix_to_bitmatrix(ctx, k, m, matrix);

  printf("Last (m * w) rows of the Binary Distribution Matrix:\n\n");
  jerasure_print_bitmatrix(bitmatrix, w*m, w*k, w);
  printf("\n");

  srand48(0);
  data = talloc(char *, k);
  for (i = 0; i < k; i++) {
    data[i] = talloc(char, psize*w);
    fillrand(data[i], psize*w);
  }

  coding = talloc(char *, m);
  for (i = 0; i < m; i++) {
    coding[i] = talloc(char, psize*w);
  }

  jerasure_bitmatrix_encode(k, m, w, bitmatrix, data, coding, w*psize, psize);
  
  printf("Encoding Complete:\n\n");
  print_data_and_coding_2(k, m, w, psize, data, coding);

  erasures = talloc(int, (m+1));
  erased = talloc(int, (k+m));
  for (i = 0; i < m+k; i++) erased[i] = 0;
  for (i = 0; i < m; ) {
    erasures[i] = lrand48()%(k+m);
    if (erased[erasures[i]] == 0) {
      erased[erasures[i]] = 1;
      memset((erasures[i] < k) ? data[erasures[i]] : coding[erasures[i]-k], 0, psize*w);
      i++;
    }
  }
  erasures[i] = -1;

  printf("Erased %d random devices:\n\n", m);
  print_data_and_coding_2(k, m, w, psize, data, coding);
  
  i = jerasure_bitmatrix_decode(k, m, w, bitmatrix, 0, erasures, data, coding, 
		  w*psize, psize);

  printf("State of the system after decoding:\n\n");
  print_data_and_coding_2(k, m, w, psize, data, coding);
  
  decoding_matrix = talloc(int, k*k*w*w);
  dm_ids = talloc(int, k);

  for (i = 0; i < m; i++) erased[i] = 1;
  for (; i < k+m; i++) erased[i] = 0;

  jerasure_make_decoding_bitmatrix(k, m, w, bitmatrix, erased, decoding_matrix, dm_ids);

  printf("Suppose we erase the first %d devices.  Here is the decoding matrix:\n\n", m);
  jerasure_print_bitmatrix(decoding_matrix, k*w, k*w, w);
  printf("\n");
  printf("And dm_ids:\n\n");
  jerasure_print_matrix(dm_ids, 1, k, w);

  //memcpy(&l, data[0], sizeof(gdata));
  //printf("\nThe value of device #%d, word 0 is: %lx\n", 0, l);
  memset(data[0], 0, w*psize);
  jerasure_bitmatrix_dotprod(k, w, decoding_matrix, dm_ids, 0, data, coding, w*psize, psize);

  printf("\nAfter calling jerasure_matrix_dotprod, we calculate the value of device #0, packet 0 to be:\n");
	printf("\nD0  p0 :");
	for(i = 0; i < psize; i +=sizeof(gdata)) {
		memcpy(&l, data[0]+i, sizeof(gdata));
		printf(" %08lx", l);
	}
	printf("\n\n");
  //memcpy(&l, data[0], sizeof(gdata));
  /* free data to avoid false positives for leak testing */
  free(dm_ids);
  free(decoding_matrix);
  free(erased);
  free(erasures);
  for (i = 0; i < m; i++) {
    free(coding[i]);
  }
  free(coding);
  for (i = 0; i < k; i++) {
    free(data[i]);
  }
  free(data);
  free(bitmatrix);
  free(matrix);
  jerasure_release_context(ctx);

  return 0;
}
