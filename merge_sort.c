/*
 * merge_sort.c
 *
 *	Double buffer merge sort.
 *
 *  Created on: 2013/01/11
 *      Author: leo
 */

#include "sort.h"

static int	(*comp)(const void *, const void *);
static size_t	length;

static void copy(void *dst, const void *src, size_t size)
{
#ifdef	DEBUG
//	qsort_moved += size;
	qsort_moved++;
	if (trace_level >= TRACE_MOVE) fprintf(OUT, "copy(dst = %s, src = %s, size = %ld)\n",
			dump_data(dst), dump_data(src), size);
#endif
	memcpy(dst, src, size * length); /* restore an elements  */
}

static void sort(void *dst, void *src, bool revert,  size_t nmemb) {
	if (nmemb <= 1) return;
#ifdef DEBUG
	qsort_called++;
	if (trace_level >= TRACE_DUMP) dump_array("sort() start in " __FILE__, src, nmemb, length);
#endif
	size_t anterior = nmemb >> 1;	// = nmemb / 2
	size_t posterior = nmemb - anterior;
	sort(dst, src, ! revert, anterior);
	sort((char *)dst + anterior * length, (char *)src + anterior * length, ! revert, posterior);
	char *store = revert ? src : dst;
#ifdef DEBUG
	char *head = store;		// for debugging
#endif
	char *left = revert ? dst : src;
	char *right = &left[anterior * length];
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) {
		dump_array("left", left, anterior, length);
		dump_array("right", right, posterior, length);
	}
#endif
	while (TRUE) {
		if (comp(left, right) < 0) {
			copy(store, left, 1); store += length; left += length;		// add one
			if (--anterior <= 0) {	// empty?
#ifdef DEBUG
				if (trace_level >= TRACE_DUMP) dump_array("append right", right, posterior, length);
#endif
				copy(store, right, posterior);	// append remained data
				break;
			}
		}
		else {
			copy(store, right, 1); store += length; right += length;
			if (--posterior <= 0) {
#ifdef DEBUG
				if (trace_level >= TRACE_DUMP) dump_array("append left", left, anterior, length);
#endif
				copy(store, left, anterior);
				break;
			}
		}
	}
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) dump_array("sort() done.", head, nmemb, length);
#endif
}

void merge_sort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "merge_sort() nmemb = %ld\n", nmemb);
#endif
	if (nmemb <= 1) return;
	void *dup = calloc(nmemb, size);
	if (dup != NULL) {
		length = size; comp = compar;
		memcpy(dup, base, nmemb * size);
		sort(base, dup, FALSE, nmemb);
		free(dup);
	}
}
