/*
 * qsort_width.c
 *
 *	Quick sort - number of elements to select a pivot is about log2(nmemb).
 *
 *  Created on: 2013/05/25
 *      Author: leo
 */
#include	"sort.h"
#include	"log2s.h"
#include	<math.h>

static int		(*comp)(const void *, const void *);
static size_t	length;
static void	*index[MAX_BIT];	// address of picked up elements
static char	*pivot;
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
#define	first	((char *)base)
	char	*hole, *last = first + length * (nmemb - 1);	// point a last element
	if (nmemb > log2_boundary) {
		int		pickup = ((*func_log2)(nmemb) - 1) | 1;	// make an odd number 2N-1
		size_t	distance = (size_t)(nmemb / pickup);		// distance of elements
#ifdef	DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "nmemb = %ld\tbit_width = %d\tdistance = %ld\n" , nmemb, pickup, distance);
#endif
		distance *= length;		// size in byte
		hole = base + (size_t)(random_number * nmemb / pickup) * length;	// 1st pick up point
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
		iqsort(index, pickup, comp);	// indexed qsort(3)
		hole = index[pickup >> 1];
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot = %s\n", dump_data(hole));
#endif
	}
	else hole = first + length * (nmemb >> 1);	// middle element
#ifdef	DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- hole = %s <-- tail = %s\n", dump_data(hole), dump_data(last));
#endif
	copy(pivot, hole);
	copy(hole, last);	// pivot <-- hole <-- tail
	hole = last;
	char *lo = first, *hi = last - length, *eq = NULL;
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
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore pivot %s --> %s [%ld]\n",
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
#ifdef DEBUG
	dump_array("sort() done.", base, nmemb, length);
#endif
}

void qsort_log2(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{
	if (nmemb > 1) {
		char a[size]; pivot = a; *a = '\0';
		length = size; comp = compar;
		set_random();
#ifdef	DEBUG
		search_pivot = 0;
#endif
		sort(base, nmemb);
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT,"search pivot %ld times\n", search_pivot);
#endif
	}
}
