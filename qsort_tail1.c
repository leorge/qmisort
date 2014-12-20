/*
 * qsort_tail1.c
 *
 *	Quicksort as written in wikipedia, pivot is a last element. Single loop.
 *
 *	Do the following command before GUI login after reboot.
 *
 *		for N in `seq 1000 1000 10000`; do
 *    	src/random.awk $N | Release/Sort -N $N -h1; done |
 *    	grep -v qsort.3 | grep " [012] %" | tee tmp$$
 *
 *	Nested loop of qsort_head() is faster than single loop of this.
 *
 *  Created on: 2014/12/19 as qsort_head1.c
 *  Renamed on: 2014/12/21 to qsort_tail1.c
 *      Author: leo
 */
#include "sort.h"

static int 	(*comp)(const void *, const void *);
static size_t	length;
static char	*swapbuf;

/* wapper of memcpy() */
static void copy(void *dst, const void *src)
{
#ifdef DEBUG
	qsort_moved++;
	if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(dst = %p, src = %p : %s)\n",
			dst, src, dump_data(src));
#endif
	memcpy(dst, src, length);
}

static void sort(char *head, char *tail) {
#define	N	((tail - head) / length + 1)	// Number of elements
	if (head >= tail) return;	// 0 or 1
#ifdef DEBUG
	qsort_called++;
	dump_array("sort() start in " __FILE__, head, N, length);
#endif
	char *store = head;	// pivot is a last element
	for (char *idx = head; idx < tail; idx += length) {
		if (comp(idx, tail) < 0) {
			if (idx > store) {
				copy(swapbuf, idx);
				copy(idx, store);
				copy(store, swapbuf);
#ifdef	DEBUG
				if (trace_level >= TRACE_DUMP) fprintf(OUT, "swap data %s <--> %s\n",
						dump_data(idx), dump_data(store));
			}
			else {
				if (trace_level >= TRACE_DEBUG) fprintf(OUT, "idx = %s\n", dump_data(idx));
#endif
			}
			store += length;
		}
	}
	if (store < tail) {
#ifdef	DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "swap pivot %s <--> %s\n",
				dump_data(tail), dump_data(store));
#endif
		copy(swapbuf, tail);
		copy(tail, store);
		copy(store, swapbuf);
	}
	sort(head, store - length);
	sort(store + length, tail);
#ifdef DEBUG
	dump_array("sort() done.", head, N, length);
#endif
}

void qsort_tail1(void *base, size_t nmemb, size_t size, int(*compar)(const void *, const void *))
{
	if (nmemb > 1) {
		char a[size]; swapbuf = a; *a = '\0';
		length = size; comp = compar;
		sort(base, (char *)base + (nmemb - 1) * size);
	}
}
