/*
 * QMI_sort.c
 *
 *  Hybrid of asymmetric Quicksort, indirect Mergesort and linear Insertion sort.
 *
 *  Created on: 2016/02/12
 *      Author: Takeuchi Leorge <qmisort@gmail.com>
 */
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define     copy(a, b)  memcpy((a), (b), length)

// constants
#define     INSERT      15      // 0 < N <= INSERT : Linear insertion sort
#define     MERGE       1023    // INSERT < N <= MERGE : Indirect Mergesort
                                // MERGE < N : Asymmetric Quicksort

static int      (*comp)(const void *, const void *);
static size_t   length;

/* hybrid Mergesort */
static void hmsort(void **dst, void **src, size_t nmemb) {
    if (nmemb > INSERT) {    // Mergesort
        size_t n_lo = nmemb >> 1, n_hi = nmemb - n_lo;
        hmsort(src, dst, n_lo);
        hmsort(&src[n_lo], &dst[n_lo], n_hi);
        void **left = src, **right = &left[n_lo];
        while (1) {
            if (comp(*left, *right) <= 0) {
                *dst++ = *left++;   // add one
                if (--n_lo == 0) {  // empty?
                    while (n_hi-- > 0) {*dst++ = *right++;}
                    break;
                }
            }
            else {
                *dst++ = *right++;
                if (--n_hi == 0) {
                    while(n_lo--> 0) {*dst++ = *left++;}
                    break;
                }
            }
        }
    }
    else if (nmemb > 1) {   // linear Insertion sort
        void **last = &dst[nmemb - 1];      // point the last
        for (void **hole = dst; ++hole <= last;) {
            void **p1, **p2, *save = *hole; // dig a hole
            for (p2 = hole; (p1 = p2 - 1) >= dst; p2 = p1) {
                if (comp(*p1, save) <= 0) break;
                *p2 = *p1;
            }
            *p2 = save;     // fill the hole with saved pointer
        }
    }
}

/* hybrid Quicksort */
static void hqsort(void *base, size_t nmemb) {
    char *p, pivot[length];
    int chk;    // result of comp()
    size_t  idx;
    while (nmemb > MERGE) {
        char *hole = NULL, *first = (char *)base, *last = first + (nmemb - 1) * length;
        // choose a pivot from logarithmic number of elements
        size_t  pickup = ((size_t)log2(nmemb) >> 1) | 1;    // odd number of elements
        void *index[pickup];
        p = first + (nmemb * rand() / ((size_t)RAND_MAX + 1) / pickup) * length; // 1st pick up point
        size_t  distance = (size_t)(nmemb / pickup) * length;   // distance of elements
        for (idx = 0; idx < pickup; p += distance) index[idx++] = p;    // build an index
        void **left = index, **right = &index[pickup -1], **middle = &index[pickup >> 1];
        while (left < right) {  // search a pointer to the middle element
            void **phole = &left[(right - left) >> 1];  // hole in the index
            char *ppivot = *phole;       // save the middle pointer
            *phole = *right;    // move the last pointer to the middle of index
            phole = right;      // dig a hole at the last of index
            void **plo = left, **phi = right - 1, **peq = NULL;
            for (; plo < phole; plo++) {
                if ((chk = comp(*plo, ppivot)) >= 0) {
                    if (chk > 0) peq = NULL;    // discontinued
                    else if (peq == NULL) peq = phole;
                    *phole = *plo; phole = plo;
                    for (; phi > phole; phi--) {
                        if ((chk = comp(*phi, ppivot)) < 0) {
                            *phole = *phi; phole = phi;
                        }
                        else if (chk > 0) peq = NULL;
                        else if (peq == NULL) peq = phi;
                    }
                }
            }
            *phole = ppivot;     //restore
            if (peq == NULL) peq = phole;   // phole <= peq
            if (middle < phole) right = phole - 1;
            else if (peq < middle) left = peq + 1;
            else break;                     // phole <= middle <= peq
        }
        hole = *middle;     // hole is in the middle of index[]
        // partition
        copy(pivot, hole); copy(hole, last);    // save <-- hole <-- last
        char *lo = first,  *hi = (hole = last) - length, *eq = NULL;
        for (; lo < hole; lo += length) {
            if ((chk = comp(lo, pivot)) >= 0) {
                if (chk > 0) eq = NULL; // discontinued
                else if (eq == NULL) eq = hole;
                copy(hole, lo); hole = lo;
                for (; hi > hole; hi -= length) {
                    if ((chk =comp(hi, pivot)) < 0) {
                        copy(hole, hi); hole = hi;
                    }
                    else if (chk > 0) eq = NULL;
                    else if (eq == NULL) eq = hi;   // first equal element
                }
            }
        }
        if (eq == NULL) eq = hole;
        copy(hole, pivot);  // restore

        // sort sub-arrays iterative and recursive.
        size_t  n_lo = (hole - first) / length; // number of element in lower partition
        size_t  n_hi = (last - eq) / length;
        if (n_lo < n_hi) {
            hqsort(base, n_lo);  // sort shorter sub-array
            base = eq + length; nmemb = n_hi;
        } else {
            hqsort(eq + length, n_hi);
            nmemb = n_lo;
        }
    }
    // indirect merge sort
    if (nmemb > 1) {
        char *first = (char *)base, *last = first + (nmemb - 1) * length;
        void *src[nmemb], *dst[nmemb];
        // make 2 indices
        for (p = first, idx = 0; idx < nmemb; p += length) src[idx++] = p;
        memcpy(dst, src, nmemb * sizeof(void **));  // duplicate

        // sort pointers
        hmsort(dst, src, nmemb);    // hybrid Mergesort

        // reorder array by cyclic permutation
        void **index;
        char *array;
        for (array = first, index = dst; array < last; array += length, index++) {
            void **p1;
            char *p2, *p3;
            if (*(p1 = index) != array) {
                copy(pivot, p2 = array);     // save an element
                do {
                    copy(p2, p3 = *p1);     // copy an element
                    *p1 = p2;               // restore into dst[]
                    p1 = &dst[((p2 = p3) - first) / length];
                } while (*p1 != array);
                copy(*p1 = p3, pivot);       // restore saved element
            }
        }
    }
}

void QMI_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (base == NULL || compare == NULL || size == 0) return;
    length = size; comp = compare;
    hqsort(base, nmemb);        // hybrid Quicksort
}
