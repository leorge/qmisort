/*
 * pivot_hole.c : modified quick_hole.c.
 *
 *  The simplest new quicksort using a pivot hole.
 *
 *  Created on: 2016/09/20
 *      Author: Takeuchi Leorge <qmisort@gmail.com>
 */
#include    <string.h>

static int      (*comp)(const void *, const void *);
static size_t   length;

static void sort(void *base, size_t nmemb) {
    if (nmemb > 1) {
#define copy(a,b)   memcpy((a), (b), length)
#define first (char *)base
        char *last = first + (nmemb - 1) * length;  // point the last element
        char pivot[length], *hole; copy(pivot, hole = last);    // save the last element as a pivot
        char *lo = first, *hi = last - length;  // search pointers
        for (; lo < hole; lo += length) {       // outer loop
            if (comp(lo, pivot) > 0) {
                copy(hole, lo); hole = lo;  // move a hole to lo.
                for (; hi > hole; hi -= length) {   // inner loop, symmetric to the outer loop
                    if (comp(hi, pivot) < 0) {      // symmetric comparison
                        copy(hole, hi); hole = hi;  // move a hole to hi
                    }
                }
            }
        }
        copy(hole, pivot);  // restore
        sort(first, (hole - first) / length);
        sort(hole + length, (last - hole) / length);
    }
}


void pivot_hole(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    length = size; comp = compare;
    sort(base, nmemb);
}
