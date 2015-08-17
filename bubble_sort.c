/*
 * bubble_sort.c
 *
 *  Improved bubble sort that uses anchor pointer instead of exchanged flag.
 *
 *  Created on: 2015/05/23
 *      Author: leo
 */

#include "sort.h"

void bubble_sort(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    dump_pointer("bubble_sort() start in " __FILE__, base, nmemb);
#endif
    void **last = base + (nmemb - 1);
    do {
        void **p1, **p2, **anchor = base + 1;
#ifdef DEBUG
    dump_pointer("search", base, last - base + 1);
        qsort_called++; // loop count
#endif
        for (p1 = base; p1 < last; p1 = p2) {   // search a reverse order pair
            if (compare(*p1, *(p2 = p1 + 1)) > 0) {  // found
#ifdef DEBUG
				if (trace_level >= TRACE_MOVE) fprintf(OUT, "%s <--> %s\n", dump_data(*p1), dump_data(*p2));
#endif
				void *tmp = *p1; *p1 = *p2; *p2 = tmp;	// swap
				anchor = p2;
            }
        }
        last = anchor - 1;
    } while (base < last);
#ifdef DEBUG
    dump_pointer("bubble_sort() done.", base, nmemb);
#endif
}
