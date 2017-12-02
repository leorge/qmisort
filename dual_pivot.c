/*
 * http://grepcode.com/file/repository.grepcode.com/java/root/jdk/openjdk/7-b147/java/util/DualPivotQuicksort.java
 *
 * Copyright (c) 2009, 2011, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#include "sort.h"

static int   (*comp)(const void *, const void *);
static int   length;    // unsigned is inhibited in Java

static void copy(void *dst, const void *src)
{
#ifdef  DEBUG
    dump_copy(dst, src);
#endif
    memcpy(dst, src, length); /* restore an elements  */
}

static void qi_sort(char *a, int left, int right, bool leftmost); // declaration

//package java.util;

/**
 * This class implements the Dual-Pivot Quicksort algorithm by
 * Vladimir Yaroslavskiy, Jon Bentley, and Josh Bloch. The algorithm
 * offers O(n log(n)) performance on many data sets that cause other
 * quicksorts to degrade to quadratic performance, and is typically
 * faster than traditional (one-pivot) Quicksort implementations.
 *
 * @author Vladimir Yaroslavskiy
 * @author Jon Bentley
 * @author Josh Bloch
 *
 * @version 2011.02.11 m765.827.12i:5\7pm
 * @since 1.7
 */
//final class DualPivotQuicksort {

/**
 * Prevents instantiation.
 */
//    private DualPivotQuicksort() {}

/*
 * Tuning parameters.
 */

/**
 * The maximum number of runs in merge sort.
 */
#define MAX_RUN_COUNT   67

/**
 * The maximum length of run in merge sort.
 */
#define MAX_RUN_LENGTH  33

/**
 * If the length of an array to be sorted is less than this
 * constant, Quicksort is used in preference to merge sort.
 */
#define QUICKSORT_THRESHOLD 286

/**
 * If the length of an array to be sorted is less than this
 * constant, insertion sort is used in preference to Quicksort.
 */
#define INSERTION_SORT_THRESHOLD    47

/**
 * If the length of a byte array to be sorted is greater than this
 * constant, counting sort is used in preference to insertion sort.
 */
//    private static final int COUNTING_SORT_THRESHOLD_FOR_BYTE = 29;

/**
 * If the length of a short or char array to be sorted is greater
 * than this constant, counting sort is used in preference to Quicksort.
 */
//    private static final int COUNTING_SORT_THRESHOLD_FOR_SHORT_OR_CHAR = 3200;

/*
 * Sorting methods for seven primitive types.
 */

/**
 * Sorts the specified array.
 *
 * @param a the array to be sorted
 */
//public static void sort(int[] a) {
//      sort(a, 0, a.length - 1);
//}

/**
 * Sorts the specified range of the array.
 *
 * @param a the array to be sorted
 * @param left the index of the first element, inclusive, to be sorted
 * @param right the index of the last element, inclusive, to be sorted
 */
static void sort(char *a, int left, int right) {
    // Use Quicksort on small arrays
#ifdef DEBUG
    char *base = a;
//    qsort_called++;
    int N = right - left + 1;
    if (N > 1 ) dump_array("sort() start in " __FILE__, a, N, 0, 0, length);
#endif
    if (right - left < QUICKSORT_THRESHOLD) {
        qi_sort(a, left, right, TRUE);
        return;
    }

    /*
     * Index run[i] is the start of i-th run
     * (ascending or descending sequence).
     */
    int *run = calloc(sizeof(int), MAX_RUN_COUNT + 1);
    int count = 0; run[0] = left;

    // Check if the array is nearly sorted
    for (int k = left; k < right; run[count] = k) {
        if (comp(&a[k * length], &a[(k + 1) * length]) < 0) { // ascending
            while (++k <= right && comp(&a[(k - 1) * length], &a[k * length]) <= 0);
        } else if (comp(&a[k * length], &a[(k + 1) * length]) > 0) { // descending
            while (++k <= right && comp(&a[(k - 1) * length], &a[k * length]) >= 0);
            for (int lo = run[count] - 1, hi = k; ++lo < --hi; ) {
                char t[length]; copy(t, &a[lo * length]); copy(&a[lo * length], &a[hi * length]); copy(&a[hi * length], t);
            }
        } else { // equal
            for (int m = MAX_RUN_LENGTH; ++k <= right && comp(&a[(k - 1) * length], &a[k * length]) == 0; ) {
                if (--m == 0) {
                    qi_sort(a, left, right, TRUE);
                    return;
                }
            }
        }

        /*
         * The array is not highly structured,
         * use Quicksort instead of merge sort.
         */
        if (++count == MAX_RUN_COUNT) {
            qi_sort(a, left, right, TRUE);
            return;
        }
    }

    // Check special cases
    if (run[count] == right++) { // The last run contains one element
        run[++count] = right;
    } else if (count == 1) { // The array is already sorted
        return;
    }

    /*
     * Create temporary array, which is used for merging.
     * Implementation note: variable "right" is increased by 1.
     */
    char *b; byte odd = 0;
    for (int n = 1; (n <<= 1) < count; odd ^= 1);

    if (odd == 0) { // Block copy with memcpy() is better. However, this case doesn't use quicksort.
        b = a; a = calloc(sizeof(char *), right - left + 1);
        for (int i = left - 1; ++i < right;) copy(&a[i * length], &b[i * length]);
    } else {
        b = calloc(sizeof(char *), right - left + 1);
    }

    // Merging
    for (int last; count > 1; count = last) {
        for (int k = (last = 0) + 2; k <= count; k += 2) {
            int hi = run[k], mi = run[(k - 1)];
            for (int i = run[k - 2], p = i, q = mi; i < hi; ++i) {
                if (q >= hi || (p < mi && (comp(&a[p * length], &a[q * length]) <= 0))) {
                    copy(&b[i * length], &a[p++ * length]);
                } else {
                    copy(&b[i * length], &a[q++ * length]);
                }
            }
            run[++last] = hi;
        }
        if ((count & 1) != 0) {
            for (int i = right, lo = run[count - 1]; --i >= lo;)
                copy(&b[i * length], &a[i * length]);
            run[++last] = right;
        }
        char *t; t = a; a = b; b = t;
    }
#ifdef DEBUG
    dump_array("qi_sort() merge done.", base, N, 0, 0, length);
#endif
    return;
}

/**
 * Sorts the specified range of the array by Dual-Pivot Quicksort.
 *
 * @param a the array to be sorted
 * @param left the index of the first element, inclusive, to be sorted
 * @param right the index of the last element, inclusive, to be sorted
 * @param leftmost indicates if this part is the leftmost in the range
 */
static void qi_sort(char *a, int left, int right, bool leftmost) {
    int N = right - left + 1;
#ifdef DEBUG
//    qsort_called++;
    char *base = a, *head = a + left * length;
    if (N > 1) dump_array("qi_sort() start in " __FILE__, head, N, 0, 0, length);
#endif

    // Use insertion sort on tiny arrays
    if (N < INSERTION_SORT_THRESHOLD) {
        if (leftmost) {
            /*
             * Traditional (without sentinel) insertion sort,
             * optimized for server VM, is used in case of
             * the leftmost part.
             */
            for (int i = left, j = i; i < right; j = ++i) {
                char ai[length]; copy(ai, &a[(i + 1) * length]);
                while (comp(ai, &a[j * length]) < 0) {
                    copy(&a[(j + 1) * length], &a[j * length]);
                    if (j-- == left) {
                        break;
                    }
                }
                copy(&a[(j + 1) * length], ai);
            }
        } else {
            /*
             * Skip the longest ascending sequence.
             */
            do {
                if (left >= right) {
#ifdef DEBUG
                    dump_array("qi_sort() insertion sort done.", base, N, 0, 0, length);
#endif
                    return;
                }
                left++;
            } while (comp(&a[left * length], &a[(left - 1) * length]) >= 0);
            /*
             * Every element from adjoining part plays the role
             * of sentinel, therefore this allows us to avoid the
             * left range check on each iteration. Moreover, we use
             * the more optimized algorithm, so called pair insertion
             * sort, which is faster (in the context of Quicksort)
             * than traditional implementation of insertion sort.
             */
            for (int k = left; ++left <= right; k = ++left) {
                char a1[length], a2[length]; copy(a1, &a[k * length]); copy(a2, &a[left * length]);

                if (comp(a1, a2) < 0) {
                    copy(a2 , a1); copy(a1, &a[left * length]);
                }
                while (comp(a1, &a[--k * length]) < 0) {
                    copy(&a[(k + 2) * length], &a[k * length]);
                }
                copy(&a[(++k + 1) * length], a1);

                while (comp(a2, &a[--k * length]) < 0) {
                    copy(&a[(k + 1) * length],&a[k * length]);
                }
                copy(&a[(k + 1) * length], a2);
            }
            char last[length]; copy(last, &a[right * length]);

            while (comp(last, &a[(--right) * length]) < 0) {
                copy(&a[(right + 1) * length], &a[right * length]);
            }
            copy(&a[(right + 1) * length], last);
        }
#ifdef DEBUG
            dump_array("qi_sort() pair insertion sort done.", head, N, 0, 0, length);
#endif
            return;
    }

    // Inexpensive approximation of N / 7
    int seventh = (N >> 3) + (N >> 6) + 1;

    /*
     * Sort five evenly spaced elements around (and including) the
     * center element in the range. These elements will be used for
     * pivot selection as described below. The choice for spacing
     * these elements was empirically determined to work well on
     * a wide variety of inputs.
     */
    int e3 = left + ((right - left) >> 1); // The midpoint
    int e2 = e3 - seventh;
    int e1 = e2 - seventh;
    int e4 = e3 + seventh;
    int e5 = e4 + seventh;
    char t[length]; // temporary data

    // Sort these elements using insertion sort
    if (comp(&a[e2 * length], &a[e1 * length]) < 0) { copy(t, &a[e2 * length]); copy(&a[e2 * length], &a[e1 * length]); copy(&a[e1 * length], t); }

    if (comp(&a[e3 * length], &a[e2 * length]) < 0) { copy(t, &a[e3 * length]); copy(&a[e3 * length], &a[e2 * length]); copy(&a[e2 * length], t);
        if (comp(t, &a[e1 * length]) < 0) { copy(&a[e2 * length], &a[e1 * length]); copy(&a[e1 * length], t); }
    }
    if (comp(&a[e4 * length], &a[e3 * length]) < 0) { copy(t, &a[e4 * length]); copy(&a[e4 * length], &a[e3 * length]); copy(&a[e3 * length] ,t);
        if (comp(t, &a[e2 * length]) < 0) { copy(&a[e3 * length], &a[e2 * length]); copy(&a[e2 * length], t);
            if (comp(t, &a[e1 * length]) < 0) { copy(&a[e2 * length], &a[e1 * length]); copy(&a[e1 * length], t); }
        }
    }
    if (comp(&a[e5 * length], &a[e4 * length]) < 0) { copy(t, &a[e5 * length]); copy(&a[e5 * length], &a[e4 * length]); copy(&a[e4 * length], t);
        if (comp(t, &a[e3 * length]) < 0) { copy(&a[e4 * length], &a[e3 * length]); copy(&a[e3 * length], t);
            if (comp(t, &a[e2 * length]) < 0) { copy(&a[e3 * length], &a[e2 * length]); copy(&a[e2 * length], t);
                if (comp(t, &a[e1 * length]) < 0) { copy(&a[e2 * length], &a[e1 * length]); copy(&a[e1 * length], t); }
            }
        }
    }

    // Pointers
    int less  = left;  // The index of the first element of center part
    int great = right; // The index before the first element of right part

    if (comp(&a[e1 * length], &a[e2 * length]) && comp(&a[e2 * length], &a[e3 * length]) && comp(&a[e3 * length], &a[e4 * length]) && comp(&a[e4 * length], &a[e5 * length])) {

        qsort_called++;

        /*
         * Use the second and fourth of the five sorted elements as pivots.
         * These values are inexpensive approximations of the first and
         * second terciles of the array. Note that pivot1 <= pivot2.
         */
        char pivot1[length]; copy(pivot1, &a[e2 * length]);
        char pivot2[length]; copy(pivot2, &a[e4 * length]);

        /*
         * The first and the last elements to be sorted are moved to the
         * locations formerly occupied by the pivots. When partitioning
         * is complete, the pivots are swapped back into their final
         * positions, and excluded from subsequent sorting.
         */
        copy(&a[e2 * length], &a[left  * length]);
        copy(&a[e4 * length], &a[right * length]);

        /*
         * Skip elements, which are less or greater than pivot values.
         */
        while (comp(&a[++less * length], pivot1) < 0);
        while (comp(&a[--great* length], pivot2) > 0);

        /*
         * Partitioning:
         *
         *   left part           center part                   right part
         * +--------------------------------------------------------------+
         * |  < pivot1  |  pivot1 <= && <= pivot2  |    ?    |  > pivot2  |
         * +--------------------------------------------------------------+
         *               ^                          ^       ^
         *               |                          |       |
         *              less                        k     great
         *
         * Invariants:
         *
         *              all in (left, less)   < pivot1
         *    pivot1 <= all in [less, k)     <= pivot2
         *              all in (great, right) > pivot2
         *
         * Pointer k is the first index of ?-part.
         */
        for (int k = less - 1; ++k <= great; ) {
            char ak[length]; copy(ak, &a[k * length]);
            if (comp(ak, pivot1) < 0) { // Move a[k * length] to left part
                copy(&a[k * length], &a[less * length]);
                /*
                 * Here and below we use "a[i] = b; i++;" instead
                 * of "a[i++] = b;" due to performance issue.
                 */
                copy(&a[less * length], ak);
                ++less;
            } else if (comp(ak, pivot2) > 0) { // Move a[k * length] to right part
                while (comp(&a[great * length], pivot2) > 0) {
                    if (great-- == k) {
                        goto outer1;    // break the nest of loop
                    }
                }
                if (comp(&a[great * length], pivot1) < 0) { // a[great * length] <= pivot2
                    copy(&a[k * length], &a[less * length]);
                    copy(&a[less * length], &a[great * length]);
                    ++less;
                } else { // pivot1 <= a[great * length] <= pivot2
                    copy(&a[k * length], &a[great * length]);
                }
                /*
                 * Here and below we use "a[i] = b; i--;" instead
                 * of "a[i--] = b;" due to performance issue.
                 *
                 */
                copy(&a[great * length], ak);
                --great;
            }
        }
outer1:

        // Swap pivots into their final positions
        copy(&a[left  * length], &a[(less  - 1) * length]); copy(&a[(less  - 1) * length], pivot1);
        copy(&a[right * length], &a[(great + 1) * length]); copy(&a[(great + 1) * length], pivot2);

        // Sort left and right parts recursively, excluding known pivots
        qi_sort(a, left, less - 2, leftmost);
        qi_sort(a, great + 2, right, FALSE);

        /*
         * If center part is too large (comprises > 4/7 of the array),
         * swap internal pivot values to ends.
         */
        if (less < e1 && e5 < great) {
            /*
             * Skip elements, which are equal to pivot values.
             */
            while (comp(&a[less * length], pivot1) == 0) {
                ++less;
            }

            while (comp(&a[great * length], pivot2) == 0) {
                --great;
            }

            /*
             * Partitioning:
             *
             *   left part         center part                  right part
             * +----------------------------------------------------------+
             * | == pivot1 |  pivot1 < && < pivot2  |    ?    | == pivot2 |
             * +----------------------------------------------------------+
             *              ^                        ^       ^
             *              |                        |       |
             *             less                      k     great
             *
             * Invariants:
             *
             *              all in (*,  less) == pivot1
             *     pivot1 < all in [less,  k)  < pivot2
             *              all in (great, *) == pivot2
             *
             * Pointer k is the first index of ?-part.
             */
            for (int k = less - 1; ++k <= great; ) {
                char ak[length]; copy(ak, &a[k * length]);
                if (comp(ak, pivot1) == 0) { // Move a[k * length] to left part
                    copy(&a[k * length], &a[less * length]);
                    copy(&a[less * length], ak);
                    ++less;
                } else if (comp(ak, pivot2) == 0) { // Move a[k * length] to right part
                    while (comp(&a[great * length], pivot2) == 0) {
                        if (great-- == k) {
                            goto outer2;    // break the nest of loop
                        }
                    }
                    if (comp(&a[great * length], pivot1) == 0) { // a[great * length] < pivot2
                        copy(&a[k * length], &a[less * length]);
                        /*
                         * Even though a[great] equals to pivot1, the
                         * assignment a[less] = pivot1 may be incorrect,
                         * if a[great] and pivot1 are floating-point zeros
                         * of different signs. Therefore in float and
                         * double sorting methods we have to use more
                         * accurate assignment a[less] = a[great].
                         */
                        copy(&a[less * length], pivot1);
                        ++less;
                    } else { // pivot1 < a[great * length] < pivot2
                        copy(&a[k * length], &a[great * length]);
                    }
                    copy(&a[great * length], ak);
                    --great;
                }
            }
        }
outer2:

        // Sort center part recursively
        qi_sort(a, less, great, FALSE);

    } else { // Partitioning with one pivot
        /*
         * Use the third of the five sorted elements as pivot.
         * This value is inexpensive approximation of the median.
         */
        char pivot[length]; copy(pivot, &a[e3 * length]);

        /*
         * Partitioning degenerates to the traditional 3-way
         * (or "Dutch National Flag") schema:
         *
         *   left part    center part              right part
         * +-------------------------------------------------+
         * |  < pivot  |   == pivot   |     ?    |  > pivot  |
         * +-------------------------------------------------+
         *              ^              ^        ^
         *              |              |        |
         *             less            k      great
         *
         * Invariants:
         *
         *   all in (left, less)   < pivot
         *   all in [less, k)     == pivot
         *   all in (great, right) > pivot
         *
         * Pointer k is the first index of ?-part.
         */
        for (int k = less; k <= great; ++k) {
            if (comp(&a[k * length], pivot) == 0) {
                continue;
            }
            char ak[length]; copy(ak, &a[k * length]);
            if (comp(ak, pivot) < 0) { // Move a[k * length] to left part
                copy(&a[k * length], &a[less * length]);
                copy(&a[less * length], ak);
                ++less;
            } else { // a[k * length] > pivot - Move a[k * length] to right part
                while (comp(&a[great * length], pivot) > 0) {
                    --great;
                }
                if (k > great) break;   // Debugged. Confirm with the output of KillDualPivot.pl
                if (comp(&a[great * length], pivot) < 0) { // a[great * length] <= pivot
                    copy(&a[k * length], &a[less * length]);
                    copy(&a[less * length], &a[great * length]);
                    ++less;
                } else { // a[great * length] == pivot
                    /*
                     * Even though a[great] equals to pivot, the
                     * assignment a[k] = pivot may be incorrect,
                     * if a[great] and pivot are floating-point
                     * zeros of different signs. Therefore in float
                     * and double sorting methods we have to use
                     * more accurate assignment a[k] = a[great].
                     */
                    copy(&a[k * length], pivot);
                }
                copy(&a[great * length], ak);
                --great;
            }
        }

        /*
         * Sort left and right parts recursively.
         * All elements from center part are equal
         * and, therefore, already sorted.
         */
        qi_sort(a, left, less - 1, leftmost);
        qi_sort(a, great + 1, right, FALSE);
    }
#ifdef DEBUG
    dump_array("qi_sort() done.", head, N, 0, 0, length);
#endif
    return;
}


void dual_pivot(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb <= 1) return;
    length = (int)size; comp = compare;
    sort((char *)base, 0, (int)nmemb - 1);
}
