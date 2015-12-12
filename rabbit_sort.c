/*
 * rabbit_sort.c
 *
 *  Rabbit sort is variant of bi-directional bubble sort. Unstable.
 *
 *  Created on: 2015/05/23
 *      Author: leo
 */

#include "sort.h"

void rabbit_sort(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    dump_pointer("rabbit_sort() start in " __FILE__, base, nmemb);
#endif
    char **first = base, **last = first + nmemb - 1;
    do {
#ifdef DEBUG
        dump_pointer("Search a larger  rabbit.", first, last - first + 1);
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
                    tmp = *hole; *hole = *rabbit; *rabbit = tmp;  // push out a smaller rabbit.
                    anchor = hole; hole = NULL; rabbit = p;
                }
                else if (chk > 0) rabbit = p;   // a bigger rabbit.
            }
        }
        if (hole != NULL) { // The biggest rabbit is roaming.
            assert(hole == last);
            tmp = *hole; *hole = *rabbit; *rabbit = tmp;
            anchor = hole; hole = NULL;
        }
        else if (anchor == NULL) break; // no changed
        last = anchor - 1;
#ifdef DEBUG
        dump_pointer("Search a smaller rabbit.", first, last - first + 1);
        qsort_called++; // corresponds to recursive calls
#endif
        for (char **p = rabbit = last; p-- > first;) {
            int chk = compare(*p, *rabbit);
            if (chk > 0) hole = p;
            else {
                if (hole != NULL) {
                    tmp = *hole; *hole = *rabbit; *rabbit = tmp;
                    anchor = hole; hole = NULL; rabbit = p;
                }
                else if (chk < 0) rabbit = p;
            }
        }
        if (hole != NULL) {
            assert(hole == first);
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
