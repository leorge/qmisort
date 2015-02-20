/*
 * qsort_first.c
 *
 *	traditional quick sort using swap, pivot is a first element. nested loop.
 *
 *  Created on: 2013/01/27
 *      Author: leo
 */
#include "sort.h"

static int 	(*comp)(const void *, const void *);
static size_t	length;
static char	*swapbuf;

#ifdef DEBUG
static long	shift_count;
#endif

/* wapper of memcpy() */
static void swap(void *p1, void *p2)
{
	if (p1 == p2) return;
#ifdef DEBUG
	qsort_moved += 3;
	if (trace_level >= TRACE_MOVE) fprintf(OUT, "swap(%s, %s)\n", dump_data(p1), dump_data(p2));
#endif
	memcpy(swapbuf, p1, length);
	memcpy(p1, p2, length);
	memcpy(p2, swapbuf, length);
}

static void sort(void *base, size_t nmemb) {
	if (nmemb <= 1) return;	// 0 or 1
#define	first	((char *)base)
#define	pivot	first
#ifdef DEBUG
	qsort_called++;
	dump_array("sort() start in " __FILE__, base, nmemb, length);
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot = %s at first element\n", dump_data(pivot));
#endif
	char *last = base + (nmemb - 1) * length;
	char *lo = first;
	char *hi = last + length;
	while (TRUE) {
		while(comp(lo += length, pivot) < 0) if (lo >= last) break;
		while(comp(hi -= length, pivot) > 0) if (hi <= first) break;
#ifdef	DEBUG
		if (trace_level >= TRACE_DEBUG) fprintf(OUT, "lo = %s\thi = %s\n"
				, dump_data(lo), dump_data(hi));
#endif
		if (lo >= hi) break;
		swap(lo, hi);
	}
	swap(first, hi);
#ifdef	DEBUG
	dump_array("sort() partitioned.", base, nmemb, length);
#endif
	size_t	n_lo = (hi - first) / length;	// number of elements in lower partition
	size_t	n_hi = (last - hi) / length;
#ifdef	DEBUG
	dump_rate(n_lo, n_hi);
#endif
	sort(first, n_lo);
	sort(hi + length, n_hi);
#ifdef DEBUG
	dump_array("sort() done.", base, nmemb, length);
#endif
}

void qsort_first(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
	if (nmemb > 1) {
#ifdef DEBUG
		shift_count = 0L;
#endif
		char a[size]; swapbuf = a; *a = '\0';
		length = size; comp = compare;
		sort(base, nmemb);
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "shift_count =  %ld\n", shift_count);
#endif
	}
}
