/*
 * nibble_sort.c
 *
 *  Nibble insertion sort with binary search.
 *
 *		Number of elements should be less or equal 16.
 *
 *  Created on: 2015/02/16
 *      Author: leo
 */

#include "sort.h"

static int  (*comp)(const void *, const void *);

static void sort(void **dst, void **src, bool revert, size_t nmemb) {
    if (nmemb <= 1 || nmemb > MAX_SIZE) return;
    void **store = revert ? src : dst;  // destination
#ifdef DEBUG
    qsort_called++;
    if (trace_level >= TRACE_DUMP) dump_pointer("sort() start in " __FILE__, src, nmemb);
    void **first = store;
	char    dumpbuf[MAX_SIZE + 1];
#endif
	void **load = revert ? dst : src;   // source
	INSERT_INDEX index = 0, mask;
	for (size_t idx = 1; idx < nmemb; idx++) {
		void *pivot = load[idx];
#ifdef DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "insert %s at %ld index = %s\n",
			dump_data(pivot), idx, dump_index(dumpbuf, index, idx));
#endif
		// binary-search
		int     ck;
		size_t pos = 0, cmp;
		size_t lo = 0, hi = idx - 1;
		while (lo <= hi)
		{   // binary search
			ck = comp(pivot, load[cmp = ((index >> (DIGIT_WIDTH * (pos = lo + ((hi - lo) >> 1)))) & MASK_DIGIT)]);
			if (ck == 0) break; // found an equal element.
			else if (ck > 0) lo = pos + 1;
			else if (pos == 0) break;
			else hi = pos - 1;      // ck < 0
		};
		if (ck > 0) pos++;
		mask = ~(INSERT_INDEX)0 << (pos *= DIGIT_WIDTH);    // note: LSB of index is smallest
#ifdef DEBUG
		if (trace_level >= TRACE_DEBUG) fprintf(OUT, "mask = %llx\tpos = %ld\n", mask, pos / DIGIT_WIDTH);
#endif
		index = ((index & mask) << DIGIT_WIDTH) // make a gap
			  | (idx << pos)                    // insert subs
			  | (index & ~mask);                // remained part
	}
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "sorted index = %s\n", dump_index(dumpbuf, index, nmemb));
#endif
	for (size_t idx = 0; idx < nmemb; index >>= DIGIT_WIDTH) store[idx++] = load[index & MASK_DIGIT];
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) dump_pointer("sort() done.", first, nmemb);
#endif
}

// pointer sort
void nibble_pointer(void **base, size_t nmemb, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1 || nmemb > MAX_SIZE) return;
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT,
			"nibble_pointer(base=%p, nmemb=%ld, compare) start.\n", base, nmemb);
#endif
	void **idxtbl = calloc(nmemb, sizeof(void *));  // double buffer
	if (idxtbl == NULL) perror(NULL);
	else {
		comp = compare;
		memcpy(idxtbl, base, nmemb * sizeof(void *));   // copy pointers
		sort(base, idxtbl, FALSE, nmemb);
		free(idxtbl);
	}
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "nibble_pointer() done.\n");
#endif
}

// index sort
void nibble_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    if (nmemb <= 1 || nmemb > MAX_SIZE) return;
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT,
			"nibble_sort(base=%p, nmemb=%ld, size=%ld, compare) start.\n", base, size, nmemb);
#endif
	void **idxtbl = make_index(base, nmemb, size);
	if (idxtbl != NULL) {
		mi_psort(idxtbl, nmemb, compare);
		unindex(base, idxtbl, nmemb, size);
		free(idxtbl);
	}
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "nibble_sort() done.\n");
#endif
}
