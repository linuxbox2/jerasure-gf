/* Examples/cauchy_02.c
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
#include "cauchy.h"
#include "examples.h"

usage(char *s)
{
  fprintf(stderr, "usage: cauchy_02 k m w - Scheduled CRS coding example with the original matrix in  GF(2^w).\n");
  fprintf(stderr, "       \n");
  fprintf(stderr, "       k+m must be <= 2^w.  It sets up a Cauchy distribution matrix using the original\n");
  fprintf(stderr, "       Cauchy Distribution matrix construction algorithm, then it encodes\n");
  fprintf(stderr, "       k devices of w*%d bytes using smart bit-matrix scheduling.\n", sizeof(gdata));
  fprintf(stderr, "       It decodes using bit-matrix scheduling as well.\n");
  fprintf(stderr, "       \n");
  fprintf(stderr, "This demonstrates: cauchy_original_coding_matrix()\n");
  fprintf(stderr, "                   cauchy_xy_coding_matrix()\n");
  fprintf(stderr, "                   cauchy_n_ones()\n");
  fprintf(stderr, "                   jerasure_smart_bitmatrix_to_schedule()\n");
  fprintf(stderr, "                   jerasure_schedule_encode()\n");
  fprintf(stderr, "                   jerasure_schedule_decode_lazy()\n");
  fprintf(stderr, "                   jerasure_print_matrix()\n");
  fprintf(stderr, "                   jerasure_get_stats()\n");
  if (s != NULL) fprintf(stderr, "%s\n", s);
  exit(1);
}

int main(int argc, char **argv)
{
  gdata l;
  int k, w, i, j, m;
  int *matrix, *bitmatrix, *m2, *x, *y;
  char **data, **coding, **ptrs;
  int **smart;
  int no;
  int *erasures, *erased;
  double stats[3];
  struct jerasure_context *ctx;
  
  if (argc != 4) usage(NULL);
  if (sscanf(argv[1], "%d", &k) == 0 || k <= 0) usage("Bad k");
  if (sscanf(argv[2], "%d", &m) == 0 || m <= 0) usage("Bad m");
  if (sscanf(argv[3], "%d", &w) == 0 || w <= 0 || w > 32) usage("Bad w");
  if (w < 30 && (k+m) > (1 << w)) usage("k + m is too big");

  ctx = jerasure_make_context(w);
  matrix = cauchy_original_coding_matrix(ctx, k, m);
  if (matrix == NULL) {
    usage("couldn't make coding matrix");
  }

  no = 0;
  for (i = 0; i < k*m; i++) {
    no += cauchy_n_ones(ctx, matrix[i]);
  }
  printf("Matrix has %d ones\n\n", no);
  jerasure_print_matrix(matrix, m, k, w);
  printf("\n", no);
  bitmatrix = jerasure_matrix_to_bitmatrix(ctx, k, m, matrix);

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
  
  x = talloc(int, m);
  y = talloc(int, k);
  if (x == NULL || y == NULL) { perror("malloc"); exit(1); }
  for (i = 0; i < m; i++) x[i] = i;
  for (i = 0; i < k; i++) y[i] = m+i;
  m2 = cauchy_xy_coding_matrix(ctx, k, m, x, y);
  if (memcmp(matrix, m2, sizeof(int)*k*m) != 0) {
    printf("Error -- the matrices made by original and xy don't match\n");
    exit(1);
  } else {
    printf("Generated the identical matrix using cauchy_xy_coding_matrix()\n");
  }
  /* free data to avoid false positives for leak testing */
  free(m2);
  free(y);
  free(x);
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
  free(bitmatrix);
  free(matrix);
  jerasure_release_context(ctx);

  return 0;
}
