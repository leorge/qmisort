/*
 * array_sort.c
 *
 *  Array sort of QM / QMI sort
 *
 *  Created on: 2013/03/23
 *      Author: leo
 */
#include    <math.h>
#include "sort.h"

static int      (*comp)(const void *, const void *);
static void 	*index[MAX_BIT];    // address of picked up elements
static size_t   length;
static char 	*pivot;
#ifdef  DEBUG
static  size_t  search_pivot;
#endif

static void copy(void *dst, const void *src)
{
#ifdef  DEBUG
    qsort_moved++;
    if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(dst = %s, src = %s)\n", dump_data(dst), dump_data(src));
#endif
    memcpy(dst, src, length); /* restore an elements  */
}

static void sort(void *base, size_t nmemb) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    qsort_called++;
    dump_array("sort() start in " __FILE__, base, nmemb, length);
#endif
    if (nmemb <= middle_boundary) {
        (*middle_array)(base, nmemb, length, comp);
    }
    else {	// N is large
#define first   ((char *)base)
    	size_t  distance;
    	char    *hole;
    	int pickup;
    	switch (QA) {	// Quicksort Algorithm
    	case RANDOM:
            hole = base + (size_t)(random_number * nmemb) * length;		// pick up one element at random
    		break;
    	case RANDOM3:
			distance = nmemb / 3;    // distance between elements
			char	*p1, *p2, *p3;
			p1 = base + (size_t)(random_number * distance) * length;		// pick up median of random 3 elements
			p3 = (p2 = p1 + distance * length) + distance * length;
#ifdef  DEBUG
    		if (trace_level >= TRACE_DUMP) fprintf(OUT,
    				"nmemb = %ld\tdistance = %ld\t pickup = (%s, %s, %s)\n", nmemb, distance, p1, p2, p3);
#endif
			hole = (comp(p1, p3) < 0 ?
				   (comp(p2, p1) < 0 ? p1: (comp(p2,  p3) < 0 ? p2 : p3)) :
				   (comp(p2, p3) < 0 ? p3 : (comp(p2, p1) < 0 ? p2 : p1)));
    		break;
    	case LOG2:
    		pickup = ((*func_log2)(nmemb) - 1) | 1; // make an odd number
    		distance = (size_t)(nmemb / pickup);    // distance between elements
#ifdef  DEBUG
    		if (trace_level >= TRACE_DUMP) fprintf(OUT,
    				"nmemb = %ld\tchoose = %d\tdistance = %ld\n", nmemb, pickup, distance);
#endif
    		distance *= length;     // size in byte
    		hole = base + (size_t)(random_number * nmemb / pickup) * length;   // 1st pick up point
    		for (int idx = 0; idx < pickup; hole += distance) {
#ifdef  DEBUG
    			if (trace_level >= TRACE_DEBUG) fprintf(OUT, "array[%ld] at %p = %s\n", (hole - first) / length, hole, dump_data(hole));
#endif
    			index[idx++] = hole;
    		}
#ifdef DEBUG
    		if (trace_level >= TRACE_DUMP) fprintf(OUT, "sort to find a pivot\n");
    		search_pivot++;
#endif
    		(*pivot_sort)(index, pickup, comp);
    		hole = index[pickup >> 1];
    		break;
    	}
#ifdef DEBUG
    	if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot = %s\n", dump_data(hole));
#endif
        char *last = first + length * (nmemb - 1);
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- hole = %s <-- last = %s\n", dump_data(hole), dump_data(last));
#endif
        copy(pivot, hole); copy(hole, last);    // pivot <-- hole <-- last
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
                    if ((chk = comp(hi, pivot)) < 0) {
#ifdef  DEBUG
                        if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s <-- %s\n", dump_data(hole), dump_data(hi));
#endif
                        copy(hole, hi);
                        hole = hi;
                        eq = NULL;
                    }
                    else if (chk > 0) eq = NULL;
                    else if (eq == NULL) eq = hi;
                }
            }
        }
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s to %s [%ld]\n",
                dump_data(pivot), dump_data(hole), (hole - first) / length);
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
        sort(first, n_lo);
        sort(eq + length, n_hi);
    }
#ifdef DEBUG
    dump_array("sort() done.", base, nmemb, length);
#endif
}

void array_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb > 1) {
        char a[size]; pivot = a; *a = '\0';
        length = size; comp = compare;
        set_random();
#ifdef DEBUG
        search_pivot = 0;
#endif
        sort(base, nmemb);
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "search_pivot = %ld\n", search_pivot);
#endif
    }
}

/* Stable sort for pointer array */

typedef struct {
    size_t  index;      // index in a void **idxtbl
    void    *address;   // addres of element
} POINTER_INDEX;

static int      (*comp_p)(const void *, const void *);

static int acomp(const void *p1, const void *p2) {
    return  comp_p(((const POINTER_INDEX *)p1)->address, ((const POINTER_INDEX *)p2)->address);
}

static int icomp(const void *p1, const void *p2) {
    return  ((POINTER_INDEX *)p1)->index -  ((POINTER_INDEX *)p2)->index;
}

void stable_pointer(void **idxtbl, size_t nmemb, int (*compare)(const void *, const void *))
{   // like index_sort()
    if (nmemb <= 1) return;
    void *tbl = calloc(nmemb, sizeof(POINTER_INDEX));
    if (tbl != NULL) {
        /*  store idxtbl to POINTER_INDEX[] */
        void    **p = idxtbl;
        POINTER_INDEX   *t = (POINTER_INDEX *)tbl;
        for (size_t i = 0; i < nmemb; t++) {
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "tbl[%ld].address = %p %s\n", i, *p, (char *)*p);
#endif
            t->index = i++;
            t->address = *p++;  // may be gotten by malloc()
        }
        /*  sort    */
        char a[sizeof(POINTER_INDEX)]; pivot = a; *a = '\0';
        comp = acomp;       // sort() calls comp()
        comp_p = compare;   // comp() calls compare()
        length = sizeof(POINTER_INDEX);
        set_random();
        sort(tbl, nmemb);   // sort array
        /*  sort to be stable and reoder idxtbl */
        comp = icomp;
        t = (POINTER_INDEX *)tbl;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "tbl[0] = %ld, %p %s\n", t->index, t->address, (char *)(t->address));
#endif
        POINTER_INDEX   *from = t++;
        for (size_t i = 1; i < nmemb; i++, t++) {
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "tbl[%ld] = %ld, %p %s\n", i, t->index, t->address, (char *)(t->address));
#endif
            if (compare(t->address, from->address)) {
                sort(from, t - from);
                from = t;
            }
        }
        sort(from, t - from);
        /* reorder idxtbl   */
        p = idxtbl;
        t = (POINTER_INDEX *)tbl;
        for (size_t i = 0; i < nmemb; i++) {
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "tbl[%ld] = %ld, %p %s\n", i, t->index, t->address, (char *)(t->address));
#endif
            *p++ = t++->address;
        }
        /* done */
        free(tbl);
    }
}
