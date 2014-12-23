/*
 * qsort_trad.c
 *
 *	Traditional quick sort.
 *
 *  Created on: 2013/02/04
 *      Author: leo
 */

#include	"sort.h"

static int 	(*comp)(const void *, const void *);
static size_t	length;
static char	*swapbuf;
static char	*pivot;

/* wapper of memcpy() */
static void copy(void *dst, const void *src)
{
	if (dst == src) return;
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
	// select a pivot data
#define	head	((char *)base)
	char *middle = head + length * (nmemb >> 1);
	char *tail = head + length * (nmemb - 1);
	if (comp(head, tail) < 0) {	// *head < *tail
		if (comp(head, middle) > 0) copy(pivot, head);
		else if (comp(middle, tail) > 0) copy(pivot, tail);
		else copy(pivot, middle);
	}	// *head >= *tail below
	else if (comp(head, middle) < 0) copy(pivot, head);
	else if (comp(middle, tail) < 0) copy(pivot, tail);
	else copy(pivot, middle);
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot = %s\tfrom (%s, %s, %s)\n"
			, pivot, dump_data(head), dump_data(middle), dump_data(tail));
#endif
	char *lo = head, *hi = tail;
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
	dump_array("sort() done.", base, nmemb, length);
#endif
}

void qsort_trad(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{
	if (nmemb > 1) {
		char a[size]; swapbuf = a; *a = '\0';	// reused in this file
		char b[size]; pivot = b; *b = '\0';
		length = size; comp = compar;			// common in this file
		sort(base, nmemb);
	}
}
