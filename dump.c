/*
 * dump.c
 *
 *  Created on: 2012/12/01
 *      Author: leo
 */

#include    "sort.h"

static const char *fmt = sizeof(size_t)==sizeof(unsigned int)? "%d": "%ld";

const char *dump_size_t(char *buf, size_t val) {
    static char tmp[32];
    char *out = buf ? buf : tmp;
    sprintf(out, fmt, val);
    return  out;
}

// Dump an element.
const char *dump_data(const void *data) {
    return data;
}

void dump_copy(void *dst, const void *src)
{
    qsort_moved++;
    if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(dst = %s, src = %s)\n", dump_data(dst), dump_data(src));
}

void dump_array(const char *msg, const void *base, size_t left, size_t middle, size_t right, size_t size)
{
    if (trace_level < TRACE_DUMP) return;
    size_t  i, nmemb = left + middle + right;
    const char  *p = base;
    fprintf(OUT, "%s nmemb = %s :", msg, dump_size_t(NULL, nmemb));
    if (nmemb <= 16) {
        if (left > 0) for (i = 0; i++ < left; p += size) fprintf(OUT, " %s", dump_data(p));
        if (middle > 0 || right > 0) {
            fprintf(OUT, " [");
            for(i = 0; i++ < middle; p+= size) fprintf(OUT, " %s", dump_data(p));
            fprintf(OUT, " ]");
        }
        if (right > 0) for (i = 0; i++ < right; p += size) fprintf(OUT, " %s", dump_data(p));
    } else {
        for (i = 0; i++ < 8; p += size) fprintf(OUT, " %s", dump_data(p));
        fprintf(OUT, " ...");
        p = base;
        p += size * (i = nmemb - 8);
        for (; i++ < nmemb; p += size) fprintf(OUT, " %s", dump_data(p));
    }
    fprintf(OUT, "\n");
}

void dump_pointer(char *msg, void *base[], size_t nmemb) {
    if (trace_level < TRACE_DUMP) return;
    size_t  i;
    fprintf(OUT, "%s nmemb = %s :", msg, dump_size_t(NULL, nmemb));
    if (nmemb > 16) {
        for (i = 0; i < 8;) fprintf(OUT, " %s", dump_data((char *)base[i++]));
        fprintf(OUT, " ...");
        for (i = nmemb - 8; i < nmemb;) fprintf(OUT, " %s", dump_data((char *)base[i++]));
    }
    else for (i = 0; i < nmemb;) fprintf(OUT, " %s", dump_data((char *)base[i++]));
    fprintf(OUT, "\n");
//  if (nmemb > 16) dump_pointer(msg, &base[8], nmemb - 16);    // dump whole data
}

void    dump_rate(size_t n_lo, size_t n_hi) {
    if (trace_level >= TRACE_DUMP) {
        char tmp1[32], tmp2[32], tmp3[32];
        fprintf(OUT, "sub partition ( %s , %s ) rate = %.3f\tdiff = %s\n"
            , dump_size_t(tmp1, n_lo), dump_size_t(tmp2, n_hi), (double)n_lo / (n_lo + n_hi)
            , dump_size_t(tmp3, n_lo > n_hi? n_lo - n_hi: n_hi - n_lo));
    }
}
