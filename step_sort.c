/*
 * step_sort.c
 *
 *  Step_up sort and step up&down sort. Unstable.
 *
 *  Created on: 2015/05/23
 *      Author: leo
 */

#include "sort.h"

/* Bidirectional stepup_sort */
void step_sort(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("step_sort() start in " __FILE__, base, nmemb);
#endif
    char **first = base, **last = first + nmemb - 1, *tmp;
    do {
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_pointer("Step up", first, last - first + 1);
        qsort_called++; // corresponds to recursive calls
#endif
        char **p1, **p2, **pivot;
        // step up
        char **anchor = first + 1;
        for (pivot = p1 = first; p1 < last; p1 = p2) {
            if (compare(*pivot, *(p2 = p1 + 1)) > 0) {
                if (p2 < last) continue; else p1 = p2;
            }
            else if (p1 == pivot) {
                pivot =p2;
                continue;
            }
            tmp = *pivot; *pivot = *p1; *p1 = tmp;  // swap
            anchor = p1; pivot = p2;
        }
        last = anchor - 1;
        // step down
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_pointer("Step down", first, last - first + 1);
        qsort_called++; // corresponds to recursive calls
#endif
        anchor = last - 1;
        for (pivot = p2 = last; p2 > first; p2 = p1) {
            if (compare(*(p1 = p2 - 1), *pivot) > 0) {
                if (p1 > first) continue; else p2 = p1;
            }
            else if (p2 == pivot) {
                pivot = p1;
                continue;
            }
            tmp = *pivot; *pivot = *p2; *p2 = tmp;  // swap
            anchor = p2; pivot = p1;
        }
        first = anchor + 1;
    } while (first < last);
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("step_sort() done.", base, nmemb);
#endif
}

void stepup_sort(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("stepup_sort() start in " __FILE__, base, nmemb);
#endif
    char **first = base, **last = first + nmemb - 1, *tmp;
    do {
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_pointer("Step up", first, last - first + 1);
        qsort_called++; // corresponds to recursive calls
#endif
        char **p1, **p2, **pivot;
        char **anchor = first + 1;
        for (pivot = p1 = first; p1 < last; p1 = p2) {
            if (compare(*pivot, *(p2 = p1 + 1)) > 0) {
                if (p2 < last) continue; else p1 = p2;
            }
            else if (p1 == pivot) {
                pivot =p2;
                continue;
            }
            tmp = *pivot; *pivot = *p1; *p1 = tmp;  // swap
            anchor = p1; pivot = p2;
        }
        last = anchor - 1;
    } while (first < last);
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("stepup_sort() done.", base, nmemb);
#endif
}

