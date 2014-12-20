/*
 * qsort_head1.c
 *
 *	traditional quick sort using swap, pivot is a first element. Single loop.
 *
 *  Created on: 2013/01/27
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
#define	count	((tail - head) / length + 1)
	if (tail <= head) return;	// 0 or 1
#ifdef DEBUG
	qsort_called++;
	dump_array("sort() start in " __FILE__, head, count, length);
#endif
	char *idx, *store = tail;
	for (idx = tail; idx > head; idx -= length) {
		if (comp(idx, head) > 0) {
			if (idx < store) {
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
			store -= length;
		}
	}
	if (store > head) {
#ifdef	DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "swap pivot %s <--> %s\n",
				dump_data(head), dump_data(store));
#endif
		copy(swapbuf, head);
		copy(head, store);
		copy(store, swapbuf);
	}
	sort(head, store - length);
	sort(store + length, tail);
#ifdef DEBUG
	dump_array("sort() done.", head, count, length);
#endif
}

void qsort_head1(void *base, size_t nmemb, size_t size, int(*compar)(const void *, const void *))
{
	if (nmemb > 1) {
		char a[size]; swapbuf = a; *a = '\0';
		length = size; comp = compar;
		sort(base, (char *)base + (nmemb - 1) * size);
	}
}
