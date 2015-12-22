/*
 * get_pivot.c
 *
 *  Search the median of several elements.
 *
 *  Created on: 2015/05/27
 *      Author: leo
 */
#include    "sort.h"

void *median_of_5(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *), size_t random) {
    void *rtn;
    size_t distance = nmemb >> 2;   // N / 4
    size_t offset = (distance * random) / RAND_BASE;    // [0, N/4)
    char *p1 = (char *)base + offset * size;                    // in [0, N/4)
    char *p5 = p1 + ((nmemb >> 1) + distance) * size;   // in [3N/4, N)
    char *p3 = (char *)base + ((nmemb >> 1) - (nmemb >> 4) + (offset >> 1)) * size; // in [7N/16, 9N/16)
    distance >>= 1;         // N / 8
    char *p2 = p3 - distance * size;    // in [5N/16, 7N/16)
    char *p4 = p3 + distance * size;    // in [9N/16, 11N/16)
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "Median of 5 in %ld from %s %s %s %s %s",
        nmemb, dump_data(p1), dump_data(p2), dump_data(p3), dump_data(p4), dump_data(p5));
#endif
    // If you are a librarian, you can convert If statements to ternary operators.
    if (compare(p1, p5) > 0) {char *tmp = p1; p1 = p5; p5 = tmp;}   // p1 <--> p5 then *p1 < *p5
    if (compare(p2, p4) > 0) {char *tmp = p2; p2 = p4; p4 = tmp;}   // p2 <--> P4 then *p2 < *p4
    if (compare(p3, p2) < 0) {char *tmp = p2; p2 = p3; p3 = tmp;}   // p3 <--> p2 then *p2 < *p3 < *p4
    else if (compare(p4, p3) < 0) {char *tmp = p4; p4 = p3; p3 = tmp;}  // p4 <--> p3 then *p2 < *p3 < *p4
    rtn = compare(p3, p1) < 0 ? (compare(p1, p4) < 0 ? p1 : p4)
                            : (compare(p5, p3) < 0 ? (compare(p5, p2) < 0 ? p2 : p5) : p3);
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, " --> %s\n", dump_data(rtn));
    int LEFT = 0, RIGHT=0, CHK;
#define SIDE(a) CHK = compare((a), rtn); if (CHK < 0) LEFT++; else if (CHK > 0) RIGHT++;
    SIDE(p1); SIDE(p2); SIDE(p3); SIDE(p4); SIDE(p5);
    assert(LEFT < 3 && RIGHT < 3);
#endif
    return  rtn;
}

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

/* search a pivot in an array   */
void *pivot_array(void *base, size_t nmemb, size_t size, size_t pickup, int (*compare)(const void *, const void *), size_t random)
{
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT,
            "pivot_array(base, nmemb = %ld, size = %ld, pickup = %ld, compare(), random = %ld)\n", nmemb, size, pickup, random);
    assert(base != NULL);
    assert(nmemb > 1);
    assert(size != 0);
    assert(pickup != 0);
    assert(compare != NULL);
#endif
    void *rtn;
    size_t  distance = (size_t)(nmemb / pickup);      // distance of elements
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "distance = %ld\n", distance);
#endif
    distance *= size;     // size in byte
#define first   ((char *)base)
    char *p = first + (nmemb * random / RAND_BASE / pickup) * size;  // 1st pick up point
#ifdef  DEBUG
    char tmp[10], buffer[pickup * sizeof(tmp)]; *buffer = '\0';
#endif
    void *index[pickup];
    for (int idx = 0; idx < pickup; p += distance) {
        index[idx++] = p;
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) {
            snprintf(tmp, sizeof(tmp), " %s", dump_data(p));
            strcat(buffer, tmp);
        }
#endif
    }
    rtn = search_median(index, pickup, compare);
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "Median of %ld in %ld from%s --> %s\n"
                                              , pickup, nmemb, buffer, dump_data(rtn));
#endif
    return rtn;
}
