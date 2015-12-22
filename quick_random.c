/*
 * quick_random.c
 *
 *  Quicksort - select a random pivot as a hole.
 *               asymmetric search to resolve a few data issue.
 *
 *  Created on: 2013/05/10
 *      Author: leo
 */
#include "sort.h"

static int      (*comp)(const void *, const void *);
static size_t   length;
#ifdef  DEBUG
static size_t   random; // reused in DEBUG program
#endif

static void copy(void *dst, const void *src)
{
#ifdef  DEBUG
    dump_copy(dst, src);
#endif
    memcpy(dst, src, length); /* restore an elements  */
}

typedef struct {
    char  *base;
    size_t N;
} ARRAY;

#ifdef  DEBUG
void dump_partition(const char *msg, ARRAY array) {
    dump_array(msg, array.base, array.N, length);
}
#endif

static void partition(const ARRAY array, ARRAY *sub_array, char *hole) {
    size_t nmemb = array.N;
    char *first = array.base;
    char *last = first + length * (nmemb - 1);    // point a last element
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- hole = %s [%ld] <-- last = %s\n"
            , dump_data(hole), (hole - first) / length ,dump_data(last));
#endif
    char save[length]; copy(save, hole); copy(hole, last);    // save <-- hole <-- last
    char    *lo = first,  *hi = (hole = last) - length, *eq = NULL;
    for (; lo < hole; lo += length) {
        if (comp(lo, save) >= 0) {
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s --> %s\n", dump_data(lo), dump_data(hole));
#endif
            copy(hole, lo);
            hole = lo;
            for (; hi > hole; hi -= length) {
                int chk;
                if ((chk =comp(hi, save)) < 0) {
#ifdef  DEBUG
                    if (trace_level >= TRACE_DUMP) fprintf(OUT, "move %s <-- %s\n", dump_data(hole), dump_data(hi));
#endif
                    copy(hole, hi);
                    hole = hi;
                    eq = NULL;  // not equal then reset
                }
                else if (chk > 0) eq = NULL;
                else if (eq == NULL) eq = hi;   // first equal element
            }
        }
    }
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "restore save %s --> %s [%ld]\n"
            , dump_data(save), dump_data(hole), (lo - first) / length);
#endif
    copy(hole, save);  // restore
#ifdef DEBUG
    dump_partition("sort() partitioned", array);
#endif
    if (eq == NULL) eq = hole;
#ifdef DEBUG
    else if (trace_level >= TRACE_DUMP) fprintf(OUT,"skip higher %ld elements\n", (eq - hole) / length);
#endif
    size_t  n_lo = (hole - first) / length; // number of element in lower partition
    size_t  n_hi = (last - eq) / length;
#ifdef DEBUG
    dump_rate(n_lo, n_hi);
#endif
    sub_array[0].base = array.base;  sub_array[0].N = n_lo;
    sub_array[1].base = eq + length; sub_array[1].N = n_hi;
}

/* not hybrid sort */
static void random_sort(ARRAY array, int depth) {
    if (array.N <= 1) return;
#ifdef DEBUG
    qsort_called++;
    dump_partition("sort() start in " __FILE__, array);
#else
    size_t   random;    // not reused in Release program
#endif
    if (depth > 0) {
        depth--;
        random = set_random();  // to choose a random element
    }
#ifdef DEBUG
    else if (reuse_random) {   // no change
    	if (trace_level >= TRACE_DUMP) fprintf(OUT,"random number is reused %ld\n", random);
    }
#endif
    else random = RAND_BASE >> 1;   // to choose the middle element

    size_t distance, nmemb = array.N;
    char *hole, *first = array.base;
    if (nmemb <= median1) {
        hole = first + (nmemb * random / RAND_BASE) * length;   // usually the middle element
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot is at middle %s [%ld]\n",
                dump_data(hole), (hole - first) / length);
#endif
    }
    else if (nmemb <= median3) {
        distance = nmemb >> 2;    // N / 4
        char *p1 = first + ((distance >> 1) + ((distance * random) / RAND_BASE)) * length;  // in [N/8, 3N/8)
        char *p2 = p1 + (distance *= length);                                               // in [3N/8, 5N/8)
        char *p3 = p2 + distance;                                                           // in [5N/8, 7N/8)
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "Median of 3 in %ld from %s %s %s",
            nmemb, dump_data(p1), dump_data(p2), dump_data(p3));
#endif
        hole = (comp(p1, p3) < 0 ?
               (comp(p2, p1) < 0 ? p1: (comp(p2, p3) < 0 ? p2 : p3)) :
               (comp(p2, p3) < 0 ? p3: (comp(p2, p1) < 0 ? p2 : p1)));
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, " --> %s\n", dump_data(hole));
        int LEFT = 0, RIGHT=0, CHK;
#define SIDE(a) CHK = comp((a), hole); if (CHK < 0) LEFT++; else if (CHK > 0) RIGHT++;
        SIDE(p1); SIDE(p2); SIDE(p3);
        assert(LEFT < 2 && RIGHT < 2);
#endif
    }
    else if (nmemb <= median5) {
        hole = median_of_5(first, nmemb, length, comp, random);
    }
    else {  // N is large
        hole = pivot_array(first, nmemb, length, ((size_t)log2(nmemb) - 1) | 1, comp, random);
    }

    ARRAY sub_array[2];
    partition(array, sub_array, hole);
    random_sort(sub_array[0], depth);
    random_sort(sub_array[1], depth);
#ifdef DEBUG
    dump_partition("sort() done.", array);
#endif
}

void quick_random(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb > 1) {
#ifdef DEBUG
        dump_array("quick_random() start in " __FILE__, base, nmemb, size);
#endif
        length = size; comp = compare;
        ARRAY array; array.base = base; array.N = nmemb;
        random_sort(array, random_depth);
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "quick_random() done.\n");
#endif
    }
}

/* hybrid sort */
static void hybrid_sort(ARRAY array, RANDOM_DEPTH depth) {
    size_t  nmemb = array.N;
    if (nmemb > 1) {
        if (nmemb <= threshold) {    // hybrid sort
            (*medium_func)(array.base, nmemb, length, comp);
        }
        else {  // N is large
#ifdef DEBUG
            qsort_called++;
            dump_partition("hybrid_sort() start in " __FILE__, array);
#endif
            char *hole;
            size_t   random;
            if (depth > 0) {
                depth--;
                random = set_random();
            }
            else random = RAND_BASE >> 1;
            hole = median_of_5(array.base, nmemb, length, comp, random);
            ARRAY sub_array[2];
            partition(array, sub_array, hole);
            random_sort(sub_array[0], depth);
            random_sort(sub_array[1], depth);
#ifdef DEBUG
            dump_partition("hybrid_sort() done.", array);
#endif
        }	// I will make this function secure.
    }
}

void quick_hybrid(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb > 1) {
        length = size; comp = compare;
        ARRAY array; array.base = base; array.N = nmemb;
        hybrid_sort(array, random_depth);
#ifdef DEBUG
        if (search_pivot && trace_level >= TRACE_DUMP)
            fprintf(OUT, "search_pivot = %ld times\n", search_pivot);
#endif
    }
}

/* Stable sort */
typedef struct {
    size_t  index;      // index in a void **idxtbl
    void    *address;   // addres of element
} POINTER_INDEX;

static int      (*comp_p)(const void *, const void *);

static int acomp(const void *p1, const void *p2) {
    return  comp_p(((const POINTER_INDEX *)p1)->address, ((const POINTER_INDEX *)p2)->address);
}

static int icomp(const void *p1, const void *p2) {
    return  ((POINTER_INDEX *)p1)->index -  ((POINTER_INDEX *)p2)->index;
}

void stable_pointer(void **idxtbl, size_t nmemb, int (*compare)(const void *, const void *))
{
    if (nmemb <= 1) return;
    void *tbl = calloc(nmemb, sizeof(POINTER_INDEX));
    ARRAY array;
    if (tbl != NULL) {
        /*  store idxtbl to POINTER_INDEX[] */
        void    **p = idxtbl;
        POINTER_INDEX   *t = (POINTER_INDEX *)tbl;
        for (size_t i = 0; i < nmemb; t++) {
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "tbl[%ld].address = %p %s\n", i, *p, (char *)*p);
#endif
            t->index = i++;
            t->address = *p++;  // may be gotten by malloc()
        }
        /*  sort    */
        comp = acomp;       // sort() calls comp()
        comp_p = compare;   // comp() calls compare()
        length = sizeof(POINTER_INDEX);
        array.base = tbl; array.N = nmemb;
        hybrid_sort(array, random_depth);   // sort array
        /*  sort to be stable and reoder idxtbl */
        comp = icomp;
        t = (POINTER_INDEX *)tbl;
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "tbl[0] = %ld, %p %s\n", t->index, t->address, (char *)(t->address));
#endif
        POINTER_INDEX   *from = t++;
        for (size_t i = 1; i < nmemb; i++, t++) {
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "tbl[%ld] = %ld, %p %s\n", i, t->index, t->address, (char *)(t->address));
#endif
            if (compare(t->address, from->address)) {
                array.base = from; array.N = t - from;
                hybrid_sort(array, random_depth);
                from = t;
            }
        }
        array.base = from, array.N = t - from;
        hybrid_sort(array, 0);
        /* reorder idxtbl   */
        p = idxtbl;
        t = (POINTER_INDEX *)tbl;
        for (size_t i = 0; i < nmemb; i++) {
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "tbl[%ld] = %ld, %p %s\n", i, t->index, t->address, (char *)(t->address));
#endif
            *p++ = t++->address;
        }
        /* done */
        free(tbl);
    }
}
