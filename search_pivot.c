/*
 * search_pivot.c
 *
 *  Search a median element quickly.
 *
 *  Created on: 2015/05/27
 *      Author: leo
 */
#include    "sort.h"

static void *search_median(void **base, size_t nmemb, int (*compare)(const void *, const void *))
{
#ifdef DEBUG
	search_pivot++;
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "search_median() : ");
    assert(base != NULL);
    assert(nmemb != 0);
    assert(compare != NULL);
#endif
    void **top = base, **bottom = &base[nmemb - 1], **middle = &base[nmemb >> 1];
    while (top < bottom) {
#ifdef  DEBUG
        dump_pointer("array", top, bottom - top + 1);
#endif
        void **hole = &top[(bottom - top) >> 1];    // middle
        void *pivot = *hole;    // store middle data
        *hole = *bottom;
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- %s <-- %s\n"
                , dump_data(pivot), dump_data(*hole));
#endif
        hole = bottom; // last data --> middle
        void **lo = top, **hi = bottom - 1, **eq = NULL;
        for (; lo < hole; lo++) {
            if (compare(*lo, pivot) >= 0) {
#ifdef  DEBUG
                if (trace_level >= TRACE_MOVE) fprintf(OUT, "move %s --> %s\n"
                        , dump_data(*lo), dump_data(*hole));
#endif
                *hole = *lo; hole = lo;
                for (; hi > hole; hi--) {
                    int chk;
                    if ((chk =compare(*hi, pivot)) < 0) {
#ifdef  DEBUG
                        if (trace_level >= TRACE_MOVE) fprintf(OUT, "move %s <-- %s\n"
                                , dump_data(*hole), dump_data(*hi));
#endif
                        *hole = *hi; hole = hi;
                        eq = NULL;  // not equal then reset
                    }
                    else if (chk > 0) eq = NULL;
                    else if (eq == NULL) eq = hi;   // first equal element
                }
            }
        }
        *hole = pivot;  // restore
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) {
            fprintf(OUT, "hole = %s\tmiddle = %s\teq = %s\n", dump_data(*hole),
                    dump_data(*middle), eq ? dump_data(*eq) : "(null)");
            dump_pointer("", top, bottom - top + 1);
        }
#endif
        if (hole == middle) break;  // if input data is sorted then break soon.
        else if (middle < hole) bottom = hole - 1;
        else if (eq == NULL) top = hole + 1;    // hole < middle && not continuous
        else if (eq < middle) top = eq + 1;     // hole < eq < middle
        else break; // hole < middle <= eq
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT,"search_median() --> %s\n", dump_data(*middle));
#endif
    return  *middle;
}

/* search a pivot in an array	*/
void *pivot_array(void *base, size_t nmemb, size_t size, size_t pickup, int (*compare)(const void *, const void *), size_t random)
{
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT,
            "pivot_array(base, nmemb = %ld, size = %ld, pickup = %ld, compare())\n", nmemb, size, pickup);
    assert(base != NULL);
    assert(nmemb > 1);
    assert(size != 0);
    assert(pickup != 0);
    assert(compare != NULL);
#endif
    size_t  distance = (size_t)(nmemb / pickup);      // distance of elements
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "distance = %ld\n", distance);
#endif
    distance *= size;     // size in byte
#define first   ((char *)base)
    char *p = first + (nmemb * random / RAND_BASE / pickup) * size;  // 1st pick up point
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "first data = %s\n", dump_data(p));
#endif
    void *index[pickup];
    for (int idx = 0; idx < pickup; p += distance) index[idx++] = p;
    return search_median(index, pickup, compare);
}

/* search a pivot in an index	*/
//void *pivot_pointer(void **base, size_t nmemb, size_t pickup, int (*compare)(const void *, const void *), size_t random)
//{
//#ifdef  DEBUG
//    if (trace_level >= TRACE_DUMP) fprintf(OUT,
//            "pivot_pointer(base, nmemb = %ld, pickup = %ld, compare())\n", nmemb, pickup);
//    assert(base != NULL);
//    assert(nmemb > 1);
//    assert(pickup != 0);
//#endif
//    size_t distance = (size_t)(nmemb / pickup);    // distance between elements
//    void **p = &base[nmemb * random / RAND_BASE / pickup];  // 1st pick up point
//#ifdef  DEBUG
//    if (trace_level >= TRACE_DUMP) fprintf(OUT, "distance = %ld\tfirst data = %s\n" , distance, dump_data(p));
//#endif
//    void *index[pickup];
//    for (int idx = 0; idx < pickup; p += distance) index[idx++] = p;
//    return search_median(index, pickup, compare);
//}
