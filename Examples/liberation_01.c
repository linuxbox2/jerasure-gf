/* Examples/liberation_01.c
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
#include "liberation.h"
#include "examples.h"

usage(char *s)
{
  fprintf(stderr, "usage: liberation_01 k w - Liberation RAID-6 coding/decoding example in GF(2^w).\n");
  fprintf(stderr, "       \n");
  fprintf(stderr, "       w must be prime and k <= w.  It sets up a Liberation bit-matrix\n");
  fprintf(stderr, "       then it encodes k devices of w*%d bytes using dumb bit-matrix scheduling.\n", sizeof(gdata));
  fprintf(stderr, "       It decodes using smart bit-matrix scheduling.\n");
  fprintf(stderr, "       \n");
  fprintf(stderr, "This demonstrates: liberation_coding_bitmatrix()\n");
  fprintf(stderr, "                   jerasure_smart_bitmatrix_to_schedule()\n");
  fprintf(stderr, "                   jerasure_dumb_bitmatrix_to_schedule()\n");
  fprintf(stderr, "                   jerasure_schedule_encode()\n");
  fprintf(stderr, "                   jerasure_schedule_decode_lazy()\n");
  fprintf(stderr, "                   jerasure_print_bitmatrix()\n");
  fprintf(stderr, "                   jerasure_get_stats()\n");
  if (s != NULL) fprintf(stderr, "%s\n", s);
  exit(1);
}

int main(int argc, char **argv)
{
  gdata l;
  int k, w, i, j, m;
  int *bitmatrix;
  char **data, **coding, **ptrs;
  int **dumb;
  int *erasures, *erased;
  double stats[3];
  
  if (argc != 3) usage(NULL);
  if (sscanf(argv[1], "%d", &k) == 0 || k <= 0) usage("Bad k");
  if (sscanf(argv[2], "%d", &w) == 0 || w <= 0 || w > 32) usage("Bad w");
  m = 2;
  if (w < k) usage("k is too big");

  bitmatrix = liberation_coding_bitmatrix(k, w);
  if (bitmatrix == NULL) {
    usage("couldn't make coding matrix");
  }

  printf("Coding Bit-Matrix:\n\n");
  jerasure_print_bitmatrix(bitmatrix, w*m, w*k, w);
  printf("\n");

  dumb = jerasure_dumb_bitmatrix_to_schedule(k, m, w, bitmatrix);

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
  
  return 0;
}
