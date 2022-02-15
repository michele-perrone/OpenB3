#ifndef FMEMSTREAM_H
#define FMEMSTREAM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Specification.  */
#include <stdio.h>

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

# define INITIAL_ALLOC 64

#ifndef HAVE_MEMSTREAM
FILE *fmemopen(void *buf, size_t size, const char *mode);
FILE * open_memstream (char **buf, size_t *len);
#endif

#ifdef __cplusplus
}
#endif

#endif
