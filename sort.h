/*
 * isort.h
 *
 *  Created on: 2012/10/01
 *      Author: leo
 */

#ifndef SORT_H_
#define SORT_H_
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define OUT     stdout

typedef enum {  // Pivot choice
    PIVOT_RANDOM,
    PIVOT_RANDOM3,
    PIVOT_VARIOUS,
    PIVOT_LOG2,
} PivotChoice;

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

#define DIGIT_WIDTH 4       // nibble

#define MAX_SIZE    (1 << DIGIT_WIDTH)
#define MASK_DIGIT  (MAX_SIZE - 1)

typedef long RANDOM_DEPTH;
typedef unsigned long long INSERT_INDEX;    // 4 bits/element * 16 elements = 64 bits = bit width of long long

typedef unsigned char       byte;

/* for glibc */
#ifndef __compar_d_fn_t
typedef int (*__compar_d_fn_t) (const void *, const void *, void *);
#endif

/***** variables *****/

extern size_t		*gaplist;			// main.c
extern size_t       INS;                // main.c
extern size_t       random_number;      // main.c
extern size_t       medium_boundary;    // main.c
extern size_t       small_boundary;     // main.c
extern int			gap_count;			// main.c
extern int          pivot_number;       // main.c
extern PivotChoice  pivot_type;       	// main.c
extern RANDOM_DEPTH random_depth;       // main.c
extern bool         reuse_random;       // main.c
extern bool         ispointer;          // index_sort.c

#ifdef DEBUG
extern long qsort_moved, qsort_comp_str, qsort_called, search_pivot;	// main.c
extern Trace    trace_level;            // main.c
#endif

/***** Functions *****/
extern size_t set_random(void);
extern void (*small_func)();
extern void (*medium_func)();

// array sort
void    merge_sort      (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_first     (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_hole      (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_hybrid    (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_kr        (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_med3      (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_middle    (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_random    (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_random3   (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    stable_array    (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));

// index sort
void    merge_hybrid    (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    merge_index     (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));

// pointer sort
void    bubble_sort     (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    comb_sort       (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    heap_sort       (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    heap2_sort      (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    insert_binary   (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    insert_sort     (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    qsort3_indr     (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    qsort_phybrid   (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    qsort_pointer   (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    merge_phybrid   (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    merge_pointer   (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    shellsort       (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    stable_pointer  (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    step_sort       (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    stepup_sort     (void *base[], size_t nmemb, int (*compare)(const void *, const void *));

// search pivot
void    *pivot_array(void *base, size_t nmemb, size_t size, size_t pickup, int (*compare)(const void *, const void *), size_t random);
void    *pivot_pointer(void **base, size_t nmemb, size_t pickup, int (*compare)(const void *, const void *), size_t random);

// others
const char  *dump_data(const void *data);
void    **make_index(void *array1d, size_t nmemb, size_t size);
void    unindex(void *array1d, void *idxtbl[], size_t nmemb, size_t size);

// for debug
#ifdef DEBUG
void    dump_array(const char *msg, const void *head, size_t nmemb, size_t size);
char    *dump_index(char *buf, INSERT_INDEX index, int length);
void    dump_pointer(char *msg, void *head[], size_t length);
void    dump_rate(size_t anterior, size_t posterior);
#endif

#endif /* SORT_H_ */
