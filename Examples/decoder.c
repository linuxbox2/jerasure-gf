/* Examples/decoder.c
 * Catherine D. Schuman, James S. Plank

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
This program takes as input an inputfile, k, m, a coding
technique, w, and packetsize.  It is the companion program
of encoder.c, which creates k+m files.  This program assumes 
that up to m erasures have occurred in the k+m files.  It
reads in the k+m files or marks the file as erased. It then
recreates the original file and creates a new file with the
suffix "decoded" with the decoded contents of the file.

This program does not error check command line arguments because 
it is assumed that encoder.c has been called previously with the
same arguments, and encoder.c does error check.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <signal.h>
#include "jerasure.h"
#include "reed_sol.h"
#include "galois.h"
#include "cauchy.h"
#include "liberation.h"

#define N 10

enum Coding_Technique {Reed_Sol_Van, Reed_Sol_R6_Op, Cauchy_Orig, Cauchy_Good, Liberation, Blaum_Roth, Liber8tion, RDP, EVENODD, No_Coding};

char *Methods[N] = {"reed_sol_van", "reed_sol_r6_op", "cauchy_orig", "cauchy_good", "liberation", "blaum_roth", "liber8tion", "rdp", "evenodd", "no_coding"};

/* Global variables for signal handler */
enum Coding_Technique method;
int readins, n;

/* Function prototype */
void ctrl_bs_handler(int dummy);

int main (int argc, char **argv) {
	FILE *fp;				// File pointer

	/* Jerasure arguments */
	char **data;
	char **coding;
	int *erasures;
	int *erased;
	int *matrix;
	int *bitmatrix;
	
	/* Parameters */
	int k, m, w, packetsize, buffersize;
	enum Coding_Technique tech;
	char *c_tech;
	
	int i, j;				// loop control variables
	int blocksize;			// size of individual files
	int origsize;			// size of file before padding
	int total;				// used to write data, not padding to file
	struct stat status;		// used to find size of individual files
	int numerased;			// number of erased files
		
	/* Used to recreate file names */
	char *temp;
	char *cs1, *cs2, *extension;
	char *fname;
	int md;

	/* Used to time decoding */
	struct timeval t1, t2, t3, t4;
	struct timezone tz;
	double tsec;
	double totalsec;

	struct jerasure_context *ctx;
	
	signal(SIGQUIT, ctrl_bs_handler);

	matrix = NULL;
	bitmatrix = NULL;
	totalsec = 0.0;
	
	/* Start timing */
	gettimeofday(&t1, &tz);

	/* Error checking parameters */
	if (argc != 2) {
		fprintf(stderr, "usage: inputfile\n");
		exit(0);
	}
	/* Begin recreation of file names */
	cs1 = (char*)malloc(sizeof(char)*strlen(argv[1]));
	cs2 = strrchr(argv[1], '/');
	if (cs2 != NULL) {
		cs2++;
		strcpy(cs1, cs2);
	}
	else {
		strcpy(cs1, argv[1]);
	}
	cs2 = strchr(cs1, '.');
	if (cs2 != NULL) {
                extension = strdup(cs2);
		*cs2 = '\0';
	} else {
           extension = strdup("");
        }	
	fname = (char *)malloc(sizeof(char*)*(100+strlen(argv[1])+10));

	/* Read in parameters from metadata file */
	sprintf(fname, "Coding/%s_meta.txt", cs1);

	fp = fopen(fname, "rb");
        if (fp == NULL) {
          fprintf(stderr, "Error: no metadata file %s\n", fname);
          exit(1);
        }
	temp = (char *)malloc(sizeof(char)*(strlen(argv[1])+10));
	fscanf(fp, "%s", temp);	
	
	if (fscanf(fp, "%d", &origsize) != 1) {
		fprintf(stderr, "Original size is not valid\n");
		exit(0);
	}
	if (fscanf(fp, "%d %d %d %d %d", &k, &m, &w, &packetsize, &buffersize) != 5) {
		fprintf(stderr, "Parameters are not correct\n");
		exit(0);
	}
	c_tech = (char *)malloc(sizeof(char)*(strlen(argv[1])+10));
	fscanf(fp, "%s", c_tech);
	fscanf(fp, "%d", &tech);
	method = tech;
	fscanf(fp, "%d", &readins);
	fclose(fp);	

	/* Allocate memory */
	erased = (int *)malloc(sizeof(int)*(k+m));
	for (i = 0; i < k+m; i++)
		erased[i] = 0;
	erasures = (int *)malloc(sizeof(int)*(k+m));

	data = (char **)malloc(sizeof(char *)*k);
	coding = (char **)malloc(sizeof(char *)*m);
	if (buffersize != origsize) {
		for (i = 0; i < k; i++) {
			data[i] = (char *)malloc(sizeof(char)*(buffersize/k));
		}
		for (i = 0; i < m; i++) {
			coding[i] = (char *)malloc(sizeof(char)*(buffersize/k));
		}
		blocksize = buffersize/k;
	}

	sprintf(temp, "%d", k);
	md = strlen(temp);
	gettimeofday(&t3, &tz);

	ctx = jerasure_make_context(w);

	/* Create coding matrix or bitmatrix */
	switch(tech) {
		case No_Coding:
			break;
		case Reed_Sol_Van:
			matrix = reed_sol_vandermonde_coding_matrix(ctx, k, m);
			break;
		case Reed_Sol_R6_Op:
			matrix = reed_sol_r6_coding_matrix(ctx, k);
			break;
		case Cauchy_Orig:
			matrix = cauchy_original_coding_matrix(ctx, k, m);
			bitmatrix = jerasure_matrix_to_bitmatrix(ctx, k, m, matrix);
			break;
		case Cauchy_Good:
			matrix = cauchy_good_general_coding_matrix(ctx, k, m);
			bitmatrix = jerasure_matrix_to_bitmatrix(ctx, k, m, matrix);
			break;
		case Liberation:
			bitmatrix = liberation_coding_bitmatrix(k, w);
			break;
		case Blaum_Roth:
			bitmatrix = blaum_roth_coding_bitmatrix(k, w);
			break;
		case Liber8tion:
			bitmatrix = liber8tion_coding_bitmatrix(k);
	}
	gettimeofday(&t4, &tz);
	tsec = 0.0;
	tsec += t4.tv_usec;
	tsec -= t3.tv_usec;
	tsec /= 1000000.0;
	tsec += t4.tv_sec;
	tsec -= t3.tv_sec;
	totalsec += tsec;
	
	/* Begin decoding process */
	total = 0;
	n = 1;	
	while (n <= readins) {
		numerased = 0;
		/* Open files, check for erasures, read in data/coding */	
		for (i = 0; i < k; i++) {
			sprintf(fname, "Coding/%s_k%0*d%s", cs1, md, (i+1), extension);
			fp = fopen(fname, "rb");
			if (fp == NULL) {
				erased[i] = 1;
				erasures[numerased] = i;
				numerased++;
				//printf("%s failed\n", fname);
			}
			else {
				if (buffersize == origsize) {
					stat(fname, &status);
					blocksize = status.st_size;
					data[i] = (char *)malloc(sizeof(char)*blocksize);
					fread(data[i], sizeof(char), blocksize, fp);
				}
				else {
					fseek(fp, blocksize*(n-1), SEEK_SET); 
					fread(data[i], sizeof(char), buffersize/k, fp);
				}
				fclose(fp);
			}
		}
		for (i = 0; i < m; i++) {
			sprintf(fname, "Coding/%s_m%0*d%s", cs1, md, (i+1), extension);
				fp = fopen(fname, "rb");
			if (fp == NULL) {
				erased[k+i] = 1;
				erasures[numerased] = k+i;
				numerased++;
				//printf("%s failed\n", fname);
			}
			else {
				if (buffersize == origsize) {
					stat(fname, &status);
					blocksize = status.st_size;
					coding[i] = (char *)malloc(sizeof(char)*blocksize);
					fread(coding[i], sizeof(char), blocksize, fp);
				}
				else {
					fseek(fp, blocksize*(n-1), SEEK_SET);
					fread(coding[i], sizeof(char), blocksize, fp);
				}	
				fclose(fp);
			}
		}
		/* Finish allocating data/coding if needed */
		if (n == 1) {
			for (i = 0; i < numerased; i++) {
				if (buffersize != origsize) {
				} else if (erasures[i] < k) {
					data[erasures[i]] = (char *)malloc(sizeof(char)*blocksize);
				}
				else {
					coding[erasures[i]-k] = (char *)malloc(sizeof(char)*blocksize);
				}
			}
		}
		
		erasures[numerased] = -1;
		gettimeofday(&t3, &tz);
	
		/* Choose proper decoding method */
		if (tech == Reed_Sol_Van || tech == Reed_Sol_R6_Op) {
			i = jerasure_matrix_decode(ctx, k, m, matrix, 1, erasures, data, coding, blocksize);
		}
		else if (tech == Cauchy_Orig || tech == Cauchy_Good || tech == Liberation || tech == Blaum_Roth || tech == Liber8tion) {
			i = jerasure_schedule_decode_lazy(k, m, w, bitmatrix, erasures, data, coding, blocksize, packetsize, 1);
		}
		else {
			fprintf(stderr, "Not a valid coding technique.\n");
			exit(0);
		}
		gettimeofday(&t4, &tz);
	
		/* Exit if decoding was unsuccessful */
		if (i == -1) {
			fprintf(stderr, "Unsuccessful!\n");
			exit(0);
		}
	
		/* Create decoded file */
		sprintf(fname, "Coding/%s_decoded%s", cs1, extension);
		if (n == 1) {
			fp = fopen(fname, "wb");
		}
		else {
			fp = fopen(fname, "ab");
		}
		for (i = 0; i < k; i++) {
			if (total+blocksize <= origsize) {
				fwrite(data[i], sizeof(char), blocksize, fp);
				total+= blocksize;
			}
			else {
				for (j = 0; j < blocksize; j++) {
					if (total < origsize) {
						fprintf(fp, "%c", data[i][j]);
						total++;
					}
					else {
						break;
					}
					
				}
			}
		}
		n++;
		fclose(fp);
		tsec = 0.0;
		tsec += t4.tv_usec;
		tsec -= t3.tv_usec;
		tsec /= 1000000.0;
		tsec += t4.tv_sec;
		tsec -= t3.tv_sec;
		totalsec += tsec;
	}
	
	/* Free allocated memory */
	free(cs1);
	free(extension);
	free(fname);
	for (i = 0; i < k; i++) {
		free(data[i]);
	}
	for (i = 0; i < m; i++) {
		free(coding[i]);
	}
	free(data);
	free(coding);
	free(erasures);
	free(erased);

	free(temp);
	free(c_tech);

	if (bitmatrix)
		free(bitmatrix);
	jerasure_release_context(ctx);
	
	/* Stop timing and print time */
	gettimeofday(&t2, &tz);
	tsec = 0;
	tsec += t2.tv_usec;
	tsec -= t1.tv_usec;
	tsec /= 1000000.0;
	tsec += t2.tv_sec;
	tsec -= t1.tv_sec;
	printf("Decoding (MB/sec): %0.10f\n", (origsize/1024/1024)/totalsec);
	printf("De_Total (MB/sec): %0.10f\n\n", (origsize/1024/1024)/tsec);
}	

void ctrl_bs_handler(int dummy) {
	time_t mytime;
	mytime = time(0);
	fprintf(stderr, "\n%s\n", ctime(&mytime));
	fprintf(stderr, "You just typed ctrl-\\ in decoder.c\n");
	fprintf(stderr, "Total number of read ins = %d\n", readins);
	fprintf(stderr, "Current read in: %d\n", n);
	fprintf(stderr, "Method: %s\n\n", Methods[method]);
	signal(SIGQUIT, ctrl_bs_handler);
}
