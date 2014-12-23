/*
 * qsort_head.c
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
static char	*pivot;

#ifdef DEBUG
static long	shift_count;
#endif

/* wapper of memcpy() */
static void copy(void *dst, const void *src)
{
#ifdef DEBUG
	qsort_moved++;
	if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(dst = %p, src = %p : %s)\n", dst, src, dump_data(src));
#endif
	memcpy(dst, src, length);
}

static void sort(void *base, size_t nmemb) {
	if (nmemb <= 1) return;	// 0 or 1
#ifdef DEBUG
	qsort_called++;
	dump_array("sort() start in " __FILE__, base, nmemb, length);
#endif
	copy(pivot, base);	// head element
#ifdef	DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- %s [head]\n", dump_data(pivot));
#endif
#define	head	((char *)base)
	char *tail = base + (nmemb - 1) * length;
	char *lo = base;
	char *hi = tail;
	while (TRUE) {
		while(comp(hi, pivot) > 0) hi -= length;
#ifdef	DEBUG
		if (trace_level >= TRACE_DEBUG) fprintf(OUT, "hi = %s at %p\n", dump_data(hi), hi);
#endif
		if (hi <= lo) break;
#ifdef	DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "swap %s <--> %s\n", dump_data(lo), dump_data(hi));
#endif
		copy(swapbuf, lo);
		copy(lo, hi);
		copy(hi, swapbuf);
		lo += length; hi -= length;
		while(comp(lo, pivot) < 0) lo += length;
#ifdef	DEBUG
		if (trace_level >= TRACE_MOVE) fprintf(OUT, "lo = %s at %p\n", dump_data(lo), lo);
#endif
	}
#ifdef	DEBUG
	dump_array("sort() partitioned.", base, nmemb, length);
#endif
	size_t	anterior = (lo - head) / length;	// number of elements in anterior partition
	if (lo == hi) {
#ifdef	DEBUG
		if (anterior > 0) {
			shift_count++;
			if ((trace_level >= TRACE_DUMP))
				fprintf(OUT, "shift lo = %s\tpivot = %s\tnmemb = %ld\n", lo, dump_data(pivot), nmemb);
		}
#endif
		lo += length;
	}
	size_t	posterior = (tail - lo) / length + 1;
#ifdef	DEBUG
	dump_rate(anterior, posterior);
#endif
	sort(head, anterior);
	sort(lo, posterior);
#ifdef DEBUG
	dump_array("sort() done.", base, nmemb, length);
#endif
}

void qsort_head(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{
	if (nmemb > 1) {
#ifdef DEBUG
		shift_count = 0L;
#endif
		char a[size]; swapbuf = a; *a = '\0';
		char b[size]; pivot = b; *b = '\0';
		length = size; comp = compar;
		sort(base, nmemb);
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "shift_count =  %ld\n", shift_count);
#endif
	}
}
