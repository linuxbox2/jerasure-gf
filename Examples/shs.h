#ifndef _SHS_DEFINED

#include <stdint.h>
// #include "k5-int.h"

#define _SHS_DEFINED

/* Some useful types */

typedef unsigned char	BYTE;

typedef uint32_t	LONG;


/* Define the following to use the updated SHS implementation */
#define NEW_SHS         /**/

/* The SHS block size and message digest sizes, in bytes */

#define SHS_DATASIZE    64
#define SHS_DIGESTSIZE  20

/* The structure for storing SHS info */

typedef struct {
               LONG digest[ 5 ];            /* Message digest */
               LONG countLo, countHi;       /* 64-bit bit count */
               LONG data[ 16 ];             /* SHS data buffer */
               } SHS_INFO;

/* Message digest functions (shs.c) */
void shsInit (SHS_INFO *shsInfo);
void shsUpdate (SHS_INFO *shsInfo, BYTE *buffer, int count);
void shsFinal (SHS_INFO *shsInfo);

#define NIST_SHA_CKSUM_LENGTH		SHS_DIGESTSIZE
#define HMAC_SHA_CKSUM_LENGTH		SHS_DIGESTSIZE

#endif /* _SHS_DEFINED */
