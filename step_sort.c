/*
 * step_sort.c
 *
 *  Step_up sort and step up&down sort. Unstable.
 *
 *  Created on: 2015/05/23
 *      Author: leo
 */

#include "sort.h"

/* pointer sorting  */
void step_pointer(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("stepup_pointer() start in " __FILE__, base, nmemb);
#endif
    void **first = base;
    void **last = first + nmemb - 1;
    do {
        void **p1, **p2, **anchor = first + 1;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_pointer("stepup_pointer()", base, last - first + 1);
#endif
        for (p1 = first; p1 < last; p1++) {
            if (compare(*p1, *(p2 = p1 + 1)) > 0) {
                void *pivot = *p1;
                for (p1 = p2; (p2 = p1 + 1) <= last; p1++) {
                    if (compare(*p2, pivot) > 0) break;
                    *p1 = *p2;
                }
                *(anchor = p1) = pivot;
            }
        }
        last = anchor - 1;
    } while (first < last);
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("stepup_pointer() done.", base, nmemb);
#endif
}

/* array sorting */

// Bidirectional stepup_sort() below.
void step_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
    char pivot[size+1]; pivot[0] = '\0';
	char *first = base, *last = first + (nmemb - 1) * size;
    do {
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("search forward", first, (last - first) / size + 1, size);
        qsort_called += 2;  // corresponds to recursive calls
#endif
        char *p1, *p2, *anchor = first + size;
        for (p1 = first; p1 < last; p1 += size) {    // search a reverse order pair
            if (compare(p1, p2 = p1 + size) > 0) {  // found
                memcpy(pivot, p1, size);
                while ((p2 += size) <= last) {    // search a more greater element
                    if (compare(p2, pivot) > 0) break;  // found
                }
				memcpy(p1, p2 -= size, size);
#ifdef DEBUG
				qsort_moved += 3;
				if (trace_level >= TRACE_DUMP) fprintf(OUT, "%s <--> %s\n", dump_data(pivot), dump_data(p1));
#endif
				memcpy(anchor = p1 = p2, pivot, size);
            }
        }   // array[anchor..last] are sorted
        last = anchor - size;   // array[first..last] are not sorted
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("search backward", first, (last - first) / size + 1, size);
        qsort_called += 2;  // corresponds to recursive calls
#endif
        for (p2 = last; p2 < first; p2 -= size) {   // search a reverse order pair
            if (compare(p1 = p2 - size, p2) > 0) {  // found
                memcpy(pivot, p2, size);
                while ((p1 -= size) >= first) {     // search a more lesser element
                    if (compare(p1, pivot) < 0) break;  // found
                }
				memcpy(p2, p1 += size, size);
#ifdef DEBUG
				qsort_moved += 3;
				if (trace_level >= TRACE_DUMP) fprintf(OUT, "%s <--> %s\n", dump_data(pivot), dump_data(p2));
#endif
				memcpy(anchor = p1 = p2, pivot, size);
            }
        }   // array[anchor..last] are sorted
        first = anchor + size;  // array[first..last] are not sorted
    } while (first < last);
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("stepup_sort() done.", base, nmemb, size);
#endif
}

// Improved bubble sort to reduce swaps.
void stepup_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
    char pivot[size+1]; pivot[0] = '\0';
#define first   ((char *)base)
    char *last = first + (nmemb - 1) * size;
    do {
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("search", first, (last - first) / size + 1, size);
        qsort_called++; // corresponds to recursive calls
#endif
        char *p1, *p2, *anchor = first + size;
        for (p1 = first; p1 < last; p1 += size) {    // search a reverse order pair
            if (compare(p1, p2 = p1 + size) > 0) {  // found
#ifdef DEBUG
                if (trace_level >= TRACE_DUMP) fprintf(OUT, "revese order %s , %s\n", dump_data(p1), dump_data(p2));
#endif
                memcpy(pivot, p1, size);
                while ((p2 += size) <= last) {    // search a more greater element
                    if (compare(p2, pivot) > 0) {
#ifdef DEBUG
                    	if (trace_level >= TRACE_DUMP) fprintf(OUT, "%s is greater than %s\n", dump_data(p2), dump_data(pivot));
#endif
                    	break;  // found
                    }
                }
				memcpy(p1, p2 -= size, size);
#ifdef DEBUG
				qsort_moved += 3;
				if (trace_level >= TRACE_DUMP) fprintf(OUT, "%s <--> %s\n", dump_data(pivot), dump_data(p1));
#endif
				memcpy(anchor = p1 = p2, pivot, size);
            }
        }   // array[anchor..last] are sorted
        last = anchor - size;   // array[first..last] are not sorted
    } while (first < last);
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) dump_array("stepup_sort() done.", base, nmemb, size);
#endif
}
