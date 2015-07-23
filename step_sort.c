/*
 * step_sort.c
 *
 *  Step_up sort and step up&down sort. Unstable.
 *
 *  Created on: 2015/05/23
 *      Author: leo
 */

#include "sort.h"

/* pointer sorting of step_sort */
void step_pointer(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("stepup_pointer() start in " __FILE__, base, nmemb);
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
    if (trace_level >= TRACE_DUMP) dump_pointer("step_pointer() done.", base, nmemb);
#endif
}

/* array sorting */

// step up/down sort
void step_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
    char tmp[size+1]; tmp[0] = '\0';
    char *first = base, *last = first + (nmemb - 1) * size;
    do {
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("Step up", first, (last - first) / size + 1, size);
        qsort_called++; // corresponds to recursive calls
#endif
        char *p1, *p2, *pivot, *anchor = first + size;
        for (pivot = p1 = first; p1 < last; p1 = p2) {
            if (compare(pivot, p2 = p1 + size) > 0) {
#ifdef DEBUG
                if (p1 > pivot) { /* searching a greater element */ }
                else if (trace_level >= TRACE_DUMP) // p1 == pivot
                    fprintf(OUT, "revese order (%s , %s)\n", dump_data(p1), dump_data(p2));
#endif
                if (p2 < last) continue;
                else p1 = p2;   // the last element is smaller than the pivot
            }
            else if (p1 == pivot) { // increasing
                pivot = p2; // step up
                continue;
            }
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "swap %s <--> %s\n", dump_data(pivot), dump_data(p1));
            qsort_moved += 3;
#endif
            memcpy(tmp, pivot, size);
            memcpy(pivot, p1, size);
            memcpy(anchor = p1, tmp, size);
            pivot = p2; // step up
        }
        last = anchor - size;   // array[first..last] are not sorted
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("Step down", first, (last - first) / size + 1, size);
        qsort_called++; // corresponds to recursive calls
#endif
        anchor = last - size;
        for (pivot = p2 = last; p2 > first; p2 = p1) {
            if (compare(p1 = p2 - size, pivot) > 0) {
#ifdef DEBUG
                if (p2 < pivot) { /* searching a lesser element */ }
                else if (trace_level >= TRACE_DUMP) // p1 == pivot
                    fprintf(OUT, "revese order (%s , %s)\n", dump_data(p1), dump_data(p2));
#endif
                if (p1 > first) continue;
                else p2 = p1;   // the last element is greater than the pivot
            }
            else if (p2 == pivot) { // decreasing
                pivot = p1; // step down
                continue;
            }
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "swap %s <--> %s\n", dump_data(pivot), dump_data(p2));
            qsort_moved += 3;
#endif
            memcpy(tmp, pivot, size);
            memcpy(pivot, p2, size);
            memcpy(anchor = p2, tmp, size);
            pivot = p1; // step down
        }
        first = anchor + size;   // array[first..last] are not sorted
    } while (first < last);
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("step_sort() done.", base, nmemb, size);
#endif
}

// Improved bubble sort to reduce swaps.
void stepup_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
    char tmp[size+1]; tmp[0] = '\0';
#define first   ((char *)base)
    char *last = first + (nmemb - 1) * size;
    do {
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("search", first, (last - first) / size + 1, size);
        qsort_called++; // corresponds to recursive calls
#endif
        char *p1, *p2, *pivot, *anchor = first + size;
        for (pivot = p1 = first; p1 < last; p1 = p2) {
            if (compare(pivot, p2 = p1 + size) > 0) {
#ifdef DEBUG
                if (p1 > pivot) { /* searching a greater element */ }
                else if (trace_level >= TRACE_DUMP) // p1 == pivot
                    fprintf(OUT, "revese order (%s , %s)\n", dump_data(p1), dump_data(p2));
#endif
                if (p2 < last) continue;
                else p1 = p2;   // the last element is smaller than the pivot
            }
            else if (p1 == pivot) { // increasing
                pivot = p2; // step up
                continue;
            }
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "swap %s <--> %s\n", dump_data(pivot), dump_data(p1));
            qsort_moved += 3;
#endif
            memcpy(tmp, pivot, size);
            memcpy(pivot, p1, size);
            memcpy(anchor = p1, tmp, size);
            pivot = p2; // step up
        }
        last = anchor - size;   // array[first..last] are not sorted
    } while (first < last);
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("stepup_sort() done.", base, nmemb, size);
#endif
}
