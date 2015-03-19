/*
 * dump.c
 *
 *  Created on: 2012/12/01
 *      Author: leo
 */

#ifdef DEBUG

#include    "sort.h"

// Dump an element.
const char *dump_data(const void *data) {
    return ispointer? *(const char **)data: data ;
}

void dump_array(const char *msg, const void *base, size_t nmemb, size_t size)
{
    if (trace_level < TRACE_DUMP) return;
    size_t  i;
    const char  *p = base;
    fprintf(OUT, "%s nmemb = %ld :", msg, nmemb);
    if (nmemb > 16) {
        for (i = 0; i++ < 8; p += size) fprintf(OUT, " %s", dump_data(p));
        fprintf(OUT, " ...");
        p = base;
        p += size * (i = nmemb - 8);
        for (; i++ < nmemb; p += size) fprintf(OUT, " %s", dump_data(p));
    }
    else for (i = 0; i++ < nmemb; p += size) fprintf(OUT, " %s", dump_data(p));
    fprintf(OUT, "\n");
}

void dump_pointer(char *msg, void *base[], size_t nmemb) {
    if (trace_level < TRACE_DUMP) return;
    size_t  i;
    fprintf(OUT, "%s nmemb = %ld :", msg, nmemb);
    if (nmemb > 16) {
        for (i = 0; i < 8;) fprintf(OUT, " %s", dump_data((char *)base[i++]));
        fprintf(OUT, " ...");
        for (i = nmemb - 8; i < nmemb;) fprintf(OUT, " %s", dump_data((char *)base[i++]));
    }
    else for (i = 0; i < nmemb;) fprintf(OUT, " %s", dump_data((char *)base[i++]));
    fprintf(OUT, "\n");
//  if (nmemb > 16) dump_pointer(msg, &base[8], nmemb - 16);    // dump whole data
}

char *dump_index(char *buf, INSERT_INDEX index, int length) {
    if (trace_level < TRACE_DUMP) return NULL;
    char    *p = buf;
    int     i;
    for (i = 0; i++ < length; ) *p++ = '\0';    // fill with zero
    *p = '\0';
    for (i = 0; i++ < length;) {
        int c = index & MASK_DIGIT;
        *--p = c + (c < 10 ? '0' : 'A' - 10);   // Hexadecimal character
        index >>= DIGIT_WIDTH;
        index &= ((INSERT_INDEX)1 << ((MAX_SIZE - 1) * DIGIT_WIDTH)) - 1;
    }
    return  buf;
}

void    dump_rate(size_t n_lo, size_t n_hi) {
    if (trace_level >= TRACE_DUMP)
        fprintf(OUT, "sub partition ( %ld , %ld ) rate = %.3f\tdiff = %ld\n", n_lo, n_hi
            , (double)n_lo / (n_lo + n_hi)
            , (n_lo > n_hi)? (n_lo - n_hi): (n_hi - n_lo));
}
#endif
