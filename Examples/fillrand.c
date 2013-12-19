#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jerasure.h"
#include "examples.h"

void
fillrand(char *cp, int s)
{
	int l, i;
	while (s > 0) {
		l = mrand48();
		i = sizeof l;
		if (i > s) i = s;
		memcpy(cp, &l, i);
		cp += i;
		s -= i;
	}
}
