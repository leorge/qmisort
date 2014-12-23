/*
 * qsort_various.c
 *
 *	Quick sort - Quick sort with a hole. Pivot is selected from various elements.
 *
 *  Created on: 2014/09/15
 *      Author: leo
 */
#include "sort.h"
#include "log2s.h"
#include	<math.h>

static int		(*comp)(const void *, const void *);
static size_t	length;
static void	**index;	// address of picked up elements
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
		size_t	distance = (size_t)(nmemb / pivot_number);		// distance of elements
#ifdef	DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "nmemb = %ld\tdistance = %ld\n" , nmemb, distance);
#endif
		distance *= length;		// size in byte
		hole = base + (size_t)(random_number * nmemb / pivot_number) * length;	// 1st pick up point
		for (int idx = 0; idx < pivot_number; hole += distance) {
#ifdef	DEBUG
			if (trace_level >= TRACE_DEBUG) fprintf(OUT, "array[%ld] at %p = %s\n", (hole - head) / length, hole, dump_data(hole));
#endif
			index[idx++] = hole;
		}
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "sort to find a pivot\n");
#endif
		iqsort(index, pivot_number, comp);	// indexed qsort(3)
		hole = index[pivot_number >> 1];
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot = %s\n", dump_data(hole));
#endif
	}
	else hole = head + length * (nmemb >> 1);	// middle element
#ifdef	DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- hole = %s <-- tail = %s\n", dump_data(hole), dump_data(tail));
#endif
	copy(pivot, hole);
	copy(hole, tail);	// pivot <-- hole <-- tail
	hole = tail;
	char *lo = head, *hi = tail - length, *hi_head = NULL;
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
					hi_head = NULL;	// not equivalent then reset
				}
				else if (chk > 0) hi_head = NULL;
				else if (hi_head == NULL) hi_head = hi;	// first equivalent element
			}
		}
	}
#ifdef	DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s --> %s [%ld]\n",
			dump_data(pivot), dump_data(hole), (hole - head) / length);
#endif
	copy(hole, pivot);	// restore
#ifdef DEBUG
	dump_array("sort() partitioned", base, nmemb, length);
#endif
	if (hi_head == NULL) hi_head = hole;
#ifdef DEBUG
	else if (trace_level >= TRACE_DUMP) fprintf(OUT,"skip higher %ld elements\n", (hi_head - hole) / length);
#endif
	size_t	anterior = (hole - head) / length;	// number of element in anterior partition
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

void qsort_various(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{
	if (nmemb > 1) {
		char *buff = calloc(pivot_number, sizeof(void *));
		if (buff != NULL) {
#ifdef DEBUG
			if (trace_level >= TRACE_DUMP) fprintf(OUT,
					"pivot_number = %d\tboundary = %ld\n" , pivot_number, log2_boundary);
#endif
			char a[size]; pivot = a; *a = '\0';
			length = size; comp = compar;
			set_random();
			index = (void **)buff;
			sort(base, nmemb, random_depth);
			free(buff);
		}
	}
}
