/*
 * quick_random.c
 *
 *  Quicksort - select a random pivot as a hole.
 *               asymmetric search to resolve a few data issue.
 *
 *  Created on: 2013/05/10
 *      Author: leo
 */
#include "sort.h"

static int      (*comp)(const void *, const void *);
static size_t   length;
#ifdef  DEBUG
static size_t   random;         // random number to reuse
#endif

static void copy(void *dst, const void *src)
{
#ifdef  DEBUG
	dump_copy(dst, src);
#endif
    memcpy(dst, src, length); /* restore an elements  */
}

static void sort(void *base, size_t nmemb, int depth) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    qsort_called++;
    dump_array("sort() start in " __FILE__, base, nmemb, length);
#else
    size_t   random;
#endif
#define first   ((char *)base)
    char    *hole;
    if (nmemb <= small_boundary) {
        hole = first + (nmemb >> 1) * length;	// middle element
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot is at middle %s [%ld]\n",
                    dump_data(hole), (hole - first) / length);
#endif
    }
    else {  // N is large
		if (depth > 0) {
			depth--;
			random = set_random();
		}
#ifdef DEBUG
		else if (reuse_random) {}   // no change
#endif
		else random = RAND_BASE >> 1;
        size_t distance;
        switch (pivot_type) {   // Quicksort Algorithm
        case PIVOT_RANDOM:      // a single random element
            hole = first + (nmemb * random / RAND_BASE) * length;    // 0 .. (nmemb - 1)
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "random pivot is %s [%ld]\n",
                    dump_data(hole), (hole - first) / length);
#endif
            break;
        case PIVOT_HYBRID:
        	if (nmemb >= 128) {	// 2^7
                hole = pivot_array(base, nmemb, length, ((size_t)log2(nmemb) - 1) | 1, comp, random);
        		break;
        	}	// do the case PIVOT_RANDOM3: if log2(N) < 5
        	/* no break */
        case PIVOT_RANDOM5:     // median of 5 elements
        	if (nmemb >= 32) {	// 2^5
                distance = nmemb >> 2;	// N / 4
                size_t offset = (distance * random) / RAND_BASE;	// [0, N/4)
                char *p1 = first + offset * length;					// in [0, N/4)
                char *p5 = p1 + ((nmemb >> 1) + distance) * length;	// in [3N/4, N)
                char *p3 = first + ((nmemb >> 1) - (nmemb >> 4) + (offset >> 1)) * length;	// in [7N/16, 9N/16)
                distance >>= 1;			// N / 8
                char *p2 = p3 - distance * length;	// in [5N/16, 7N/16)
                char *p4 = p3 + distance * length;	// in [9N/16, 11N/16)
#ifdef  DEBUG
                if (trace_level >= TRACE_DUMP) fprintf(OUT, "Choose a pivot from %s %s %s %s %s",
                    dump_data(p1), dump_data(p2), dump_data(p3), dump_data(p4), dump_data(p5));
#endif
                if (comp(p1, p5) > 0) {char *tmp = p1; p1 = p5; p5 = tmp;}	// p1 <--> p5 then *p1 < *p5
                if (comp(p2, p4) > 0) {char *tmp = p2; p2 = p4; p4 = tmp;}	// p2 <--> P4 then *p2 < *p4
                if (comp(p3, p2) < 0) {char *tmp = p2; p2 = p3; p3 = tmp;}	// p3 <--> p2 then *p2 < *p3 < *p4
                else if (comp(p4, p3) < 0) {char *tmp = p4; p4 = p3; p3 = tmp;}	// p4 <--> p3 then *p2 < *p3 < *p4
                hole = comp(p3, p1) < 0 ? (comp(p1, p4) < 0 ? p1 : p4)
                						: (comp(p5, p3) < 0 ? (comp(p5, p2) < 0 ? p2 : p5) : p3);
#ifdef  DEBUG
                if (trace_level >= TRACE_DUMP) fprintf(OUT, " --> %s\n", dump_data(hole));
                int	LEFT = 0, RIGHT=0, CHK = 0;
#define SIDE(a) CHK = comp((a), hole); if (CHK < 0) LEFT++; else if (CHK > 0) RIGHT++;
                SIDE(p1); SIDE(p2); SIDE(p3); SIDE(p4); SIDE(p5);
                assert(LEFT < 3 && RIGHT < 3);
#endif
        		break;
        	}	// do the case PIVOT_RANDOM3: if log2(N) < 5
        	/* no break */
        case PIVOT_RANDOM3:     // median of 3 elements
            distance = nmemb >> 2;    // N / 4
            char *p1 = first + ((distance >> 1) + ((distance * random) / RAND_BASE)) * length;	// in [N/8, 3N/8)
            char *p2 = p1 + (distance *= length);												// in [3N/8, 5N/8)
            char *p3 = p2 + distance;															// in [5N/8, 7N/8)
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "Choose a pivot from %s %s %s",
                dump_data(p1), dump_data(p2), dump_data(p3));
#endif
            hole = (comp(p1, p3) < 0 ?
                   (comp(p2, p1) < 0 ? p1: (comp(p2, p3) < 0 ? p2 : p3)) :
                   (comp(p2, p3) < 0 ? p3: (comp(p2, p1) < 0 ? p2 : p1)));
#ifdef  DEBUG
                if (trace_level >= TRACE_DUMP) fprintf(OUT, " --> %s\n", dump_data(hole));
                int	LEFT = 0, RIGHT=0, CHK = 0;
                SIDE(p1); SIDE(p2); SIDE(p3);
                assert(LEFT < 2 && RIGHT < 2);
#endif
            break;
        case PIVOT_VARIOUS:     // median of variout elements
            hole = pivot_array(base, nmemb, length, pivot_number, comp, random);
            break;
        case PIVOT_LOG2:        // median of log2(N) elements
            hole = pivot_array(base, nmemb, length, ((size_t)log2(nmemb) - 1) | 1, comp, random);
            break;
        default:
#ifdef DEBUG
            assert(FALSE);
#endif
            break;
        }
    }
    char *last = first + length * (nmemb - 1);    // point a last element
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- hole = %s [%ld] <-- last = %s\n"
    		, dump_data(hole), (hole - first) / length ,dump_data(last));
#endif
    char pivot[length]; copy(pivot, hole); copy(hole, last);    // pivot <-- hole <-- last
    char    *lo = first,  *hi = (hole = last) - length, *eq = NULL;
    for (; lo < hole; lo += length) {
        if (comp(lo, pivot) >= 0) {
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s --> %s\n", dump_data(lo), dump_data(hole));
#endif
            copy(hole, lo);
            hole = lo;
            for (; hi > hole; hi -= length) {
                int chk;
                if ((chk =comp(hi, pivot)) < 0) {
#ifdef  DEBUG
                    if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s <-- %s\n", dump_data(hole), dump_data(hi));
#endif
                    copy(hole, hi);
                    hole = hi;
                    eq = NULL;  // not equal then reset
                }
                else if (chk > 0) eq = NULL;
                else if (eq == NULL) eq = hi;   // first equal element
            }
        }
    }
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s --> %s [%ld]\n"
            , dump_data(pivot), dump_data(hole), (lo - first) / length);
#endif
    copy(hole, pivot);  // restore
#ifdef DEBUG
    dump_array("sort() partitioned", base, nmemb, length);
#endif
    if (eq == NULL) eq = hole;
#ifdef DEBUG
    else if (trace_level >= TRACE_DUMP) fprintf(OUT,"skip higher %ld elements\n", (eq - hole) / length);
#endif
    size_t  n_lo = (hole - first) / length; // number of element in lower partition
    size_t  n_hi = (last - eq) / length;
#ifdef DEBUG
    dump_rate(n_lo, n_hi);
#endif
    sort(first, n_lo, depth);
    sort(eq + length, n_hi, depth);
#ifdef DEBUG
    dump_array("sort() done.", base, nmemb, length);
#endif
}

void quick_random(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb > 1) {
#ifdef DEBUG
        dump_array("quick_random() start in " __FILE__, base, nmemb, size);
#endif
        length = size; comp = compare;
        sort(base, nmemb, random_depth);
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "quick_random() done.\n");
#endif
    }
}
