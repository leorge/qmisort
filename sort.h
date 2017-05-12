/*
 * isort.h
 *
 *  Created on: 2012/10/01
 *      Author: Leorge Takeuchi <qmisort@gmail.com>
 */

#ifndef SORT_H_
#define SORT_H_
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define OUT     stdout

typedef enum {  // trace lelve
    TRACE_NONE,
    TRACE_COUNT,    // count up
    TRACE_DUMP,     // dump behavior
    TRACE_MOVE,     // dump memcpy()
    TRACE_COMPARE,  // dump strcmp()
    TRACE_DEBUG
} Trace;

#define bool    int
#define FALSE   0
#define TRUE    (! FALSE)
#define MAX_BIT     (8 * sizeof(size_t))    // bit size of nmemb
#define RAND_BASE   ((size_t)RAND_MAX+1)

typedef unsigned char       byte;

/* for glibc */
#ifndef __compar_d_fn_t
typedef int (*__compar_d_fn_t) (const void *, const void *, void *);
#endif

/***** variables *****/

extern size_t       *gaplist;           // main.c
extern size_t       INS;                // main.c
extern size_t       threshold;          // main.c
extern size_t       small_boundary;     // main.c
extern size_t       single1, median1, median3, median5, medianL;   // main.c
extern int          gap_count;          // main.c
extern bool         reuse_random;       // main.c

extern long qsort_moved, qsort_comp_str, qsort_called, search_pivot;    // main.c
extern Trace    trace_level;            // main.c

/***** Functions *****/
extern size_t set_random(void);
extern void (*small_func)();
extern void (*medium_func)();

// array sort
void    asymm_qsort     (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    dual_pivot      (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    merge_sort      (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    pivot_hole      (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    QM_sort         (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    QMI_sort        (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_3way      (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_first     (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_kr        (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_med3      (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_middle    (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    quick_asymm     (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    quick_hole      (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    quick_hybrid    (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    quick_pivot     (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    quick_random    (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    quick_secure    (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    stable_array    (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));

// index sort
void    merge_hybrid    (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    merge_index     (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));

// pointer sort
void    ai_sort         (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    bubble_sort     (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    cocktail_sort   (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    comb_sort       (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    heap_top        (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    heap_bottom     (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    insert_binary   (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    insert_linear   (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    qsort3_indr     (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
//void    quick_phybrid   (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    quick_pmiddle   (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    merge_phybrid   (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    merge_pointer   (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    rabbit_sort     (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    shell_sort      (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    stable_pointer  (void *base[], size_t nmemb, int (*compare)(const void *, const void *));

// search pivot
void    *median_of_5(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    *pivot_array(void *base, size_t nmemb, size_t size, size_t pickup, int (*compare)(const void *, const void *));

// others
void    **make_index(void *array1d, size_t nmemb, size_t size);
void    unindex(void *array1d, void *idxtbl[], size_t nmemb, size_t size);

// for debug
const char  *dump_data(const void *data);
const char  *dump_size_t(size_t val);
void        dump_copy(void *dst, const void *src);
void        dump_array(const char *msg, const void *head, size_t left, size_t middle, size_t right, size_t size);
void        dump_pointer(char *msg, void *head[], size_t length);
void        dump_rate(size_t anterior, size_t posterior);

#endif /* SORT_H_ */
