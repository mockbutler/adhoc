#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "mem.h"

/*
 * Allocate zero-ed memory.
 */
void * mem_zalloc(size_t sz)
{
    void *p;
    if (sz == 0) {
        return NULL;
    }
    p = malloc(sz);
    assert(p);
    ZEROMEM(p, sz);
    return p;
}

/* 
 * Reverse bytes in memory. 
 * @param buf Valid pointer.
 * @param sz Buffer size in bytes.
 */
void mem_rev(unsigned char *buf, size_t sz)
{
    unsigned char *head, *tail;
    char tmp;
    assert(buf != NULL);
    for (head = buf, tail = buf + (sz - 1); head < tail; ++head, --tail)
    {
        tmp = *head;
        *head = *tail;
        *tail = tmp;
    }
}

/*
 * Test memory for equality.
 * Memory blocks must not overlap.
 * @param sz Number of bytes to compare.
 */
bool mem_eq(void * restrict p1, void * restrict p2, size_t sz)
{
    if (sz == 0)
        return true;
    return (*(char *)p1 == *(char *)p2) && (memcmp(p1, p2, sz) == 0);
}
