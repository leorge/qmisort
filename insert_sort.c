/*
 * insert_linear.c
 *
 *  Several insertion sort.
 *
 *  Created on: 2013/01/08
 *      Author: leo
 */

#include "sort.h"

/* nibble sort is one of the binary insertion sort */

#define DIGIT_WIDTH 4       // nibble
#define MAX_SIZE    (1 << DIGIT_WIDTH)
#define MASK_DIGIT  (MAX_SIZE - 1)
typedef unsigned long long INSERT_INDEX;    // 4 bits/element * 16 elements = 64 bits = bit width of long long

/* binary search */
void insert_binary(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    dump_pointer("insert_binary() start in " __FILE__, base, nmemb);
#endif
    void **hole = base;
    for (size_t idx = 1; idx < nmemb; idx++) {
#ifdef DEBUG
        qsort_called++;
#endif
        void *pivot = *++hole;
        // binary search
        int     chk;
        size_t  pos = 0, lo = 0, hi = idx - 1;
        while (lo <= hi) {
            chk = compare(pivot, base[pos = lo + ((hi - lo) >> 1)]);
            if (chk == 0) break; // found an equal element.
            else if (chk > 0) lo = pos + 1;
            else if (pos == 0) break;
            else hi = pos - 1;      // ck < 0
        }
        if (chk > 0) pos++;
        // store an element
        void **p1, **p2 = hole;;
        for (hi = idx; hi-- > pos; p2 = p1) *p2 = *(p1 = p2 - 1);
        *p2 = pivot;
    }
#ifdef DEBUG
    dump_pointer("bins_pointer() done.", base, nmemb);
#endif
}

/* linear search is the final stage of shell sort */
void insert_linear(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    dump_pointer("insert_sort() start in " __FILE__, base, nmemb);
#endif
    void **last = &base[nmemb - 1];     // point the last element
    for (void **hole = base + 1; hole <= last; hole++) {
#ifdef DEBUG
        qsort_called++;
        dump_pointer("", base, hole - base);
#endif
        void **p1, **p2, *pivot = *hole;    // make a hole
        for (p2 = hole; (p1 = p2 - 1) >= base; p2 = p1) {
            if (compare(*p1, pivot) <= 0) break;
            *p2 = *p1;
        }
        *p2 = pivot;
    }
#ifdef DEBUG
    dump_pointer("insert_linear() done.", base, nmemb);
#endif
}

/* accelerated insertion sort */
void ai_sort(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    qsort_called++;
    dump_pointer("ai_sort() start in " __FILE__, base, nmemb);
#endif
    void **last = &base[nmemb - 1];     // point the last element
    for (void **p1 = base, **p2 = last; p1 < last;) {
        if (compare(*p1, *p2) > 0){
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP)
                fprintf(OUT, "ai_sort() swap %s <--> %s\n", dump_data(*p1), dump_data(*p2));
#endif
            void *tmp = *p1; *p1 = *p2; *p2 = tmp;  // swap
        }
        p1++;
        if (++p2 > last)
            p2 = p1 + ((p2 - p1) >> 1);
    }
    insert_linear(base, nmemb - 1, compare);
}
