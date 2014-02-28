This is derived from revision 1.2A of Jerasure.  1.2A was basically equivalent
to revision 1.2 except it was released under the New BSD license.  Like 1.2A,
this is written in C.

Changes from 1.2A:
 * cmake
 * revised API with "jerasure context".
 * can use galois math from "GF-complete".

See technical report CS-08-627 for a description of jerasure.
	http://web.eecs.utk.edu/~plank/plank/papers/CS-08-627.html
See technical report CS-13-703 for a description of the galois field math.
	http://web.eecs.utk.edu/~plank/plank/papers/CS-13-703.html
Download GF here,
	http://web.eecs.utk.edu/~plank/plank/papers/CS-13-703/gf_complete_0.1.tar

There are two directories:

The lib directory contains the jerasure code.

The Examples directory contains the example programs.  

To build:
	cmake
	make
building out of tree
	make <somewhere>; cd <somewhere>
	cmake <path-to-source>
	make
Building with gf,
	cmake -DWITH_GF=<where-gf-built>
	make

To build with debug symbols, add "-DCMAKE_BUILD_TYPE=Debug" to the cmake line.
