/* Examples/cauchy_04.c
 * James S. Plank

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

#define talloc(type, num) (type *) malloc(sizeof(type)*(num))

usage(char *s)
{
  fprintf(stderr, "usage: cauchy_04 k m w - Scheduled CRS coding example with a good matrix in GF(2^w).\n");
  fprintf(stderr, "       \n");
  fprintf(stderr, "       k+m must be <= 2^w.  It sets up a Cauchy distribution matrix using \n");
  fprintf(stderr, "       cauchy_good_coding_matrix(), then it encodes\n");
  fprintf(stderr, "       k devices of w*%d bytes using smart bit-matrix scheduling.\n", sizeof(int32));
  fprintf(stderr, "       It decodes using bit-matrix scheduling as well.\n");
  fprintf(stderr, "       \n");
  fprintf(stderr, "This demonstrates: cauchy_original_coding_matrix()\n");
  fprintf(stderr, "                   cauchy_n_ones()\n");
  fprintf(stderr, "                   jerasure_smart_bitmatrix_to_schedule()\n");
  fprintf(stderr, "                   jerasure_schedule_encode()\n");
  fprintf(stderr, "                   jerasure_schedule_decode_lazy()\n");
  fprintf(stderr, "                   jerasure_print_matrix()\n");
  fprintf(stderr, "                   jerasure_get_stats()\n");
  if (s != NULL) fprintf(stderr, "%s\n", s);
  exit(1);
}

static void print_data_and_coding(int k, int m, int w, int psize, 
		char **data, char **coding) 
{
	int i, j, x, n, sp;
	int32 l;

	if(k > m) n = k;
	else n = m;
	sp = psize * 2 + (psize/4) + 12;

	printf("%-*sCoding\n", sp, "Data");
	for(i = 0; i < n; i++) {
		for (j = 0; j < w; j++) {
			if(i < k) {

				if(j==0) printf("D%-2d p%-2d:", i,j);
				else printf("    p%-2d:", j);
				for(x = 0; x < psize; x +=4) {
					memcpy(&l, data[i]+j*psize+x, sizeof(int32));
					printf(" %08lx", l);
				}
				printf("    ");
			}
			else printf("%*s", sp, "");
			if(i < m) {
				if(j==0) printf("C%-2d p%-2d:", i,j);
				else printf("    p%-2d:", j);
				for(x = 0; x < psize; x +=4) {
					memcpy(&l, coding[i]+j*psize+x, sizeof(int32));
					printf(" %08lx", l);
				}
			}
			printf("\n");
		}
	}

    printf("\n");
}

int main(int argc, char **argv)
{
  int32 l;
  int k, w, i, j, m;
  int *matrix, *bitmatrix;
  char **data, **coding, **ptrs;
  int **smart;
  int no;
  int *erasures, *erased;
  double stats[3];
  
  if (argc != 4) usage(NULL);
  if (sscanf(argv[1], "%d", &k) == 0 || k <= 0) usage("Bad k");
  if (sscanf(argv[2], "%d", &m) == 0 || m <= 0) usage("Bad m");
  if (sscanf(argv[3], "%d", &w) == 0 || w <= 0 || w > 32) usage("Bad w");
  if (w < 30 && (k+m) > (1 << w)) usage("k + m is too big");

  matrix = cauchy_good_general_coding_matrix(k, m, w);
  if (matrix == NULL) {
    usage("couldn't make coding matrix");
  }

  no = 0;
  for (i = 0; i < k*m; i++) {
    no += cauchy_n_ones(matrix[i], w);
  }
  printf("Matrix has %d ones\n\n", no);
  jerasure_print_matrix(matrix, m, k, w);
  printf("\n");
  bitmatrix = jerasure_matrix_to_bitmatrix(k, m, w, matrix);

  smart = jerasure_smart_bitmatrix_to_schedule(k, m, w, bitmatrix);

  srand48(0);
  data = talloc(char *, k);
  for (i = 0; i < k; i++) {
    data[i] = talloc(char, sizeof(int32)*w);
    for (j = 0; j < w; j++) {
      l = lrand48();
      memcpy(data[i]+j*sizeof(int32), &l, sizeof(int32));
    }
  }

  coding = talloc(char *, m);
  for (i = 0; i < m; i++) {
    coding[i] = talloc(char, sizeof(int32)*w);
  }

  jerasure_schedule_encode(k, m, w, smart, data, coding, w*sizeof(int32), sizeof(int32));
  jerasure_get_stats(stats);
  printf("Smart Encoding Complete: - %.0lf XOR'd bytes\n\n", stats[0]);
  print_data_and_coding(k, m, w, sizeof(int32), data, coding);

  erasures = talloc(int, (m+1));
  erased = talloc(int, (k+m));
  for (i = 0; i < m+k; i++) erased[i] = 0;
  for (i = 0; i < m; ) {
    erasures[i] = lrand48()%(k+m);
    if (erased[erasures[i]] == 0) {
      erased[erasures[i]] = 1;
      bzero((erasures[i] < k) ? data[erasures[i]] : coding[erasures[i]-k], sizeof(int32)*w);
      i++;
    }
  }
  erasures[i] = -1;

  printf("Erased %d random pieces of data/coding:\n\n", m);
  print_data_and_coding(k, m, w, sizeof(int32), data, coding);
  
  jerasure_schedule_decode_lazy(k, m, w, bitmatrix, erasures, data, coding, w*sizeof(int32), sizeof(int32), 1);
  jerasure_get_stats(stats);

  printf("State of the system after decoding: %.0lf XOR'd bytes\n\n", stats[0]);
  print_data_and_coding(k, m, w, sizeof(int32), data, coding);
  
  return 0;
}
