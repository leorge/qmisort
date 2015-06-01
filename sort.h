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
	MIDDLE,
    RANDOM,
    RANDOM3,
    VARIOUS,
    LOG2,
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

#define DIGIT_WIDTH 4       // nibble

#define MAX_SIZE    (1 << DIGIT_WIDTH)
#define MASK_DIGIT  (MAX_SIZE - 1)

typedef unsigned long long INSERT_INDEX;    // 4 bits/element * 16 elements = 64 bits = bit width of long long

typedef unsigned char       byte;

/* for glibc */
#ifndef __compar_d_fn_t
typedef int (*__compar_d_fn_t) (const void *, const void *, void *);
#endif

/***** variables *****/

extern size_t       INS;                // main.c
extern double       random_number;      // main.c
extern size_t       medium_boundary;    // main.c
extern size_t       small_boundary;     // main.c
extern int          pivot_number;       // main.c
extern PivotChoice  pivot_scheme;       // main.c
extern bool         ispointer;          // index_sort.c
extern double       usec, esd;          // timer.c
extern long         *microsec;          // timer.c
extern int          peak;               // timer.c

#ifdef DEBUG
extern long qsort_moved, qsort_called;  // main.c
extern Trace    trace_level;            // main.c
#endif

/***** Functions *****/
extern void set_random(void);
extern void (*small_func)();
extern void (*medium_func)();

// array sorting
void    hybrid_array    (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    bubble_sort     (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    cocktail_sort   (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    heap_sort       (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    insert_sort     (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    merge_sort      (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_first     (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_hole      (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_kr        (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_med3      (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort_middle    (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    quick_sort      (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    stable_array    (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    tree_sort       (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));

// index sorting
void    merge_hybrid    (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    merge_index     (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    hybrid_index    (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
void    qsort3_index    (void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));

// pointer sorting
void    bubble_pointer  (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    cocktail_pointer(void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    insert_pointer  (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    qsort3_pointer  (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    merge_pointer   (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    merge_phybrid   (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    hybrid_pointer  (void *base[], size_t nmemb, int (*compare)(const void *, const void *));
void    stable_pointer  (void *base[], size_t nmemb, int (*compare)(const void *, const void *));

// search pivot
void    *pivot_array(void *base, size_t nmemb, size_t size, size_t pickup, int (*compare)(const void *, const void *));
void    *pivot_pointer(void **base, size_t nmemb, size_t pickup, int (*compare)(const void *, const void *));

// others
const char  *dump_data(const void *data);
void    **make_index(void *array1d, size_t nmemb, size_t size);
void    unindex(void *array1d, void *idxtbl[], size_t nmemb, size_t size);

// timer
bool    begin_timer(int count);
void    start_timer();
long    stop_timer();
int     end_timer(int skip);

// for debug
#ifdef DEBUG
void    dump_array(const char *msg, const void *head, size_t nmemb, size_t size);
char    *dump_index(char *buf, INSERT_INDEX index, int length);
void    dump_pointer(char *msg, void *head[], size_t length);
void    dump_rate(size_t anterior, size_t posterior);
#endif

#endif /* SORT_H_ */
