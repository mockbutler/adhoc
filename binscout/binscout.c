/*
 * Copyright (c) 2010-2016 Marc Butler <mockbutler@gmail.com>
 *
 * Search a file for a byte sequence.
 *
 * Uses the Boyer Moore Horspool substring search algorithm for searching the
 * file contents.
 */

#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 500

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <err.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "bytevec.h"
#include "hex.h"
#include "mem.h"

#define NUMBYTES 256

/* Compile a hexadecimal string into a byte vector. If there is an odd number of
 * hexadecimal digits. The first nybble is assumed to be zero.
 */
struct bytevec *compile_hex(const char *str, size_t len)
{
    unsigned int binlen, ncnt;
    struct bytevec *bvec;

    assert(str != NULL);
    assert(len > 0);

    binlen = (unsigned int)((len + 1) / 2);
    bvec = mem_zalloc(sizeof(struct bytevec) + binlen);
    ncnt = 0;
    if ((len & 1) == 1)
        ncnt++; /* Assume the first nybble is zero. */
    while (ncnt < (2 * binlen))
    {
        if (!isxdigit(*str))
        {
            free(bvec);
            return NULL;
        }
        /* This assumes that vec has been initialized to zero. */
        bvec->vec[ncnt / 2] |= (tohex(*str++) * (((ncnt & 1) == 0) ? 16U : 1U));
        ncnt++;
    }
    bvec->len = binlen;
    return bvec;
}

/* Endianness of integral value needles. */
enum endian
{
    ENDIAN_LITTLE,
    ENDIAN_BIG
};

/* Decompose an integer into a byte vector. */
struct bytevec *decompose_int(uint64_t val, size_t sz, enum endian en)
{
    struct bytevec *bvec;
    int i;

    bvec = mem_zalloc(sizeof(struct bytevec) + sz);
    for (i = 0; i < sz; i++)
        bvec->vec[i] = val >> (i * 8);
    if (en == ENDIAN_BIG)
        mem_rev(bvec->vec, sz);
    bvec->len = sz;
    return bvec;
}

/* Memory mapped file handle. */
struct mmap_file
{
    union
    {
        void *raw;
        unsigned char *uc;
        char *c;
    } contents;
    size_t size;
};

/* Memory map the contents of a file for reading only. */
struct mmap_file *mmap_file_ro(const char *path)
{
    int fd;
    struct stat info;
    void *contents;
    struct mmap_file *pmmf;

    assert(path != NULL);

    fd = open(path, O_RDONLY);
    if (fd < 0)
        err(1, "open %s", path);
    if (fstat(fd, &info) != 0)
        err(1, "stat %s", path);

    contents = mmap(0, (size_t)info.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (contents == MAP_FAILED)
        err(1, "mmap %s", path);
    close(fd);

    pmmf = mem_zalloc(sizeof(*pmmf));
    pmmf->contents.raw = contents;
    pmmf->size = info.st_size;
    return pmmf;
}

/* Release a memory mapped file. */
void mmap_file_close(struct mmap_file *mmf)
{
    if (munmap(mmf->contents.raw, mmf->size) != 0)
        err(-1, "error unmapping memory mapped file");
    memset(mmf, 0, sizeof(*mmf));
}

/* Generate a Boyer Moore Horspool compatible jump table. */
unsigned int *bmh_gen_tbl(struct bytevec *bvec)
{
    unsigned *tbl;
    int i;
    assert(bvec != NULL && bvec->len > 0);
    tbl = mem_zalloc(sizeof(unsigned) * NUMBYTES);
    for (i = 0; i < NUMBYTES; i++)
        tbl[i] = bvec->len;
    for (i = 0; i < bvec->len - 1; i++)
        tbl[bvec->vec[i]] = bvec->len - 1 - i;
    return tbl;
}

/* Use Boyer Moore Horspool string search to look for the byte sequence. */
void bmh_crawl(struct bytevec *bvec, struct mmap_file *mmf)
{
    int rv;
    unsigned int *jmptbl;
    size_t off;
    unsigned char *haystack;
    unsigned char *needle;

    assert(bvec->len > 0);
    assert(mmf->size > 0);

    jmptbl = bmh_gen_tbl(bvec);

    rv = madvise(mmf->contents.raw, mmf->size, MADV_SEQUENTIAL);
    if (rv != 0)
        warn("madvise() failed");

    haystack = mmf->contents.uc;
    needle = bvec->vec;
    off = 0;
    while (mmf->size - off >= bvec->len)
    {
        size_t i = bvec->len - 1;
        while (haystack[off + i] == needle[i])
        {
            if (i == 0)
            {
                printf("%8lx\n", off);
                break;
            }
            i -= 1;
        }
        off = off + jmptbl[haystack[off + bvec->len - 1]];
    }
    free(jmptbl);
}

enum needle_t
{
    NEEDLE_HEX = 0,
    NEEDLE_STR,
    NEEDLE_CSTR,
    NEEDLE_LE16,
    NEEDLE_LE32,
    NEEDLE_LE64,
    NEEDLE_BE16,
    NEEDLE_BE32,
    NEEDLE_BE64
};

static char *const needle_typeids[] = {
    [NEEDLE_HEX] = "hex",
    [NEEDLE_STR] = "str",
    [NEEDLE_CSTR] = "cstr",
    [NEEDLE_LE16] = "le16",
    [NEEDLE_LE32] = "le32",
    [NEEDLE_LE64] = "le64",
    [NEEDLE_BE16] = "be16",
    [NEEDLE_BE32] = "be32",
    [NEEDLE_BE64] = "be64"};

struct bytevec *compile_int(const char *text, size_t sz, enum endian en)
{
    assert(text != NULL);
    char *end = NULL;

    if (text[0] == '-')
    {
        long long int sval;
        sval = strtoll(text, &end, 0);
        return decompose_int((uint64_t)sval, sz, en);
    }
    else
    {
        unsigned long long int uval;
        uval = strtoull(text, &end, 0);
        return decompose_int((uint64_t)uval, sz, en);
    }
}

enum nul_handling
{
    DROP_NUL,
    KEEP_NUL
};

struct bytevec *compile_str(const char *text, enum nul_handling handling)
{
    assert(text != NULL);
    struct bytevec *bvec;
    size_t len = (handling == DROP_NUL) ? strlen(text) : strlen(text) + 1;
    bvec = mem_zalloc(sizeof(struct bytevec) + len);
    memcpy(&bvec->vec, text, len);
    bvec->len = len;
    return bvec;
}

struct bytevec *form_needle(enum needle_t needle_is, const char *text)
{
    struct bytevec *bvec = NULL;
    assert(text != NULL);

    switch (needle_is)
    {
    case NEEDLE_HEX:
        bvec = compile_hex(text, strlen(text));
        break;
    case NEEDLE_STR:
        bvec = compile_str(text, DROP_NUL);
        break;
    case NEEDLE_CSTR:
        bvec = compile_str(text, KEEP_NUL);
        break;
    case NEEDLE_LE16:
        bvec = compile_int(text, 2, ENDIAN_LITTLE);
        break;
    case NEEDLE_LE32:
        bvec = compile_int(text, 4, ENDIAN_LITTLE);
        break;
    case NEEDLE_LE64:
        bvec = compile_int(text, 8, ENDIAN_LITTLE);
        break;
    case NEEDLE_BE16:
        bvec = compile_int(text, 2, ENDIAN_BIG);
        break;
    case NEEDLE_BE32:
        bvec = compile_int(text, 4, ENDIAN_BIG);
        break;
    case NEEDLE_BE64:
        bvec = compile_int(text, 8, ENDIAN_BIG);
        break;
    default:
        assert(0 && "internal error");
    }
    assert(bvec != NULL);
    return bvec;
}

void detailed_usage(void)
{
    puts("\nUsage: binscout [options] needle file\n"
         "\nSearch a binary file for the specified byte sequence.\n"
         "\nOptions:\n"
         "  -h            : This help.\n"
         "  -t <type>     : Needle type: hex, str, cstr, le16, le32, le64, be16, be32, be64\n");
}

int main(int argc, char **argv)
{
    struct bytevec *bvec;
    struct mmap_file *mmf;
    int opt, errfnd;
    char *subopts;
    char *value;
    enum needle_t needle_is = NEEDLE_HEX;

    errfnd = 0;
    while ((opt = getopt(argc, argv, "t:hBL")) != -1)
    {
        switch (opt)
        {
        case 't':
            subopts = optarg;
            while (*subopts != '\0' && !errfnd)
            {
                switch (getsubopt(&subopts, needle_typeids, &value))
                {
                case NEEDLE_HEX:
                    needle_is = NEEDLE_HEX;
                    break;
                case NEEDLE_STR:
                    needle_is = NEEDLE_STR;
                    break;
                case NEEDLE_CSTR:
                    needle_is = NEEDLE_CSTR;
                    break;
                case NEEDLE_LE16:
                    needle_is = NEEDLE_LE16;
                    break;
                case NEEDLE_LE32:
                    needle_is = NEEDLE_LE32;
                    break;
                case NEEDLE_LE64:
                    needle_is = NEEDLE_LE64;
                    break;
                case NEEDLE_BE16:
                    needle_is = NEEDLE_BE16;
                    break;
                case NEEDLE_BE32:
                    needle_is = NEEDLE_BE32;
                    break;
                case NEEDLE_BE64:
                    needle_is = NEEDLE_BE64;
                    break;
                default:
                    err(1, "Invalid needle type.");
                }
            }
            break;
        case 'h':
            detailed_usage();
            exit(EXIT_SUCCESS);
        default:
            puts("\nUsage: binscout [options] needle file\n");
            exit(EXIT_FAILURE);
        }
    }

    if ((argc - optind) < 2)
    {
        puts("\nUsage: binscout [options] needle file\n");
        exit(EXIT_FAILURE);
    }

    bvec = form_needle(needle_is, argv[optind]);
    if (bvec == NULL)
        errx(1, "Unable to parse hex '%s'", argv[optind]);
    mmf = mmap_file_ro(argv[optind + 1]);

    bmh_crawl(bvec, mmf);

    mmap_file_close(mmf);
    free(mmf);
    free(bvec);
    exit(EXIT_SUCCESS);
}
