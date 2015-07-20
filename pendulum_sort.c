/*
 * pendulum_sort.c
 *
 *  Another improvement of Cocktail sort.
 *
 *  Created on: 2015/07/20
 *      Author: leo
 */

#include "sort.h"

static size_t   length;

/* pointer sorting */
void pendulum_pointer(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("pendulum_pointer() start in " __FILE__, base, nmemb);
#endif
    void **first = base;
    void *pivot = *first;
    void **last = first + nmemb - 1;
    do {
        void **p1, **p2, **anchor = first + 1;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_pointer("search forward", first, last - first + 1);
#endif
        for (p1 = first; (p2 = p1 + 1) <= last; p1++) { // search a reverse order pair
            if (compare(*p1, *p2) > 0) {    // found
                pivot = *p1; *p1 = *p2;     // save and shift first
                for (p1 = p2; (p2 = p1 + 1) <= last; p1++) {    // search a greater element
                    if (compare(*p2, pivot) > 0) break; // found
                    *p1 = *p2;  // shift first
                }
                *(anchor = p1) = pivot; // restore
            }
        }
        anchor = (last = anchor - 1) + 1;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_pointer("search backward", first, last - first + 1);
#endif
        for (p2 = last; (p1 = p2 - 1) >= first; p2--) { // search a reverse order pair
            if (compare(*p1, *p2) > 0) {    // found
                pivot = *p2; *p2 = *p1;     // save and shift first
                for (p2 = p1; (p1 = p2 - 1) >= first; p2--) {   // search a greater element
                    if (compare(*p1, pivot) < 0) break; // found
                    *p2 = *p1;  // shift first
                }
                *(anchor = p2) = pivot; // restore
            }
        }
        first = anchor + 1;
    } while (first < last);
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("pendulum_pointer() done.", base, nmemb);
#endif
}

static char *swapbuf;
/* exchange 2 elements */
static void swap(void *p1, void *p2)
{
    if (p1 == p2) return;
#ifdef DEBUG
    qsort_moved += 3;
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "swap(%s, %s)\n", dump_data(p1), dump_data(p2));
#endif
    memcpy(swapbuf, p1, length);
    memcpy(p1, p2, length);
    memcpy(p2, swapbuf, length);
}

void pendulum_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
    char tmp_swap[size];
    swapbuf = tmp_swap;
    length = size;
    char *first = base;
    char *last = first + (nmemb - 1) * size;
    do {
        char *pivot, *p1, *p2, *anchor = first + size;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("search forward", first, (last - first) / size + 1, size);
        qsort_called++; // loop count
#endif
        for (p1 = first; (p2 = p1 + size) <= last; p1 += size) {    // search a reverse order pair
            if (compare(p1, p2) > 0) {  // found
#ifdef DEBUG
                if (trace_level >= TRACE_DUMP) fprintf(OUT, "revese order %s --> %s\n", p1, p2);
#endif
                pivot = p1;
                for (p1 = p2; (p2 = p1 + size) <= last; p1 += size) {   // search a greater element
                    if (compare(p2, pivot) > 0) break;  // found
                }
            swap(pivot, anchor = p1);
            }
        }
        // backward
        anchor = (last = anchor - size) - size;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("search backward", first, (last - first) / size + 1, size);
        if (first < last) qsort_called++;   // loop count
#endif
        for (p2 = last; (p1 = p2 - size) >= first; p2 -= size) {    // search a reverse order pair
            if (compare(p1, p2) > 0) {  // found
#ifdef DEBUG
                if (trace_level >= TRACE_DUMP) fprintf(OUT, "revese order %s <-- %s\n", p1, p2);
#endif
                pivot = p2;
                for (p2 = p1; (p1 = p2 - size) >= first; p2 -= size) {  // search a greater element
                    if (compare(p1, pivot) < 0) break;  // found
                }
            swap(anchor = p2, pivot);   // shift first
            }
        }
        first = anchor + size;
    } while (first < last);
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("pendulum_sort() done.", base, nmemb, size);
#endif
}
