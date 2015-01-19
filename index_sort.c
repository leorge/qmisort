/*
 * index_sort.c
 *
 *	Index sort of QM / QMI sort.
 *
 *  Created on: 2013/03/27
 *      Author: leo
 */
#include	<math.h>
#include "sort.h"

static int		(*comp)(const void *, const void *);
static void	*index[MAX_BIT];	// address of picked up elements
static size_t	length;
static size_t	private_boundary;
#ifdef	DEBUG
static	size_t	comp_idx_count, search_pivot;
#endif

static int	comp_idx(const void *p1, const void *p2) {
#ifdef DEBUG
	comp_idx_count++;
	if (trace_level >= TRACE_DEBUG) fprintf(OUT, "comp_idx(%s, %s)\n",
		dump_data(*(char * const *)p1), dump_data(*(char * const *)p2));
#endif
	return	comp(*(char * const *)p1, *(char * const *)p2);
}

static void sort(void *base[], size_t nmemb) {
	if (nmemb <= 1) return;
#ifdef DEBUG
	qsort_called++;	// Counted in small_index() or pivot_sort()
	dump_pointer("sort() start in " __FILE__, base, nmemb);
#endif
	if (nmemb <= private_boundary) {
		(*small_index)(base, nmemb, comp);
	}
	else {
		int	pickup = ((*func_log2)(nmemb) - 1) | 1;	// make an odd number 2N-1
		size_t	distance = (size_t)(nmemb / pickup);	// distance between elements
#ifdef	DEBUG
		ispointer = TRUE;
		if (trace_level >= TRACE_DUMP) fprintf(OUT,
			"nmemb = %ld\tbit_width = %d\tdistance = %ld\n", nmemb, pickup, distance);
#endif
		void **hole = &base[(size_t)(random_number * nmemb / pickup)];	// 1st pick up point
		for (int idx = 0; idx < pickup; hole += distance) {
#ifdef	DEBUG
			if (trace_level >= TRACE_DEBUG) fprintf(OUT, "%s\n", dump_data(*hole));
#endif
			index[idx++] = hole;
		}
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "sort to find a pivot\n");
		search_pivot++;
#endif
		(*pivot_sort)(index, pickup, comp_idx);
		hole = index[pickup >> 1];		// get address of address of middle element
#ifdef	DEBUG
		ispointer = FALSE;
		if (trace_level >= TRACE_DUMP) {
			fprintf(OUT, "pivot = %s\n", dump_data(*hole));
			fprintf(OUT, "index :");
			for (int idx = 0; idx < pickup; ) fprintf(OUT, " %s", dump_data(index[idx++]));
			fprintf(OUT, "\n");
		}
#endif
		void	**last = &base[nmemb - 1];
#ifdef	DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "\npivot <-- pivot = %s <-- last = %s\n", dump_data(*hole), dump_data(*last));
#endif
		void	*pivot = *hole; *hole = *last;
		void	**lo, **hi = hole = last, **eq = NULL;
		for (hi--, lo = base; lo < hole; lo++) {
#ifdef	DEBUG
			if (trace_level >= TRACE_DUMP) fprintf(OUT, "start : lo=%p\thole=%p\thi=%p\n", lo, hole, hi);
#endif
			if (comp(*lo, pivot) >= 0) {
#ifdef	DEBUG
				if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s --> %s\n", dump_data(*lo), dump_data(*hole));
#endif
				*hole = *lo;	// copy address of bigger element
				hole = lo;		// change position to store
				for (; hi > hole; hi--) {
					int chk;
					if ((chk = comp(*hi, pivot)) < 0) {
#ifdef	DEBUG
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
#ifdef	DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s to %s [%ld]\n",
				dump_data(pivot), dump_data(*hole), hole - base);
#endif
		*hole = pivot;	// restore
#ifdef DEBUG
		dump_pointer("sort() partitioned", base, nmemb);
#endif
		if (eq == NULL) eq = hole;
#ifdef DEBUG
		else if (trace_level >= TRACE_DUMP) fprintf(OUT,"skip higher %ld elements\n", eq - hole);
#endif
		size_t	n_lo = hole - base;	// number of element in lower partition
		size_t	n_hi = last - eq;
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
		private_boundary = pow(2, 2 * log2(nmemb) / 3);	// 2 ^ ( 2/3 * log2(N))
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT,"private_boundary = %ld\n", private_boundary);
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

void index_sort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{
	if (nmemb <= 1) return;
	void **idxtbl = make_index(base, nmemb, size);
	if (idxtbl != NULL) {
		length = size;
		pointer_sort(idxtbl, nmemb, compar);
		unindex(base, idxtbl, nmemb, size);
		free(idxtbl);
	}
}
