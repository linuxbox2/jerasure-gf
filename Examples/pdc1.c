/* Examples/pdc1.c
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

void print_data_and_coding_1(int k, int m, int w, int size, 
		char **data, char **coding) 
{
  int i, j, x;
  int n, sp;
  gdata l;

  if(k > m) n = k;
  else n = m;
  sp = size * 2 + size/(w/8) + 8;

  printf("%-*sCoding\n", sp, "Data");
  for(i = 0; i < n; i++) {
	  if(i < k) {
		  printf("D%-2d:", i);
		  for(j=0;j< size; j+=(w/8)) { 
			  printf(" ");
			  for(x=0;x < w/8;x++){
				printf("%02x", (unsigned char)data[i][j+x]);
			  }
		  }
		  printf("    ");
	  }
	  else printf("%*s", sp, "");
	  if(i < m) {
		  printf("C%-2d:", i);
		  for(j=0;j< size; j+=(w/8)) { 
			  printf(" ");
			  for(x=0;x < w/8;x++){
				printf("%02x", (unsigned char)coding[i][j+x]);
			  }
		  }
	  }
	  printf("\n");
  }
	printf("\n");
}
