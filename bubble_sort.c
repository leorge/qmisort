/*
 * bubble_sort.c
 *
 *  Improved bubble sort that uses anchor pointer instead of exchanged flag.
 *
 *  Created on: 2015/05/23
 *      Author: leo
 */

#include "sort.h"

/* pointer sorting  */
void bubble_pointer(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("bubble_pointer() start in " __FILE__, base, nmemb);
#endif
    void **last = base + nmemb - 1;
    do {
        void **p1, **p2, *tmp, **anchor = base + 1;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_pointer("bubble_pointer()", base, last - base + 1);
#endif
        for (p1 = base; p1 < last; p1++) {
            if (compare(*p1, *(p2 = p1 + 1)) > 0) {
            	tmp = *p1;
            	*p1 = *p2;
            	*(anchor = p2) = tmp;
            }
        }
        last = anchor - 1;
    } while (base < last);
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("bubble_pointer() done.", base, nmemb);
#endif
}

/* array sorting */

void bubble_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
    char tmp[size];
#define first   ((char *)base)
    char *last = first + (nmemb - 1) * size;
    do {
        char *p1, *p2, *anchor = first + size;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("search", first, (last - first) / size + 1, size);
        qsort_called++; // loop count
#endif
        for (p1 = first; p1 < last; p1 += size) {   // search a reverse order pair
            if (compare(p1, p2 = p1 + size) > 0) {  // found
#ifdef DEBUG
				qsort_moved += 3;
				if (trace_level >= TRACE_MOVE) fprintf(OUT, "%s <--> %s\n", dump_data(p1), dump_data(p2));
#endif
            	memcpy(tmp, p1, size);
            	memcpy(p1, p2, size);
            	memcpy(anchor = p2, tmp, size);
            }
        }
        last = anchor - size;
    } while (first < last);
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("bubble_sort() done.", base, nmemb, size);
#endif
}
