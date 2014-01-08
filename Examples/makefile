# Examples/makefile
# Jerasure - A C/C++ Library for a Variety of Reed-Solomon and RAID-6 Erasure Coding Techniques
# 
# Revision 1.2A
# May 24, 2011
# 
# James S. Plank
# Department of Electrical Engineering and Computer Science
# University of Tennessee
# Knoxville, TN 37996
# plank@cs.utk.edu
# 
# Copyright (c) 2011, James S. Plank
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
#  - Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 
#  - Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 
#  - Neither the name of the University of Tennessee nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
# WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

VPATH=../../Jerasure-1.2A/Examples
PFX=../../Jerasure-1.2A/lib/
#PFX=../lib
CC = gcc  
#OPTS=-O2
OPTS=-g
CFLAGS = -I. -I../lib -I../../Jerasure-1.2A/lib $(OPTS)

ALL =	jerasure_01 \
        jerasure_02 \
        jerasure_03 \
        jerasure_04 \
        jerasure_05 \
        jerasure_06 \
        jerasure_07 \
        jerasure_08 \
        reed_sol_01 \
        reed_sol_02 \
        reed_sol_03 \
        reed_sol_04 \
        cauchy_01 \
        cauchy_02 \
        cauchy_03 \
        cauchy_04 \
        liberation_01 \
		encoder \
		decoder \
		test1 \

all: $(ALL)

clean:
	rm -f core *.o $(ALL) a.out

#.SUFFIXES: .c .o
#.c.o:
	#$(CC) $(CFLAGS) -c $*.c

H=$(PFX)galois.h $(PFX)jerasure.h
H2=$(PFX)galois.h $(PFX)reed_sol.h $(PFX)jerasure.h
H3=$(PFX)galois.h $(PFX)cauchy.h $(PFX)jerasure.h
H4=$(PFX)galois.h $(PFX)liberation.h $(PFX)jerasure.h
H5=$(PFX)galois.h $(PFX)liberation.h $(PFX)jerasure.h $(PFX)reed_sol.h $(PFX)cauchy.h
L=../lib/libjerasure.a

jerasure_01.o: $(H)
jerasure_01: jerasure_01.o $(L)
	$(CC) $(CFLAGS) -o jerasure_01 jerasure_01.o $(L)

jerasure_02.o: $(H) 
jerasure_02: jerasure_02.o $(L)
	$(CC) $(CFLAGS) -o jerasure_02 jerasure_02.o $(L)

jerasure_03.o: $(H) 
jerasure_03: jerasure_03.o $(L)
	$(CC) $(CFLAGS) -o jerasure_03 jerasure_03.o $(L)

jerasure_04.o: $(H) 
jerasure_04: jerasure_04.o $(L)
	$(CC) $(CFLAGS) -o jerasure_04 jerasure_04.o $(L)

jerasure_05.o: $(H) 
jerasure_05: jerasure_05.o fillrand.o pdc1.o $(L)
	$(CC) $(CFLAGS) -o jerasure_05 jerasure_05.o fillrand.o pdc1.o $(L)

jerasure_06.o: $(H) 
jerasure_06: jerasure_06.o fillrand.o pdc2.o $(L)
	$(CC) $(CFLAGS) -o jerasure_06 jerasure_06.o fillrand.o pdc2.o $(L)

jerasure_07.o: $(H) 
jerasure_07: jerasure_07.o fillrand.o pdc2.o $(L)
	$(CC) $(CFLAGS) -o jerasure_07 jerasure_07.o fillrand.o pdc2.o $(L)

jerasure_08.o: $(H) 
jerasure_08: jerasure_08.o fillrand.o pdc2.o $(L)
	$(CC) $(CFLAGS) -o jerasure_08 jerasure_08.o fillrand.o pdc2.o $(L)

reed_sol_01.o: $(H2)
reed_sol_01: reed_sol_01.o fillrand.o pdc1.o $(L)
	$(CC) $(CFLAGS) -o reed_sol_01 reed_sol_01.o fillrand.o pdc1.o $(L)

reed_sol_02.o: $(H2)
reed_sol_02: reed_sol_02.o $(L)
	$(CC) $(CFLAGS) -o reed_sol_02 reed_sol_02.o $(L)

reed_sol_03.o: $(H2)
reed_sol_03: reed_sol_03.o fillrand.o pdc1.o $(L)
	$(CC) $(CFLAGS) -o reed_sol_03 reed_sol_03.o fillrand.o pdc1.o $(L)

reed_sol_04.o: $(H2)
reed_sol_04: reed_sol_04.o fillrand.o $(L)
	$(CC) $(CFLAGS) -o reed_sol_04 reed_sol_04.o fillrand.o $(L)

cauchy_01.o: $(H3)
cauchy_01: cauchy_01.o $(L)
	$(CC) $(CFLAGS) -o cauchy_01 cauchy_01.o $(L)

cauchy_02.o: $(H3)
cauchy_02: cauchy_02.o fillrand.o pdc2.o $(L)
	$(CC) $(CFLAGS) -o cauchy_02 cauchy_02.o fillrand.o pdc2.o $(L)

cauchy_03.o: $(H3)
cauchy_03: cauchy_03.o fillrand.o pdc2.o $(L)
	$(CC) $(CFLAGS) -o cauchy_03 cauchy_03.o fillrand.o pdc2.o $(L)

cauchy_04.o: $(H3)
cauchy_04: cauchy_04.o fillrand.o pdc2.o $(L)
	$(CC) $(CFLAGS) -o cauchy_04 cauchy_04.o fillrand.o pdc2.o $(L)

liberation_01.o: $(H4)
liberation_01: liberation_01.o fillrand.o pdc2.o $(L)
	$(CC) $(CFLAGS) -o liberation_01 liberation_01.o fillrand.o pdc2.o $(L)

test1: test1.o rc4.o shs_n.o
	$(CC) $(CFLAGS) -o test1 test1.o rc4.o shs_n.o pdc1.o pdc2.o $(L)

encoder.o: $(H5)
encoder: encoder.o $(L) $(L)
	$(CC) $(CFLAGS) -o encoder encoder.o $(L)

decoder.o: $(H5)
decoder: decoder.o $(L)
	$(CC) $(CFLAGS) -o decoder decoder.o $(L)
