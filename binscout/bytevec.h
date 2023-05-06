#ifndef BYTEVEC_H
#define BYTEVEC_H

#include <stdlib.h>

/* Byte vector. */
struct bytevec
{
    size_t len;
    unsigned char vec[];
};

#define BYTEVEC_INIT { .len = 0u; }

#define bytevec_free(bvp) free(bvp)

struct bytevec * bytevec_alloc(size_t len);

#endif