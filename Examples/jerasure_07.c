/* Examples/jerasure_07.c
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
  fprintf(stderr, "usage: jerasure_07 k m w - Scheduled Cauchy Reed-Solomon coding example in GF(2^w).\n");
  fprintf(stderr, "       \n");
  fprintf(stderr, "       k+m must be <= 2^w.  It sets up a Cauchy distribution matrix and encodes\n");
  fprintf(stderr, "       k sets of w*%d bytes. It uses bit-matrix scheduling, both smart and dumb.\n", sizeof(gdata));
  fprintf(stderr, "       It decodes using bit-matrix scheduling, then shows an example of\n");
  fprintf(stderr, "       using jerasure_do_scheduled_operations().\n");
  fprintf(stderr, "       \n");
  fprintf(stderr, "This demonstrates: jerasure_dumb_bitmatrix_to_schedule()\n");
  fprintf(stderr, "                   jerasure_smart_bitmatrix_to_schedule()\n");
  fprintf(stderr, "                   jerasure_schedule_encode()\n");
  fprintf(stderr, "                   jerasure_schedule_decode_lazy()\n");
  fprintf(stderr, "                   jerasure_do_scheduled_operations()\n");
  fprintf(stderr, "                   jerasure_get_stats()\n");
  if (s != NULL) fprintf(stderr, "%s\n", s);
  exit(1);
}

int main(int argc, char **argv)
{
  gdata l;
  int k, w, i, j, m;
  int *matrix, *bitmatrix;
  char **data, **coding, **ptrs;
  int **smart, **dumb;
  int *erasures, *erased;
  double stats[3];
  struct jerasure_context *ctx;
  
  if (argc != 4) usage(NULL);
  if (sscanf(argv[1], "%d", &k) == 0 || k <= 0) usage("Bad k");
  if (sscanf(argv[2], "%d", &m) == 0 || m <= 0) usage("Bad m");
  if (sscanf(argv[3], "%d", &w) == 0 || w <= 0 || w > 32) usage("Bad m");
  if (w < 30 && (k+m) > (1 << w)) usage("k + m is too big");

  ctx = jerasure_make_context(w);
  matrix = talloc(int, m*k);
  for (i = 0; i < m; i++) {
    for (j = 0; j < k; j++) {
      matrix[i*k+j] = ctx->gf->divide.w32(ctx->gf, 1, i ^ (m + j));
    }
  }
  bitmatrix = jerasure_matrix_to_bitmatrix(ctx, k, m, matrix);

  printf("Last m rows of the Binary Distribution Matrix:\n\n");
  jerasure_print_bitmatrix(bitmatrix, w*m, w*k, w);
  printf("\n");
  
  dumb = jerasure_dumb_bitmatrix_to_schedule(k, m, w, bitmatrix);
  smart = jerasure_smart_bitmatrix_to_schedule(k, m, w, bitmatrix);

  srand48(0);
  data = talloc(char *, k);
  for (i = 0; i < k; i++) {
    data[i] = talloc(char, sizeof(gdata)*w);
    fillrand(data[i], sizeof(gdata)*w);
  }

  coding = talloc(char *, m);
  for (i = 0; i < m; i++) {
    coding[i] = talloc(char, sizeof(gdata)*w);
  }

  jerasure_schedule_encode(k, m, w, dumb, data, coding, w*sizeof(gdata), sizeof(gdata));
  jerasure_get_stats(stats);
  printf("Dumb Encoding Complete: - %.0lf XOR'd bytes\n\n", stats[0]);
  print_data_and_coding_2(k, m, w, sizeof(gdata), data, coding);

  jerasure_schedule_encode(k, m, w, smart, data, coding, w*sizeof(gdata), sizeof(gdata));
  jerasure_get_stats(stats);
  printf("Smart Encoding Complete: - %.0lf XOR'd bytes\n\n", stats[0]);
  print_data_and_coding_2(k, m, w, sizeof(gdata), data, coding);

  erasures = talloc(int, (m+1));
  erased = talloc(int, (k+m));
  for (i = 0; i < m+k; i++) erased[i] = 0;
  for (i = 0; i < m; ) {
    erasures[i] = lrand48()%(k+m);
    if (erased[erasures[i]] == 0) {
      erased[erasures[i]] = 1;
      memset((erasures[i] < k) ? data[erasures[i]] : coding[erasures[i]-k], 0, sizeof(gdata)*w);
      i++;
    }
  }
  erasures[i] = -1;

  printf("Erased %d random devices:\n\n", m);
  print_data_and_coding_2(k, m, w, sizeof(gdata), data, coding);
  
  jerasure_schedule_decode_lazy(k, m, w, bitmatrix, erasures, data, coding, w*sizeof(gdata), sizeof(gdata), 1);
  jerasure_get_stats(stats);

  printf("State of the system after decoding: %.0lf XOR'd bytes\n\n", stats[0]);
  print_data_and_coding_2(k, m, w, sizeof(gdata), data, coding);
  
  ptrs = talloc(char *, (k+m));
  for (i = 0; i < k; i++) ptrs[i] = data[i];
  for (i = 0; i < m; i++) ptrs[k+i] = coding[i];

  for (j = 0; j < m; j++) memset(coding[j], 0, sizeof(gdata)*w);
  jerasure_do_scheduled_operations(ptrs, smart, sizeof(gdata));
  printf("State of the system after deleting the coding devices and\n");
  printf("using jerasure_do_scheduled_operations(): %.0lf XOR'd bytes\n\n", stats[0]);
  print_data_and_coding_2(k, m, w, sizeof(gdata), data, coding);
  /* free data to avoid false positives for leak testing */
  free(ptrs);
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
  jerasure_free_schedule(smart);
  jerasure_free_schedule(dumb);
  free(bitmatrix);
  free(matrix);
  jerasure_release_context(ctx);

  return 0;
}
