/* lib/rc4/rc4_enc.c */
/* Copyright (C) 1995-1996 Eric Young (eay@mincom.oz.au)
 * All rights reserved.
 *
 * This file is part of an SSL implementation written
 * by Eric Young (eay@mincom.oz.au).
 * The implementation was written so as to conform with Netscapes SSL
 * specification.  This library and applications are
 * FREE FOR COMMERCIAL AND NON-COMMERCIAL USE
 * as long as the following conditions are aheared to.
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.  If this code is used in a product,
 * Eric Young should be given attribution as the author of the parts used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by Eric Young (eay@mincom.oz.au)
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#include "rc4.h"

/* if this is defined data[i] is used instead of *data, this is a %20
 * speedup on x86 */
/* #undef RC4_INDEX /**/

#if 0
char *RC4_version="RC4 part of SSLeay 0.6.5 03-Dec-1996";

char *RC4_options()
	{
#ifdef RC4_INDEX
	if (sizeof(RC4_INT) == 1)
		return("rc4(idx,char)");
	else
		return("rc4(idx,int)");
#else
	if (sizeof(RC4_INT) == 1)
		return("rc4(ptr,char)");
	else
		return("rc4(ptr,int)");
#endif
	}
#endif

/* RC4 as implemented from a posting from
 * Newsgroups: sci.crypt
 * From: sterndark@netcom.com (David Sterndark)
 * Subject: RC4 Algorithm revealed.
 * Message-ID: <sternCvKL4B.Hyy@netcom.com>
 * Date: Wed, 14 Sep 1994 06:35:31 GMT
 */

void rc4_set_key(ks, len, key, rekey)
    rc4_key_schedule *ks;
    int len;
    register unsigned char *key;
    int rekey;
{
    register RC4_INT t;
    register unsigned y;
    register RC4_INT *s;
    register unsigned x;
    register z;

    s= &(ks->state[0]);
    if (!rekey) {
	for (x = 0; x <= 255; ++x)
	    s[x] = x;
	ks->x = 0;
	ks->y = 0;
    }
    z=y=0;

    x = 0;
    do {
	t=s[x];
	y = (unsigned char)(key[z] + t + y);
	if (++z == len) z=0;
	s[x]=s[y];
	s[y]=t;
    } while (++x <= 255);
}

void rc4(key, len, ip, op)
    rc4_key_schedule *key;
    unsigned long len;
    unsigned char *ip;
    unsigned char *op;
{
    register RC4_INT *s;
    register RC4_INT x,y,t,u;
    int i;

    x=key->x;
    y=key->y;
    s=key->state;

#ifndef RC4_INDEX
#define IN_DATA		(*ip++)
#define OUT_DATA	(*op++)
#else
#define IN_DATA		ip[i]
#define OUT_DATA	op[i]
    ip += len; op += len;
#endif

    i= -(int)len;
    for (;;) {
	++x;
	t=s[x];
	y += t;
	u = s[y];
	s[x] = u;
	s[y] = t;
	t += u;
	OUT_DATA = IN_DATA ^ s[t];
	if (!++i) break;
    }
    key->x=x;
    key->y=y;
}
