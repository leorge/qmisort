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
        void **p1, **p2, **anchor = NULL;
#ifdef DEBUG
        dump_pointer("search", base, last - base + 1);
        qsort_called++; // loop count
#endif
        for (p1 = base; p1 < last; p1 = p2) {   // search a reverse order pair
            if (compare(*p1, *(p2 = p1 + 1)) > 0) {  // found
#ifdef DEBUG
				if (trace_level >= TRACE_MOVE) fprintf(OUT, "%s <--> %s\n", dump_data(*p1), dump_data(*p2));
#endif
				void *tmp = *p1; *(anchor = p1) = *p2; *p2 = tmp;	// swap
            }
        }
        if (anchor == NULL) break;
        last = anchor;
    } while (base < last);
#ifdef DEBUG
    dump_pointer("bubble_sort() done.", base, nmemb);
#endif
}

void cocktail_sort(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    dump_pointer("cocktail_sort() start in " __FILE__, base, nmemb);
#endif
    void **last = base + (nmemb - 1);
    do {
        void **p1, **p2, **anchor = NULL;
#ifdef DEBUG
        dump_pointer("search  forward", base, last - base + 1);
        qsort_called++; // loop count
#endif
        for (p1 = base; p1 < last; p1 = p2) {   // search a reverse order pair
            if (compare(*p1, *(p2 = p1 + 1)) > 0) {  // found
#ifdef DEBUG
				if (trace_level >= TRACE_MOVE) fprintf(OUT, "%s <--> %s\n", dump_data(*p1), dump_data(*p2));
#endif
				void *tmp = *p1; *(anchor = p1) = *p2; *p2 = tmp;	// swap
            }
        }
        if (anchor == NULL) break;
        last = anchor;
#ifdef DEBUG
        dump_pointer("search backward", base, last - base + 1);
        qsort_called++; // loop count
#endif
        for (p2 = last; p2 > base; p2 = p1) {   // search a reverse order pair
            if (compare(*(p1 = p2 - 1), *p2) > 0) {  // found
#ifdef DEBUG
				if (trace_level >= TRACE_MOVE) fprintf(OUT, "%s <--> %s\n", dump_data(*p1), dump_data(*p2));
#endif
				void *tmp = *p1; *p1 = *p2; *(anchor = p2) = tmp;	// swap
            }
        }
        if (anchor == NULL) break;
        base = anchor;
    } while (base < last);
#ifdef DEBUG
    dump_pointer("cocktail_sort() done.", base, nmemb);
#endif
}

void comb_sort(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    dump_pointer("comb_sort() start in " __FILE__, base, nmemb);
#endif
    size_t gap = nmemb * 10 / 13;
    while(TRUE) {
#ifdef DEBUG
		if (trace_level >= TRACE_MOVE) fprintf(OUT, "gap = %ld\n", gap);
#endif
		void **last = base + nmemb;
		bool exchanged;
		do {
			exchanged = FALSE;
			last -= gap;
#ifdef DEBUG
			if (base < last) {
				dump_pointer("search", base, (last - base) + gap);
				qsort_called++; // loop count
			}
#endif
			for (void **p1 = base; p1 < last; p1++) {	// search a reverse order pair
				void **p2 = p1 + gap;
#ifdef DEBUG
				if (trace_level >= TRACE_MOVE) fprintf(OUT, "compare %s to %s", dump_data(*p1), dump_data(*p2));
#endif
				if (compare(*p1, *p2) > 0) {  // found
#ifdef DEBUG
					if (trace_level >= TRACE_MOVE) fprintf(OUT, "\tswap");
#endif
					void *tmp = *p1; *p1 = *p2; *p2 = tmp;	// swap
					exchanged = TRUE;
				}
#ifdef DEBUG
				if (trace_level >= TRACE_MOVE) fprintf(OUT, "\n");
#endif
			}
		} while (exchanged);
		if (gap == 1) break;
		gap = gap * 10 / 13;
    }
#ifdef DEBUG
    dump_pointer("comb_sort() done.", base, nmemb);
#endif
}
