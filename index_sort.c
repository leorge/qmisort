/*
 * index_sort.c
 *
 *  Index sort of QM / QMI sort.
 *
 *  Created on: 2013/03/27
 *      Author: leo
 */
#include    <math.h>
#include "sort.h"

static int      (*comp)(const void *, const void *);
static void *index[MAX_BIT];    // address of picked up elements
#ifdef  DEBUG
static  size_t  comp_idx_count, search_pivot;
#endif

static int  comp_idx(const void *p1, const void *p2) {
#ifdef DEBUG
    comp_idx_count++;
    if (trace_level >= TRACE_DEBUG) fprintf(OUT, "comp_idx(%s, %s)\n",
        dump_data(*(char * const *)p1), dump_data(*(char * const *)p2));
#endif
    return  comp(*(char * const *)p1, *(char * const *)p2);
}

static void sort(void *base[], size_t nmemb) {
    if (nmemb <= 1) return;
#ifdef DEBUG
    qsort_called++; // Counted in small_index() or pivot_sort()
    dump_pointer("sort() start in " __FILE__, base, nmemb);
#endif
    if (nmemb <= small_boundary) {
        (*middle_index)(base, nmemb, comp);
    }
    else {
    	size_t  distance;
    	void **hole;
    	int pickup;
    	switch (QA) {	// Quicksort Algorithm
    	case RANDOM:
            hole = &base[(size_t)(random_number * nmemb)];  // pick up one element at random
    		break;
    	case RANDOM3:
            distance = (size_t)(nmemb / 3);	    // distance between elements
            void **p1, **p2, **p3;
            p1 = &base[(size_t)(random_number * distance)];
            p3 = (p2 = p1 + distance) + distance;
#ifdef  DEBUG
    		if (trace_level >= TRACE_DUMP) fprintf(OUT,
    				"nmemb = %ld\tdistance = %ld\t pickup = (%s, %s, %s)\n",
    				nmemb, distance, dump_data(*p1), dump_data(*p2), dump_data(*p3));
#endif
			hole = (comp(*p1, *p3) < 0 ?
				   (comp(*p2, *p1) < 0 ? p1: (comp(*p2,  *p3) < 0 ? p2 : p3)) :
				   (comp(*p2, *p3) < 0 ? p3 : (comp(*p2, *p1) < 0 ? p2 : p1)));
    		break;
    	case LOG2:
        	pickup = ((*func_log2)(nmemb) - 1) | 1; // make an odd number 2N-1
            distance = (size_t)(nmemb / pickup);    // distance between elements
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT,
                "nmemb = %ld\tbit_width = %d\tdistance = %ld\n", nmemb, pickup, distance);
#endif
            hole = &base[(size_t)(random_number * nmemb / pickup)];  // 1st pick up point
            for (int idx = 0; idx < pickup; hole += distance) {
#ifdef  DEBUG
                ispointer = TRUE;
                if (trace_level >= TRACE_DEBUG) fprintf(OUT, "%s\n", dump_data(*hole));
#endif
                index[idx++] = hole;
            }
#ifdef DEBUG
			if (trace_level >= TRACE_DUMP) {
				fprintf(OUT, "index :");
				for (int idx = 0; idx < pickup; ) fprintf(OUT, " %s", dump_data(index[idx++]));
				fprintf(OUT, "\nsort to find a pivot\n");
			}
            search_pivot++;
#endif
            (*pivot_sort)(index, pickup, comp_idx);
            hole = index[pickup >> 1];      // get address of address of middle element
            break;
    	}
#ifdef  DEBUG
        ispointer = FALSE;
        if (trace_level >= TRACE_DUMP) {
            fprintf(OUT, "pivot = %s\n", dump_data(*hole));
        }
#endif
        void    **last = &base[nmemb - 1];
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "\npivot <-- pivot = %s <-- last = %s\n", dump_data(*hole), dump_data(*last));
#endif
        void    *pivot = *hole; *hole = *last;
        void    **lo, **hi = hole = last, **eq = NULL;
        for (hi--, lo = base; lo < hole; lo++) {
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "start : lo=%p\thole=%p\thi=%p\n", lo, hole, hi);
#endif
            if (comp(*lo, pivot) >= 0) {
#ifdef  DEBUG
                if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s --> %s\n", dump_data(*lo), dump_data(*hole));
#endif
                *hole = *lo;    // copy address of bigger element
                hole = lo;      // change position to store
                for (; hi > hole; hi--) {
                    int chk;
                    if ((chk = comp(*hi, pivot)) < 0) {
#ifdef  DEBUG
                        if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s <-- %s\n", dump_data(*hole), dump_data(*hi));
#endif
                        *hole = *hi;
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
                dump_data(pivot), dump_data(*hole), hole - base);
#endif
        *hole = pivot;  // restore
#ifdef DEBUG
        dump_pointer("sort() partitioned", base, nmemb);
#endif
        if (eq == NULL) eq = hole;
#ifdef DEBUG
        else if (trace_level >= TRACE_DUMP) fprintf(OUT,"skip higher %ld elements\n", eq - hole);
#endif
        size_t  n_lo = hole - base; // number of element in lower partition
        size_t  n_hi = last - eq;
#ifdef DEBUG
        dump_rate(n_lo, n_hi);
#endif
        sort(base, n_lo);
        sort(eq + 1, n_hi);
    }
#ifdef DEBUG
    dump_pointer("sort() done.", base, nmemb);
#endif
}

void pointer_sort(void **idxtbl, size_t nmemb, int (*compare)(const void *, const void *))
{
    if (nmemb <= 1) return;
    if (idxtbl != NULL) {
        comp = compare;
        set_random();
#ifdef DEBUG
        search_pivot = comp_idx_count = 0;
#endif
        sort(idxtbl, nmemb);
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) {
            fprintf(OUT, "count of comp_idx = %ld\n", comp_idx_count);
            fprintf(OUT, "search pivot %ld times\n", search_pivot);
        }
#endif
    }
}

void index_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb <= 1) return;
    void **idxtbl = make_index(base, nmemb, size);
    if (idxtbl != NULL) {
        pointer_sort(idxtbl, nmemb, compare);
        unindex(base, idxtbl, nmemb, size);
        free(idxtbl);
    }
}

/***    Stable sort for continuous array    ***/
static int acomp(const void *p1, const void *p2) {
    return  p1 - p2;
}

static void address_sort(void **idxtbl, size_t nmemb, int (*compare)(const void *, const void *))
{
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) {
        fprintf(OUT, "address_sort() start. nmemb = %ld\n", nmemb);
        for (int i = 0; i < nmemb; i++) fprintf(OUT, "idxtbl[%d] = %p\t%s\n", i, idxtbl[i], (char *)idxtbl[i]);
    }
#endif
    comp = acomp;   // replace function to compare
    void    **p = idxtbl;
    void    **from = p++;
    for (int i = 1; i < nmemb; i++, p++) {
        if (compare(*p, *from)) {
            sort(from, p - from);
            from = p;
        }
    }
    sort(from, p - from);
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) {
//      for (int i = 0; i < nmemb; i++) fprintf(OUT, "idxtbl[%d] = %p\t%s\n", i, idxtbl[i], (char *)idxtbl[i]);
        fprintf(OUT, "address_sort() done.\n");
    }
#endif
}

void stable_array(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb <= 1) return;
    void **idxtbl = make_index(base, nmemb, size);
    if (idxtbl != NULL) {
        pointer_sort(idxtbl, nmemb, compare);
        address_sort(idxtbl, nmemb, compare);
        unindex(base, idxtbl, nmemb, size);
        free(idxtbl);
    }
}
