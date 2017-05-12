/*
 * stable_sort.c
 *
 *  Created on: 2016/06/14
 *      Author: leo
 */

#include    <math.h>
#include "sort.h"

static void (*sort)(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) = QMI_sort;
size_t length;

/* Stable sort for pointer array */

typedef struct {
    size_t  index;      // index in a void **idxtbl
    void    *data;   // addres of element
} POINTER_INDEX;

static int      (*comp_p)(const void *, const void *);

static int compare_data(const void *p1, const void *p2) {
    return  comp_p(((const POINTER_INDEX *)p1)->data, ((const POINTER_INDEX *)p2)->data);
}

static int compare_index(const void *p1, const void *p2) {
    return  ((POINTER_INDEX *)p1)->index -  ((POINTER_INDEX *)p2)->index;
}

void stable_pointer(void **idxtbl, size_t nmemb, int (*compare)(const void *, const void *))
{
    if (nmemb <= 1) return;
    POINTER_INDEX *tbl = calloc(nmemb, sizeof(POINTER_INDEX));
    if (tbl != NULL) {
        /*  store idxtbl to POINTER_INDEX[] */
        void **p = idxtbl;
        POINTER_INDEX *t = tbl;
        for (size_t i = 0; i < nmemb; t++) {
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "tbl[%s].data = %p %s\n"
                , dump_size_t(i), *p, (char *)*p);
#endif
            t->index = i++;
            t->data = *p++;  // may be gotten by malloc()
        }
        /*  sort    */
        sort(tbl, nmemb, sizeof(POINTER_INDEX), compare_data);   // sort array
#ifdef DEBUG
        if (trace_level >= TRACE_DUMP)
            for (size_t i = 0; i < nmemb; t++)
                fprintf(OUT, "tbl[%s].data = %p %s\n", dump_size_t(i), *p, (char *)*p);
#endif
        /*  sort to be stable and reoder idxtbl */
        POINTER_INDEX *start = tbl;
        t = start;
        for (size_t i = 1; i < nmemb; i++) {
            if (compare(t->data, start->data)) {
                sort(start, t - start, sizeof(POINTER_INDEX), compare_index);
                start = t;
            }
        }
        sort(start, t - start, sizeof(POINTER_INDEX), compare_index);
        /* reorder idxtbl   */
        p = idxtbl;
        t = tbl;
        for (size_t i = 0; i < nmemb; i++) {
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "tbl[%s] = %s, %p %s\n"
                , dump_size_t(i), dump_size_t(t->index), t->data, (char *)(t->data));
#endif
            *p++ = t++->data;
        }
        /* done */
        free(tbl);
    }
}

/***    Stable sort for continuous array    ***/
static int compare_adrs(const void *p1, const void *p2) {
    return  p1 - p2;
}

static int compare_string(const void *p1, const void *p2)
{
    return strcmp(* (char * const *) p1, * (char * const *) p2);
}

void stable_array(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *))
{
    if (nmemb <= 1) return;
    void **idxtbl = make_index(base, nmemb, size);
    if (idxtbl != NULL) {
        sort(idxtbl, nmemb, sizeof(void *), compare_string);
        void **start = idxtbl, **p = start;
        for (int i = 0; i < nmemb; i++) {
            if (compare(++p, *start)) {
                sort(start, p - start, sizeof(void *), compare_adrs);
                start = p;
            }
        }
        sort(start, p - start, sizeof(void *), compare_adrs);
        unindex(base, idxtbl, nmemb, size);
        free(idxtbl);
    }
}
