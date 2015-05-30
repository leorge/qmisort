/*
 * cocktail_sort.c
 *
 *  Cocktail [shaker] sort, or bidirectional bubble sort without swaps.
 *
 *  Created on: 2015/05/23
 *      Author: leo
 */

#include "sort.h"

static size_t   length;

/* pointer sorting */
void cocktail_pointer(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("cocktail_pointer() start in " __FILE__, base, nmemb);
#endif
    void *pivot;
    void **first = base;
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
    if (trace_level >= TRACE_DUMP) dump_pointer("cocktail_pointer() done.", base, nmemb);
#endif
}

/* array sorting */
static void copy(void *dst, const void *src)
{
#ifdef  DEBUG
    qsort_moved++;
    if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(dst = %s, src = %s)\n", dump_data(dst), dump_data(src));
#endif
    memcpy(dst, src, length); /* restore an elements  */
}

void cocktail_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
    char pivot[size];
#ifdef DEBUG
    *pivot = 0;
#endif
    length = size;
    char *first = base;
    char *last = first + (nmemb - 1) * size;
    do {
        char *p1, *p2, *anchor = first + size;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("search forward", first, (last - first) / size + 1, size);
        qsort_called++; // loop count
#endif
        for (p1 = first; (p2 = p1 + size) <= last; p1 += size) {    // search a reverse order pair
            if (compare(p1, p2) > 0) {  // found
#ifdef DEBUG
                if (trace_level >= TRACE_DUMP) fprintf(OUT, "revese order %s --> %s\n", p1, p2);
#endif
                copy(pivot, p1); copy(p1, p2);      // save and shift first
                for (p1 = p2; (p2 = p1 + size) <= last; p1 += size) {   // search a greater element
                    if (compare(p2, pivot) > 0) break;  // found
                    copy(p1, p2);   // shift first
                }
                copy(anchor = p1, pivot);   // restore
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
                copy(pivot, p2); copy(p2, p1);      // save and shift first
                for (p2 = p1; (p1 = p2 - size) >= first; p2 -= size) {  // search a greater element
                    if (compare(p1, pivot) < 0) break;  // found
                    copy(p2, p1);   // shift first
                }
                copy(anchor = p2, pivot);   // restore
            }
        }
        first = anchor + size;
    } while (first < last);
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("cocktail_sort() done.", base, nmemb, size);
#endif
}
