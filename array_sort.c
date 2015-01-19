/*
 * array_sort.c
 *
 *	Array sort of QM / QMI sort
 *
 *  Created on: 2013/03/23
 *      Author: leo
 */
#include	<math.h>
#include "sort.h"

static int		(*comp)(const void *, const void *);
static void	*index[MAX_BIT];	// address of picked up elements
static size_t	length;
static char	*pivot;
static size_t	private_boundary;
#ifdef	DEBUG
static	size_t	search_pivot;
#endif

static void copy(void *dst, const void *src)
{
#ifdef	DEBUG
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
	if (nmemb <= private_boundary) {
		(*small_array)(base, nmemb, length, comp);
	}
	else {	// qsort_log2()
#define	first	((char *)base)
		int	pickup = ((*func_log2)(nmemb) - 1) | 1;	// make an odd number 2N-1
		size_t	distance = (size_t)(nmemb / pickup);	// distance between elements
#ifdef	DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT,
			"nmemb = %ld\tchoose = %d\tdistance = %ld\n", nmemb, pickup, distance);
#endif
		distance *= length;		// size in byte
		char	*hole = base + (size_t)(random_number * nmemb / pickup) * length;	// 1st pick up point
		for (int idx = 0; idx < pickup; hole += distance) {
#ifdef	DEBUG
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
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot = %s\n", dump_data(hole));
#endif
		char *last = first + length * (nmemb - 1);
#ifdef	DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- hole = %s <-- tail = %s\n", dump_data(hole), dump_data(last));
#endif
		copy(pivot, hole); copy(hole, last);	// pivot <-- hole <-- tail
		char	*lo = first,  *hi = (hole = last) - length, *eq = NULL;
		for (; lo < hole; lo += length) {
			if (comp(lo, pivot) >= 0) {
#ifdef	DEBUG
				if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s --> %s\n", dump_data(lo), dump_data(hole));
#endif
				copy(hole, lo);
				hole = lo;
				for (; hi > hole; hi -= length) {
					int chk;
					if ((chk = comp(hi, pivot)) < 0) {
#ifdef	DEBUG
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
#ifdef	DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s to %s [%ld]\n",
				dump_data(pivot), dump_data(hole), (hole - first) / length);
#endif
		copy(hole, pivot);	// restore
#ifdef DEBUG
		dump_array("sort() partitioned", base, nmemb, length);
#endif
		if (eq == NULL) eq = hole;
#ifdef DEBUG
		else if (trace_level >= TRACE_DUMP) fprintf(OUT,"skip higher %ld elements\n", (eq - hole) / length);
#endif
		size_t	anterior = (hole - first) / length;	// number of element in anterior partition
		size_t	posterior = (last - eq) / length;
#ifdef DEBUG
		dump_rate(anterior, posterior);
#endif
		sort(first, anterior);
		sort(eq + length, posterior);
	}
#ifdef DEBUG
	dump_array("sort() done.", base, nmemb, length);
#endif
}

void array_sort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{
	if (nmemb > 1) {
		char a[size]; pivot = a; *a = '\0';
		length = size; comp = compar;
		set_random();
		private_boundary = pow(2, 2 * log2(nmemb) / 3);	// 2 ^ ( 2/3 * log2(N))
#ifdef DEBUG
		search_pivot = 0;
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "private_boundary = %ld\n", private_boundary);
#endif
		sort(base, nmemb);
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "search_pivot = %ld\n", search_pivot);
#endif
	}
}
