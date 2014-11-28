/*
 * qsort_tail.c
 *
 *	Quick sort - Use a tail element as a hole.
 *
 *  Created on: 2013/01/01
 *      Author: leo
 */
#include "sort.h"

static int		(*comp)(const void *, const void *);
static size_t	length;
static char	*pivot;

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
#define	head	((char *)base)
	char *tail = head + length * (nmemb - 1);	// point a last element
	char *hole = tail;
	copy(pivot, hole);
#ifdef	DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- %s [tail]\n", dump_data(pivot));
#endif
	char *lo, *hi = hole - length;
	for (lo = head; lo < hole; lo += length) {
		if (comp(lo, pivot) >= 0) {
#ifdef	DEBUG
			if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s --> %s\n", dump_data(lo), dump_data(hole));
#endif
			copy(hole, lo);
			hole = lo;
			for (; hi > hole; hi -= length) {
				if (comp(hi, pivot) < 0) {	// asymmetric comparison
#ifdef	DEBUG
					if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s <-- %s\n", dump_data(hole), dump_data(hi));
#endif
					copy(hole, hi);
					hole = hi;
				}
			}
		}
	}
#ifdef	DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s to %s [%ld]\n",
			dump_data(pivot), dump_data(hole), (hole - head) / length);
#endif
	copy(hole, pivot);	// restore
	size_t	anterior = (hole - head) / length;	// number of element in anterior partition
	size_t	posterior = (tail - hole) / length;
#ifdef DEBUG
	dump_array("sort() partitioned", base, nmemb, length);
	dump_rate(anterior, posterior);
#endif
	sort(head, anterior);
	sort(hole + length, posterior);
#ifdef DEBUG
	dump_array("sort() done.", base, nmemb, length);
#endif
}

void qsort_tail(void *base, size_t nmemb, size_t size, int(*compar)(const void *, const void *))
{
	if (nmemb > 1) {
		char a[size]; pivot = a; *a = '\0';
		length = size; comp = compar;
		sort(base, nmemb);
#ifdef	DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "qsort_tail() done.\n");
#endif
	}
}
