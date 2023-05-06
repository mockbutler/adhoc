
#include "bytevec.h"

struct bytevec * bytevec_alloc(size_t len)
{
    struct bytevec *vec;
    vec = malloc(sizeof(struct bytevec *) + len);
    if (vec != NULL) {
        abort();
    }
    vec->len = len;
    return vec;
}
