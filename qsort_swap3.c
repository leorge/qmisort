/*
 * qsort_swap3.c
 *
 *	Traditional quick sort using swap().
 *		pivot is selected from head, tail and middle element.
 *
 *  Created on: 2012/11/25
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

static void swap(void *p1, void *p2) {
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "swap %s <--> %s\n", dump_data(p1), dump_data(p2));
#endif
	copy(swapbuf, p1);
	copy(p1, p2);
	copy(p2, swapbuf);
}

static void sort(void *base, size_t nmemb) {
	if (nmemb <= 1) return;
#ifdef DEBUG
	qsort_called++;
	dump_array("sort() start in " __FILE__, base, nmemb, length);
#endif
#define	head	((char *)base)
	char *tail = head + (nmemb - 1) * length;
#ifdef DEBUG
	if ((trace_level >= TRACE_DUMP))
		fprintf(OUT, "compare head and tail (%s , %s)\n", dump_data(head), dump_data(tail));
#endif
	if (comp(head, tail) > 0) swap(head, tail);
	if (nmemb < 3) goto done;	// and return
	char *middle = base + length * (nmemb >> 1);
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "compare middle to head and tail (%s , %s , %s)\n"
			, dump_data(head), dump_data(middle), dump_data(tail));
#endif
	if (comp(middle, head) < 0) swap(middle, head);
	else if (comp(middle, tail) > 0) swap(middle, tail);
	if (nmemb > 3) {
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- %s [%ld]\n", middle, (middle - head) / length);
#endif
		copy(pivot, middle);
		char *lo = head + length;
		char *hi = tail - length;
		while (TRUE) {
			while(comp(lo, pivot) < 0) lo += length;
			while(comp(pivot, hi) < 0) hi -= length;
#ifdef	DEBUG
			if (trace_level >= TRACE_DEBUG)
				fprintf(OUT, "lo = %s at %p\thi = %s at %p\n", dump_data(lo), lo, dump_data(hi), hi);
#endif
			if (lo >= hi) break;
			swap(lo, hi);
			lo += length; hi -= length;
		}
		size_t	anterior = (lo - head) / length, posterior = (tail - lo) / length + 1;
#ifdef	DEBUG
		dump_array("sort() partitioned.", base, nmemb, length);
		dump_rate(anterior, posterior);
#endif
		sort(head, anterior);
		sort(lo, posterior);
	}
done:;
#ifdef DEBUG
	dump_array("sort() done.", base, nmemb, length);
#endif
}

void qsort_swap3(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{
	if (nmemb > 1) {
		char a[size]; swapbuf = a; *a = '\0';
		char b[size]; pivot = b; *b = '\0';
		length = size; comp = compar;
		sort(base, nmemb);
	}
}
