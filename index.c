/*
 * index.c
 *
 *  Use a pair of make_index() and unindex().
 *
 *  Created on: 2013/01/09
 *      Author: leo
 */

#include "sort.h"

static void copy(void *dst, const void *src, size_t n)
{
#ifdef DEBUG
    qsort_moved++;
    if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(dst = %p, src = %p : %s , size = %ld)\n", dst, src, dump_data(src), n);
#endif
    if (dst != src) memcpy(dst, src, n); /* restore an elements  */
#ifdef  DEBUG
    else {
        fprintf(stderr, "Copy(dst == src : %p, size = %ld)\n", dst, n);
        exit(-1);
    }
#endif
}

/***** Make an idex table from array 1d *****/
void **make_index(void *array1d, size_t nmemb, size_t size)
{
    void **rtn = calloc(nmemb, sizeof(void *));
    if (rtn != NULL) {
        void    **ptr, *adrs;
        size_t  i;
        for (adrs = array1d, ptr = rtn, i = 0; i++ < nmemb; adrs += size) {
            *ptr++ = adrs;  // make an index table
        }
#ifdef DEBUG
        char    msg[30];
        if (trace_level >= TRACE_DUMP) sprintf(msg, "make_index(%p) at %p", array1d, rtn);
        dump_pointer(msg, rtn, nmemb);
#endif
    }
    else perror(NULL);
    return  rtn;
}

/***** Restore array 1d from index table *****/
void unindex(void *array1d, void *idxtbl[], size_t nmemb, size_t size)
{
    void    **ptr, *adrs;
    size_t  i;
    char    tbuf[size];     // Temporary buffer
#ifdef DEBUG
    static char *name = "unindex()";
    if (trace_level >= TRACE_DUMP) {
        dump_pointer(name, idxtbl, nmemb);
        if (trace_level >= TRACE_DEBUG)
        	for (int i = 0; i < nmemb; i++)
        		fprintf(OUT, "idxtbl[%d] = %p\t%s\n", i, idxtbl[i], (char *)idxtbl[i]);
    }
#endif
    for (adrs = array1d, ptr = idxtbl, i = 0; i < nmemb; adrs += size, ptr++, i++) {    // cyclic permutation
#ifdef DEBUG
        if (trace_level >= TRACE_DEBUG) fprintf(OUT, "array1d[%ld] = %p\t%s\n", i, adrs, dump_data(adrs));
#endif
        void    **p, *dst, *src;
        if (*(p = ptr) != adrs) {
            copy(tbuf, dst = adrs, size);   // save an element
            do {
                copy(dst, src = *p, size);          // copy an element
                *p = dst;                               // restore in idxtbl
                p = &idxtbl[((dst = src) - array1d) / size];
            } while (*p != adrs);
            copy(*p = src, tbuf, size);     // restore saved element
        }
    }
}

/***** Pointer sort of qsort(3) *****/
static int      (*comp)(const void *, const void *);

static int  comp_idx(const void *p1, const void *p2) {
#ifdef DEBUG
    if (trace_level >= TRACE_COMPARE) fprintf(OUT, "comp_idx(%s, %s)\n",
        dump_data(*(char * const *)p1), dump_data(*(char * const *)p2));
#endif
    return  comp(*(char * const *)p1, *(char * const *)p2);
}

void qsort3_pointer(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    comp = compare;
    qsort((void *)base, nmemb, sizeof(void *), comp_idx);
}

void qsort3_index(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
    void **idxtbl = make_index(base, nmemb, size);
    if (idxtbl != NULL) {
        qsort3_pointer(idxtbl, nmemb, compare);
        unindex(base, idxtbl, nmemb, size);
        free(idxtbl);
    }
}
