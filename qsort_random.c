/*
 * qsort_random.c
 *
 *	Quick sort - select a random pivot in hole method.
 *
 *  Created on: 2013/05/10
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

static void sort(void *base, size_t nmemb, int depth) {
	if (nmemb <= 1) return;
#ifdef DEBUG
	qsort_called++;
	dump_array("sort() start in " __FILE__, base, nmemb, length);
#endif
#define	head	((char *)base)
	char	*hole, *tail = head + length * (nmemb - 1);	// point a last element
	if (depth > 0) {
		depth--;
		double	random = rand()/(double)RAND_MAX;
#ifdef	DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "random = %f\n", random);
#endif
		hole = head + length * (size_t)((nmemb - 1) * random);	// 0 .. (nmemb - 1)
	}
	else hole = head + length * (nmemb >> 1);	// middle element
	copy(pivot, hole);
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) {
		fprintf(OUT,"pivot <-- %s [%ld]\n", dump_data(pivot), (hole - head) / length);
	}
#endif
	copy(hole, tail);
	hole = tail;
	char *lo, *hi = tail - length, *lo_tail = NULL, *hi_head = NULL;
	int chk;
	for (lo = head; lo < hole; lo += length) {
		if ((chk = comp(lo, pivot)) > 0) {	// An element greater than the pivot is found.
#ifdef	DEBUG
			if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s --> %s\n", dump_data(lo), dump_data(hole));
#endif
			copy(hole, lo);
			hole = lo;
			for (; hi > hole; hi -= length) {
				if ((chk =comp(hi, pivot)) < 0) {
#ifdef	DEBUG
					if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s <-- %s\n", dump_data(hole), dump_data(hi));
#endif
					copy(hole, hi);
					hole = hi;
					hi_head = NULL;	// not equivalent then reset
				}
				else if (chk > 0) hi_head = NULL;
				else if (hi_head == NULL) hi_head = hi;	// first equivalent element
			}
			lo_tail = NULL;
		}
		else if (chk < 0) lo_tail = NULL;
		else if (lo_tail == NULL) lo_tail = lo;
	};
#ifdef	DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s --> %s [%ld]\n"
			, dump_data(pivot), dump_data(hole), (lo - head) / length);
#endif
	copy(hole, pivot);	// restore
#ifdef DEBUG
	dump_array("sort() partitioned", base, nmemb, length);
#endif
	if (lo_tail == NULL) lo_tail = hole;	// not conitured equivalent element
#ifdef DEBUG
	else if (trace_level >= TRACE_DUMP) fprintf(OUT,"skip lower %ld elements\n", (hole - lo_tail) / length);
#endif
	if (hi_head == NULL) hi_head = hole;
#ifdef DEBUG
	else if (trace_level >= TRACE_DUMP) fprintf(OUT,"skip higher %ld elements\n", (hi_head - hole) / length);
#endif
	size_t	anterior = (lo_tail - head) / length;	// number of element in anterior partition
	size_t	posterior = (tail - hi_head) / length;
#ifdef DEBUG
	dump_rate(anterior, posterior);
#endif
	sort(head, anterior, depth);
	sort(hi_head + length, posterior, depth);
#ifdef DEBUG
	dump_array("sort() done.", base, nmemb, length);
#endif
}

void qsort_random(void *base, size_t nmemb, size_t size, int(*compar)(const void *, const void *))
{
	if (nmemb > 1) {
		char a[size]; pivot = a; *a = '\0';
		length = size; comp = compar;
		sort(base, nmemb, random_depth);
	}
}
