/*
 * insert_sort.c
 *
 *	Insertion sort.
 *
 *  Created on: 2013/01/08
 *      Author: leo
 */

#include "sort.h"

bool	ispointer;

/* index sort	*/
void insert_psort(void **idxtbl, size_t nmemb, int (*compare)(const void *, const void *)) {
	if (nmemb <= 1) return;
#ifdef DEBUG
	qsort_called++;
	if (trace_level >= TRACE_DUMP) dump_pointer("insert_isort in " __FILE__, idxtbl, nmemb);
#endif
	char *pivot;
	size_t	i;
	int		ck = 0;
	for (i = 1; i < nmemb; i++) {
		size_t	pos = 0, lo = 0, hi = i - 1;
		pivot = idxtbl[i];
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) {
			char	msg[30];
			sprintf(msg, "insert %s to", dump_data(pivot));
			dump_pointer(msg, idxtbl, i);
		}
#endif
		while (lo <= hi) {
			ck = compare(pivot, idxtbl[pos = lo + ((hi - lo) >> 1)]);
			if (ck == 0) break;				// found an equal element.
			else if (ck > 0) lo = pos + 1;
			else if (pos > 0) hi = pos - 1;		// ck < 0
			else break;	// found an element equevalent to the pivot.
		};
		if (ck > 0) pos++;
//		for (lo = (hi = i) - 1; hi > pos;) idxtbl[hi--] = idxtbl[lo--];	// This statement is equivalent to the next line
		memmove(&idxtbl[pos + 1], &idxtbl[pos], (i - pos)*sizeof(void *));	// instead of memcpy(3).
		idxtbl[pos] = pivot;
	}
}

/* in-place sort	*/
void insert_sort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
	if (nmemb <= 1) return;
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) dump_array("insert_sort", base, nmemb, size);
#endif
#define	head	((char *)base)
	char pivot[size];
	size_t	i;
	int		ck = 0;
	for (i = 1; i < nmemb; i++) {
		size_t	pos = 0, lo = 0, hi = i - 1;
		memcpy(pivot, base + i * size, size);
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) {
			char	msg[30];
			sprintf(msg, "insert %s to", dump_data(pivot));
			dump_array(msg, base, i, size);
		}
#endif
		while (lo <= hi) {
			ck = compar(pivot, head + (pos = lo + ((hi - lo) >> 1)) * size);
			if (ck == 0) break;				// found an equal element.
			else if (ck > 0) lo = pos + 1;
			else if (pos > 0) hi = pos - 1;		// ck < 0
			else break;	// found an element equevalent to the pivot.
		};
		if (ck > 0) pos++;
//		for (lo = (hi = i) - 1; hi > pos;) base[hi--] = base[lo--];	// This statement is equivalent to the next line
		memmove(base + (pos + 1) * size, base + pos * size, (i - pos) * size);	// instead of memcpy(3).
#ifdef	DEBUG
		qsort_called++;
		qsort_moved++;
//		qsort_moved += i - pos;
#endif
		memcpy(base + pos * size, pivot, size);
	}
}
