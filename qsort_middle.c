/*
 * qsort_middle.c
 *
 *	traditional quick sort using swap, pivot is a middle element.
 *
 *  Created on: 2013/02/09
 *      Author: leo
 */
#include "sort.h"

static int 	(*comp)(const void *, const void *);
static size_t	length;
static char	*swapbuf;
static char	*pivot;

/* wapper of memcpy() */
static void copy(void *dst, const void *src)
{
#ifdef DEBUG
	qsort_moved++;
	if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(dst = %p, src = %p : %s)\n", dst, src, dump_data(src));
#endif
	memcpy(dst, src, length); /* restore an elements  */
}

static void sort(void *base, size_t nmemb) {
	if (nmemb <= 1) return;
#ifdef DEBUG
	qsort_called++;
	dump_array("sort() start in " __FILE__, base, nmemb, length);
#endif
#define	head	((char *)base)
	copy(pivot, head + length * (nmemb >> 1));	// middle element
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- %s [%ld]\n", dump_data(pivot), nmemb >> 1);
#endif
	char	*tail = head + (nmemb - 1) * length;
	char	*lo = head;
	char	*hi = tail;
	while (TRUE) {
		while(comp(lo, pivot) < 0) lo += length;
		while(comp(pivot, hi) < 0) hi -= length;
#ifdef	DEBUG
		if (trace_level >= TRACE_DEBUG)
			fprintf(OUT, "lo = %s at %p\thi = %s at %p\n", dump_data(lo), lo, dump_data(hi), hi);
#endif
		if (lo >= hi) break;
#ifdef	DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "swap %s <--> %s\n", dump_data(lo), dump_data(hi));
#endif
		copy(swapbuf, lo);
		copy(lo, hi);
		copy(hi, swapbuf);
		lo += length; hi -= length;
	}
	size_t	anterior = (lo - head) / length;
	size_t	posterior = (tail - lo) / length + 1;
#ifdef	DEBUG
	dump_array("sort() partitioned.", head, nmemb, length);
	dump_rate(anterior, posterior);
#endif
	sort(head, anterior);
	sort(lo, posterior);
#ifdef DEBUG
	dump_array("sort() done.", head, nmemb, length);
#endif
}

void qsort_middle(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{
	if (nmemb > 1) {
		char a[size]; swapbuf = a; *a = '\0';
		char b[size]; pivot = b; *b = '\0';
		length = size; comp = compar;
		sort(base, nmemb);
	}
}
