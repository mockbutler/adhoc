#ifndef MEM_H
#define MEM_H

#include <stdlib.h>

/* Zero out memory at pointer. */
#define ZEROMEM(p, size) memset(p, 0, size)

/* 
 * Zero out memory at pointer using pointer type to determine
 * size.
 */
#define ZEROMEMAT(p) ZEROMEM(p, sizeof(*(p)))

/* Zero out variable memory. */
#define ZEROVAR(v) ZEROMEM(&(v), sizeof(v))

void * mem_zalloc(size_t);
void mem_rev(unsigned char *, size_t);
bool mem_eq(void * restrict, void * restrict, size_t);

#endif