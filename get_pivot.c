/*
 * get_pivot.c
 *
 *  Choose the median of several elements.
 *
 *  Try the following command.
 *      $ N=xx; random.awk $N | Debug/Sort -N $N OtherOptions -V 2 | grep "Median of "
 *
 *  Created on: 2015/05/27
 *      Author: leo
 */
#include    "sort.h"

void *median_of_5(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
    void *rtn;
    char *p1, *p2, *p3, *p4, *p5;
    p1 = (char *)base + (((nmemb >> 2) * rand()) / RAND_BASE) * size; // [0, N/4)
    size_t distance = ((nmemb >> 3) + (nmemb >> 4)) * size;  // N/8 + N/16 = 3N/16
    p5 = (p4 = (p3 = (p2 = p1 + distance) + distance) + distance) + distance;   // [3N/16, N)
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "Median of 5 in %s from %s %s %s %s %s",
            dump_size_t(NULL, nmemb), dump_data(p1), dump_data(p2), dump_data(p3), dump_data(p4), dump_data(p5));
#endif
    // You can rewrite statements below with a plenty of ternary operators.
    if (compare(p2, p4) > 0) {char *tmp = p2; p2 = p4; p4 = tmp;}   // p2 <--> P4 then *p2 < *p4
    if (compare(p2, p3) > 0) {char *tmp = p2; p2 = p3; p3 = tmp;}   // p3 <--> p2 then *p2 < *p3 < *p4
    else if (compare(p3, p4) > 0) {char *tmp = p4; p4 = p3; p3 = tmp;}  // p4 <--> p3 then *p2 < *p3 < *p4
    if (compare(p1, p5) > 0) {char *tmp = p1; p1 = p5; p5 = tmp;}   // p1 <--> p5 then *p1 < *p5
    rtn =  compare(p3, p1) < 0 ? (compare(p1, p4) < 0 ? p1 : p4)
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
    if (trace_level >= TRACE_DUMP) dump_pointer("search_median() : ", base, nmemb);
    assert(base != NULL);
    assert(nmemb != 0);
    assert(compare != NULL);
#endif
    void **left = base, **right = &base[nmemb - 1], **middle = &base[nmemb >> 1];
    while (left < right) {
#ifdef  DEBUG
        dump_pointer("array", left, right - left + 1);
#endif
        void **hole = &left[(right - left) >> 1];
        void *pivot = *hole;    // save middle data
        *hole = *right;         // last data --> middle
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "pivot <-- %s <-- %s\n"
                , dump_data(pivot), dump_data(*hole));
#endif
        hole = right;           // dig a hole at the last
        void **lo = left, **hi = right - 1;
        for (int chk; lo < hole; lo++) {
            if ((chk = compare(*lo, pivot))>= 0) {
#ifdef  DEBUG
                if (trace_level >= TRACE_MOVE) fprintf(OUT, "move %s --> %s\n"
                        , dump_data(*lo), dump_data(*hole));
#endif
                *hole = *lo; hole = lo;
                for (; hi > hole; hi--) {
                    if ((chk = compare(*hi, pivot)) < 0) {
#ifdef  DEBUG
                        if (trace_level >= TRACE_MOVE) fprintf(OUT, "move %s <-- %s\n"
                                , dump_data(*hole), dump_data(*hi));
#endif
                        *hole = *hi; hole = hi;
                    }
                }
            }
        }
        *hole = pivot;  // restore
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) {
            fprintf(OUT, "hole = %s\tmiddle = %s\n", dump_data(*hole), dump_data(*middle));
            dump_pointer("", left, right - left + 1);
        }
#endif
        if (middle < hole) right = hole - 1;
        else if (hole < middle) left = hole + 1;
        else break;     // hole == middle
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) {
        fprintf(OUT,"search_median() --> %s\n", dump_data(*middle));
        dump_pointer("sorted array", base, nmemb);
    }
#endif
    return  *middle;
}

/* search a pivot in an array   */
void *pivot_array(void *base, size_t nmemb, size_t size, size_t pickup, int (*compare)(const void *, const void *))
{
#ifdef  DEBUG
    char    tmp1[32], tmp2[32], tmp3[32];
    if (trace_level >= TRACE_DUMP)
        fprintf(OUT, "pivot_array(base, nmemb = %s, size = %s, pickup = %s, compare())\n"
            , dump_size_t(tmp1, nmemb), dump_size_t(tmp2, size), dump_size_t(tmp3, pickup));
    assert(base != NULL);
    assert(nmemb > 1);
    assert(size != 0);
    assert(pickup != 0);
    assert(compare != NULL);
#endif
    void *rtn;
    size_t  distance = (size_t)(nmemb / pickup);      // distance of elements
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "distance = %s\n", dump_size_t(NULL, distance));
#endif
    distance *= size;     // size in byte
#define first   ((char *)base)
    char *p = first + (nmemb * rand() / RAND_BASE / pickup) * size;  // 1st pick up point
#ifdef  DEBUG
    char tmp[16], buffer[pickup * sizeof(tmp)]; *buffer = '\0';
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
    if (trace_level >= TRACE_DUMP) {
        fprintf(OUT, "Median of %s in %s pivot_array() :%s --> %s\n"
        , dump_size_t(tmp1, pickup), dump_size_t(tmp2, nmemb), buffer, dump_data(rtn));
    }
#endif
    return rtn;
}
