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
                void *tmp = *p1; *(anchor = p1) = *p2; *p2 = tmp;   // swap
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
                void *tmp = *p1; *(anchor = p1) = *p2; *p2 = tmp;   // swap
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
                void *tmp = *p1; *p1 = *p2; *(anchor = p2) = tmp;   // swap
            }
        }
        if (anchor == NULL) break;
        base = anchor;
    } while (base < last);
#ifdef DEBUG
    dump_pointer("cocktail_sort() done.", base, nmemb);
#endif
}

void rabbit_sort(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    dump_pointer("rabbit_sort() start in " __FILE__, base, nmemb);
#endif
    char **first = (char **)base, **last = first + nmemb - 1;
    do {
#ifdef DEBUG
        dump_pointer("Search a greater rabbit.", (void **)first, last - first + 1);
        qsort_called++; // corresponds to recursive calls
#endif
        char **rabbit, *tmp;
        char **hole = NULL;     // a rabbit hole
        char **anchor = NULL;   // exchanged position
        for (char **p = rabbit = first; p++ < last;) {
            int chk = compare(*p, *rabbit); // compare the size
            if (chk < 0) hole = p;  // a smaller rabbit is in a hole.
            else {  // found a not smaller rabbit
                if (hole != NULL) {
#ifdef DEBUG
                    if (trace_level >= TRACE_MOVE)
                        fprintf(OUT, "%s <--> %s\n", dump_data(*rabbit), dump_data(*hole));
#endif
                    tmp = *hole; *hole = *rabbit; *rabbit = tmp;  // push out a smaller rabbit.
                    anchor = hole; hole = NULL; rabbit = p;
                }
                else if (chk > 0) rabbit = p;   // a bigger rabbit.
            }
        }
        if (hole != NULL) { // The biggest rabbit is roaming.
            assert(hole == last);
#ifdef DEBUG
            if (trace_level >= TRACE_MOVE)
                fprintf(OUT, "%s <--> %s\n", dump_data(*rabbit), dump_data(*hole));
#endif
            tmp = *hole; *hole = *rabbit; *rabbit = tmp;
            anchor = hole; hole = NULL;
        }
        else if (anchor == NULL) break; // no changed
        last = anchor - 1;
#ifdef DEBUG
        dump_pointer("Search a lesser  rabbit.", (void **)first, last - first + 1);
        qsort_called++; // corresponds to recursive calls
#endif
        for (char **p = rabbit = last; p-- > first;) {
            int chk = compare(*p, *rabbit);
            if (chk > 0) hole = p;
            else {
                if (hole != NULL) {
#ifdef DEBUG
                    if (trace_level >= TRACE_MOVE)
                        fprintf(OUT, "%s <--> %s\n", dump_data(*rabbit), dump_data(*hole));
#endif
                    tmp = *hole; *hole = *rabbit; *rabbit = tmp;
                    anchor = hole; hole = NULL; rabbit = p;
                }
                else if (chk < 0) rabbit = p;
            }
        }
        if (hole != NULL) {
            assert(hole == first);
#ifdef DEBUG
            if (trace_level >= TRACE_MOVE)
                fprintf(OUT, "%s <--> %s\n", dump_data(*rabbit), dump_data(*hole));
#endif
            tmp = *hole; *hole = *rabbit; *rabbit = tmp;
            anchor = hole;
        }
        else if (anchor == NULL) break; // no changed
        first = anchor + 1;
    } while (first < last);
#ifdef DEBUG
    dump_pointer("rabbit_sort() done.", base, nmemb);
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
        if (trace_level >= TRACE_MOVE) fprintf(OUT, "gap = %s\n", dump_size_t(gap));
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
            for (void **p1 = base; p1 < last; p1++) {   // search a reverse order pair
                void **p2 = p1 + gap;
#ifdef DEBUG
                if (trace_level >= TRACE_MOVE) fprintf(OUT, "compare %s to %s", dump_data(*p1), dump_data(*p2));
#endif
                if (compare(*p1, *p2) > 0) {  // found
#ifdef DEBUG
                    if (trace_level >= TRACE_MOVE) fprintf(OUT, "\tswap");
#endif
                    void *tmp = *p1; *p1 = *p2; *p2 = tmp;  // swap
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

/* shellsort */
void shell_sort(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    dump_pointer("shell_sort() start in " __FILE__, base, nmemb);
    if (trace_level >= TRACE_DUMP) {
        fprintf(OUT, "gaplist =");
        for (int gap = 0; gap < gap_count; gap++) fprintf(OUT, " %s", dump_size_t(gaplist[gap]));
        fprintf(OUT, "\n");
    }
#endif
    void **last = &base[nmemb - 1];     // point the last element
    size_t  *gap_pointer = gaplist;
    for (int i = 0; i < gap_count; i++) {
        size_t  gap = *gap_pointer++;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) {
            fprintf(OUT, "gap = %s", dump_size_t(gap));
            dump_pointer("", base, nmemb);
        }
#endif
        for (void **hole = base + gap; hole <= last; hole++) {
#ifdef DEBUG
            qsort_called++;
#endif
            void **p1, **p2, *pivot = *hole;    // make a hole
            for (p2 = hole; (p1 = p2 - gap) >= base; p2 = p1) {
                if (compare(*p1, pivot) <= 0) break;
                *p2 = *p1;
            }
            *p2 = pivot;
        }
    }
#ifdef DEBUG
    dump_pointer("shell_sort() done.", base, nmemb);
#endif
}
