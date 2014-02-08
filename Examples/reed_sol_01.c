/* Examples/reed_sol_01.c
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
#include "reed_sol.h"
#include "examples.h"

usage(char *s)
{
  fprintf(stderr, "usage: reed_sol_01 k m w - Does a simple Reed-Solomon coding example in GF(2^w).\n");
  fprintf(stderr, "       \n");
  fprintf(stderr, "       w must be 8, 16 or 32.  k+m must be <= 2^w.  It sets up a classic\n");
  fprintf(stderr, "       Vandermonde-based distribution matrix and encodes k devices of\n");
  fprintf(stderr, "       %d bytes each with it.  Then it decodes.\n", sizeof(gdata));
  fprintf(stderr, "       \n");
  fprintf(stderr, "This demonstrates: jerasure_matrix_encode()\n");
  fprintf(stderr, "                   jerasure_matrix_decode()\n");
  fprintf(stderr, "                   jerasure_print_matrix()\n");
  fprintf(stderr, "                   reed_sol_vandermonde_coding_matrix()\n");
  if (s != NULL) fprintf(stderr, "%s\n", s);
  exit(1);
}

int main(int argc, char **argv)
{
  gdata l;
  int k, w, i, j, m;
  int *matrix;
  char **data, **coding;
  int *erasures, *erased;
  int *decoding_matrix, *dm_ids;
  struct jerasure_context *ctx;
  
  if (argc != 4) usage(NULL);
  if (sscanf(argv[1], "%d", &k) == 0 || k <= 0) usage("Bad k");
  if (sscanf(argv[2], "%d", &m) == 0 || m <= 0) usage("Bad m");
  if (sscanf(argv[3], "%d", &w) == 0 || (w != 8 && w != 16 && w != 32)) usage("Bad w");
  if (w <= 16 && k + m > (1 << w)) usage("k + m is too big");

  ctx = jerasure_make_context(w);
  matrix = reed_sol_vandermonde_coding_matrix(k, m, w);

  printf("Last m rows of the Distribution Matrix:\n\n");
  jerasure_print_matrix(matrix, m, k, w);
  printf("\n");

  srand48(0);
  data = talloc(char *, k);
  for (i = 0; i < k; i++) {
    data[i] = talloc(char, sizeof(gdata));
    fillrand(data[i], sizeof(gdata));
  }

  coding = talloc(char *, m);
  for (i = 0; i < m; i++) {
    coding[i] = talloc(char, sizeof(gdata));
  }

  jerasure_matrix_encode(k, m, w, matrix, data, coding, sizeof(gdata));
  
  printf("Encoding Complete:\n\n");
  print_data_and_coding_1(k, m, w, sizeof(gdata), data, coding);

  erasures = talloc(int, (m+1));
  erased = talloc(int, (k+m));
  for (i = 0; i < m+k; i++) erased[i] = 0;
  l = 0;
  for (i = 0; i < m; ) {
    erasures[i] = lrand48()%(k+m);
    if (erased[erasures[i]] == 0) {
      erased[erasures[i]] = 1;
      memcpy((erasures[i] < k) ? data[erasures[i]] : coding[erasures[i]-k], &l, sizeof(gdata));
      i++;
    }
  }
  erasures[i] = -1;

  printf("Erased %d random devices:\n\n", m);
  print_data_and_coding_1(k, m, w, sizeof(gdata), data, coding);
  
  i = jerasure_matrix_decode(ctx, k, m, matrix, 1, erasures, data, coding, sizeof(gdata));

  printf("State of the system after decoding:\n\n");
  print_data_and_coding_1(k, m, w, sizeof(gdata), data, coding);
  /* free data to avoid false positives for leak testing */
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
  free(matrix);
  jerasure_release_context(ctx);

  return 0;
}
