/*
 ============================================================================
 Name        : @main.c
 Author      : Takeuchi Leorge <qmisort@gmail.com>
 Version     : 1.0.0
 Copyright   : free
 Description : Test sorting algorithms in C language Ansi-style.
 ============================================================================

 Compile option
     Release : -O3 -Wall -c -fmessage-length=0 -std=c99
     Debug   : -DDEBUG -O0 -g3 -Wall -c -fmessage-length=0 -std=c99

 Link option
     -lm
 */
#include    <ctype.h>
#include    <math.h>
#include    <sys/time.h>
#include    <time.h>
#include    <unistd.h>
#include    "sort.h"

/*
 * Uncomment or define symbols to examine algorithm.
 */
//#define   HEAP_SORT 1
//#define   INSERTION_SORT 1

/****   Public  ****/
PivotChoice  pivot_scheme = RANDOM3;
Trace   trace_level = TRACE_NONE;   // to debug
int     pivot_number = 5;
size_t  random_number;              // variable type is same to nmemb
RANDOM_DEPTH random_depth = 3;

size_t  medium_boundary = 1000;     //  nmemb to alternate to merge sort.
void    (*medium_func)() = qsort_middle;
size_t  small_boundary = 8;         //  nmemb to alternate from merge sort.
void    (*small_func)() = insert_pointer;

size_t set_random(void) {
	size_t	rtn = rand();
#ifdef  DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "random = %ld\n", rtn);
#endif
	return	rtn;
}

long    qsort_called, qsort_comp_str, qsort_moved, search_pivot;  // counters

/****   Private ****/

// You can edit this function for other data structure.
int     length_compare;
static int cmpstring(const void *p1, const void *p2)    // Function to compare
{
    int rtn = strncmp(p1, p2, length_compare);
#ifdef  DEBUG
    qsort_comp_str++;
    if (trace_level >= TRACE_COMPARE) fprintf(OUT, "cmpstring(%s, %s) --> %d\n", (char *)p1, (char *)p2, rtn);
#endif
    return  rtn;
}

typedef enum {
    DUMMY,
    INDEX_QSORT3,
    POINTER_QSORT3,
    MERGE_ARRAY,
    MERGE_INDEX,
    MERGE_POINTER,
    SWAP_FIRST,
    SWAP_MIDDLE,
    SWAP_KR,
    SWAP_MED3,
    HOLE_LAST,
    HEAP_SORT,
    TREE_SORT,
    INSERT_SORT,
    INSERT_POINTER,
    BUBBLE_ARRAY,
    BUBBLE_POINTER,
    COCKTAIL_ARRAY,
    COCKTAIL_POINTER,
    MERGE_HYBRID_INDEX,
    MERGE_HYBRID_POINTER,
    HYBRID_ARRAY,
    HYBRID_INDEX,
    POINTER_SORT,
    STABLE_ARRAY,
    STABLE_POINTER,
} TEST_TYPE;

typedef struct {
    int         option;             // command option
    TEST_TYPE   type;               // see above.
    const char *name;               // function name to display
    void        (*sort_function)();
    bool        pointer_sort;       // True : pointer sort      False : array sort or index sort
    const char  *description;
} INFO;

// Estimate time
static long elapsed_time(const char *comment, int skip)
{
    int percent = 0;    // return value
    int datacount = end_timer(skip);
    fprintf(OUT, "\tusec = %.0f", usec);
#ifdef  DEBUG
    if (trace_level >= TRACE_COUNT) fprintf(OUT, "\tcall = %ld\tcompare = %ld\tcopy = %ld"
        , qsort_called, qsort_comp_str, qsort_moved);
    if (datacount) {}   // dummy statement to avoid warning
#else
    if (datacount > 1) {
        percent = 100. * esd / usec + .5;
        fprintf(OUT, "\tspread = %.0f\t%3d %%", esd, percent);
        int     i;
        long    *tlen = microsec;
        fprintf(OUT, " [%ld]", *tlen++);
        for (i = 0; i < skip - 1; i++) fprintf(OUT, " [%ld]", *tlen++);
        for (; i <= datacount; i++, tlen++) {
            if (i == peak - 1) fprintf(OUT, " (%ld)", *tlen);
            else fprintf(OUT, " %ld", *tlen);
        }
    }
#endif
    fprintf(OUT, "\n");
    return  percent;
}

// examine
char *sorted_array;
size_t memsize;
static int check_result(const char *name, const void *workbuffer)
{
    int rtn = memcmp(workbuffer, sorted_array, memsize);
    if (rtn) fprintf(stderr, "Not sorted in %s\n", name);
    return  rtn;
}

static int  cmp_info(const void *t1, const void *t2) {  // to sort test[] in main().
    return ((INFO*)t1)->type - ((INFO*)t2)->type;
}

void dummy_sort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)) {
}

/*****************************************************************************/
int main(int argc, char *argv[])

{
    extern int getopt(int argc, char * const argv[], const char *optstring);
    extern  int optind;
    extern  char *optarg;
    INFO *info, test[] = {  // alphabetic order in symbol names of enum for each block.
            // simple in-place sort.
            {'3', SWAP_MED3, "qsort_med3()", qsort_med3, FALSE,
                "quick sort : pivot is median of 3 elements with swaps."},
            {'a', HYBRID_ARRAY, "hybrid_array()", hybrid_array, FALSE,
                "hybrid sorting of quick sort : array sorting."},
#ifdef  DEBUG
            {'b', BUBBLE_ARRAY, "bubble_sort()", bubble_sort, FALSE,
                "Bubble sort : with no swaps."},
//          {'B', BUBBLE_POINTER, "bubble_pointer()", bubble_pointer, TRUE,
//              "Bubble sort : pointer sorting."},
            {'c', COCKTAIL_ARRAY, "cocktail_sort()", cocktail_sort, FALSE,
                "Cocktail sort : bidirectional bubble sort."},
//          {'C', COCKTAIL_POINTER, "cocktail_pointer()", cocktail_pointer, TRUE,
//              "Cocktail sort : pointer sorting."},
#endif
            {'d', SWAP_MIDDLE, "qsort_middle()", qsort_middle, FALSE,
                "quick sort : pivot is the miDDle element with swaps."},
            {'e', MERGE_HYBRID_POINTER, "merge_phybrid(*)", merge_phybrid, TRUE,
                "hybrid sorting of mErgE sort : pointer sorting."},
            {'E', MERGE_HYBRID_INDEX, "merge_hybrid()", merge_hybrid, FALSE,
                "hybrid sorting of mErgE sort : index sorting."},
            {'f', SWAP_FIRST, "qsort_first()", qsort_first, FALSE,
                "quick sort : pivot is the First element with swaps."},
            {'G', MERGE_POINTER, "merge_pointer(*)", merge_pointer, TRUE,
                "merGe sort : pointer sorting."},
            {'h', HOLE_LAST, "qsort_hole()", qsort_hole, FALSE,
                "quick sort : prototype of Hole scheme."},
#ifdef  DEBUG
            {'H', HEAP_SORT, "heap_sort()", heap_sort, FALSE,
                "Heap sort."},
            {'i', INSERT_SORT, "insert_sort()", insert_sort, FALSE,
            "Insertion sort : linear search."},
//          {'I', INSERT_POINTER, "insert_pointer()", insert_pointer, TRUE,
//          "Insertion sort : pointer sorting."},
            {'K', SWAP_KR, "qsort_kr()", qsort_kr, FALSE,
                "quick sort : pivot is the middle element with swaps in K&R style."},
#endif
            {'m', MERGE_ARRAY, "merge_sort()", merge_sort, FALSE,
                "Merge sort : array sorting."},
            {'M', MERGE_INDEX, "merge_index()", merge_index, FALSE,
                "Merge sort : index sorting."},
            {'P', POINTER_QSORT3, "qsort3_pointer()", qsort3_pointer, TRUE,
                "Pointer sorting of qsort(3) to measure sorting time in index sorting."},
            {'p', POINTER_SORT, "hybrid_pointer(*)", hybrid_pointer, TRUE,
                "hybrid sorting of quick sort : Pointer sorting."},
            {'s', STABLE_ARRAY, "stable_array()", stable_array, FALSE,
                "Stable hybrid sorting of quick sort : array sorting."},
            {'S', STABLE_POINTER, "stable_pointer(*)", stable_pointer, TRUE,
                "Stable hybrid sorting of quick sort : pointer sorting."},
#ifdef  DEBUG
            {'t', TREE_SORT, "tree_sort()", tree_sort, FALSE,
            "Tree sort : median node tree."},
#endif
            {'U', DUMMY, "dummy_sort()", dummy_sort, FALSE,
                "dUmmy sort : do nothing to cause error."},
            {'X', INDEX_QSORT3, "qsort3_index()", qsort3_index, FALSE,
                "indeX sorting of qsort(3) to reduce copies."},
            {'x', HYBRID_INDEX, "hybrid_index()", hybrid_index, FALSE,
                "hybrid sorting of quick sort : indeX sorting."},
    };

    // prepare to analyze command arguments
    qsort(test, sizeof(test) / sizeof(INFO), sizeof(INFO), cmp_info);   // sort a table according to enumeration
    char    *p, optstring[sizeof(test) / sizeof(INFO) + 100];   // enough long
    size_t  i;
    memset(optstring, 0, sizeof(optstring));
    for (info = test, p = optstring, i = 0; i++ < sizeof(test) / sizeof(INFO); info++) *p++ = (char)info->option;
    strcat(optstring, "?A:D:L:l:N:oR:r:T:V:v:Y:Z:");
    /**** Analyze command arguments ****/
    char    *prg = strrchr(argv[0], '/') + 1;   // Program name without path
    if (prg == NULL) prg = argv[0];
    char    *fin = NULL;            // file name to input
    typedef long INDEX;
    INDEX   index = 0, idx;
    bool    print_out = FALSE;
    bool    IsPercentB = FALSE;
    int     opt, repeat_count, buffer_length = 1;
    double  Loption = 0.0;
    size_t  nmemb = 31, size = 0;
    int     limit = 2;      // boundary precent to pass

#ifdef  DEBUG   // Default
    repeat_count = 1;
#else
    repeat_count = 12;
#endif
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case '?':
            printf("Usage : %s [options]"
                " [-Z element_size] [-R repeat_count] [-NL nmemb] [-AV func] [-v choice]"
#ifdef  DEBUG
                    " [-D trace_level]"
#endif
                    " [filename]\nOptions :\n"
                    "\t-? : show this help.\n"
                    "\n"
                    , prg);
            for (idx = 1, info = test, i = 0; i++ < sizeof(test) / sizeof(INFO); idx <<= 1, info++) {
                printf("\t-%c : %s\n", info->option, info->description);
            }
            puts(
                "\n"
                "\t-A : Algorithm when N is medium.\n"
                "\t       3 - quick sort with swaps. pivot is median of first, middle and last elements.\n"
                "\t       M - index sorting of Merge sort.\n"
                "\t       m - array sorting of Merge sort.\n"
				"\t       h - Hybrid merge sort (default).\n"
                "\t-l : boundary to change algorithm when N is smaLL (default is 8).\n"
                "\t-L : boundary to change algorithm from N is Large (default is 1000).\n"
                "\t       If the value is less than 0 then value means depth.\n"
                "\t       Else if % is added then value means depth in percent.\n"
                "\t-N : Number of members (default is 31).\n"
                "\t-o : print Out the last result.\n"
                "\t-r : Random depth in recusion (default is 3)\n"
                "\t-R : Repeat count of "
#ifndef DEBUG
            	"sampling to calculate Stdev (default is 12).\n"
#else
               	"test (default is 1).\n"
#endif
#ifndef DEBUG
                "\t-T : uncerTainTy percenT to pass a test (default is 2 [%]).\n"
#endif
                "\t-v : number of elements to select a pivot for -V v option (default is 5).\n"
                "\t-V : algorithm to choose a piVot in quick sort.\n"
                "\t       r - Random element.\n"
                "\t       3 - median of random 3 elements (default).\n"
                "\t       l - median of random Log2(n) elements.\n"
                "\t       v - median of various elements. cf. -v option\n"
                "\t-Y : cYclic work buffer length.\n"
                "\t-Z : siZe of an array element.\n"
#ifdef DEBUG
                "\n"
                "\t-D : trace level for Debugging.\n"
                "\t       1 - Counts.\n"
                "\t       2 - and actions.\n"
                "\t       3 - and copies.\n"
                "\t       4 - and comparisons.\n"
                "\t       5 - and Others."
#endif
            );
            return EXIT_SUCCESS;
            break;
        case 'D':   // ignored in Release
            trace_level = strtoul(optarg, NULL, 0);
            break;
        case 'L':
            p = &optarg[strlen(optarg) - 1];
            if (*p == '%') IsPercentB = TRUE;
            Loption = atof(optarg);
            break;
        case 'l':
            small_boundary = (int)strtoul(optarg, NULL, 0);
            break;
        case 'N':
            nmemb = strtoul(optarg, NULL, 0);
            break;
        case 'o':
            print_out = TRUE;
            break;
        case 'R':
            repeat_count = strtoul(optarg, NULL, 0);
            break;
        case 'r':
            random_depth = strtol(optarg, NULL, 0);
            if (random_depth < 0) random_depth = 0;
            break;
        case 'T':
            limit = atoi(optarg);
            break;
        case 'v':
            pivot_number = (int)strtoul(optarg, NULL, 0) | 1;   // make an odd number
            break;
        case 'Y':
            buffer_length = (int)strtoul(optarg, NULL, 0);
            break;
        case 'Z':
            size = (size_t)strtoul(optarg, NULL, 0);
            break;
        case 'A':   // Algorithm when nmemb is medium for Array sort
            switch(*optarg) {
            case '3':
            	medium_func = qsort_med3;	// with swaps
                break;
            case 'm':
                medium_func = merge_sort;
                break;
            case 'M':
            	medium_func = merge_index;
                break;
            case 'h':
            	medium_func = merge_hybrid;
                break;
            default:
                fprintf(stderr, "Illegal value \"%s\" for -A option.\n", optarg);
                return EXIT_FAILURE;
                break;
            }
            break;
        case 'V':   // Algorithm to Find a pivot while N is large in quick_sort()
            switch(*optarg) {
            case '3':
                pivot_scheme = RANDOM3;
                break;
            case 'l':
                pivot_scheme = LOG2;
                break;
            case 'r':
                pivot_scheme = RANDOM;
                break;
            case 'v':
                pivot_scheme = VARIOUS;
                break;
            default:
                fprintf(stderr, "Illegal value \"%s\" for -P option.\n", optarg);
                return EXIT_FAILURE;
                break;
            }
            break;
        default:    // select sorting algorithm
            for (info = test, i = idx = 0; idx < sizeof(test) / sizeof(INFO); idx++, info++) {
                if (info->option == opt) {
                    index |= 1 << idx;
                    break;
                }
            }
            break;
        }
    }
    if (argc > optind) fin = argv[optind];
#ifdef  DEBUG
    if (trace_level > TRACE_NONE) {
        fprintf(OUT, "arguments :");
        char **p = argv;
        for (int i = 1; i++ < argc;) fprintf(OUT, " %s", *++p);
        fprintf(OUT, "\n");
    }
#endif

    /**** Read data ****/

    // Open file
    FILE *fp = fin ? fopen(fin, "r") : stdin;   // open file or use stdin
    if (fp == NULL) {
        perror(NULL);       // Output system error message to OUT.
        return EXIT_FAILURE;
    }

    // Read first line to get a record size
    char read_buff[1024];
    if (! fgets(read_buff, sizeof(read_buff) - 1, fp)) return EXIT_SUCCESS; // EOF
    int buflen = strlen(read_buff); // with EOL(End Of Line : LF or CR+LF)
    if (! isprint((int)read_buff[0])) {         // control character CR, LF, TAB etc.
        fprintf(stderr, "Use printable characters.\n");
        return EXIT_FAILURE;
    } else if (size == 0) {
        length_compare = size = buflen + 1;
    } else if (buflen >= size - 1) {
        fprintf(stderr, "Don't use too long data.\n");
        return EXIT_FAILURE;
    }
    else {
        length_compare = size;
    }

    // allocate data area
    char    *srcbuf;          // Work buffer to store all input data
    if ((srcbuf = calloc(nmemb, size)) == NULL)
    {   /* Can't get work area  */
        perror(NULL);
        return EXIT_FAILURE;
    }

    // read data
    i = 0;
    p = srcbuf;
    buflen--;
    do {
        char *q;
        if ((q = strrchr(read_buff, '\n')) != NULL) *q = '\0';  // chop LF
        if ((q = strrchr(read_buff, '\r')) != NULL) *q = '\0';  //      CR
        strncpy(p, read_buff, buflen);
        i++;
        if (! fgets(read_buff, sizeof(read_buff), fp)) break;       // EOF
        p += size;
    } while (i < nmemb);
    if (i <= 1) return EXIT_SUCCESS;
    memsize = (nmemb = i) * size;
    srcbuf = realloc(srcbuf, memsize);

    /***** Prepare *****/

    int skip = repeat_count > 1 ? 1: 0;
    if (Loption < 0) {  // depth for hybrid sorting boundary
        medium_boundary = nmemb / pow(2.0, -Loption / (IsPercentB ? 100.0 / log2(nmemb): 1.0));
    }
    else if (Loption > 0) { // size
        medium_boundary = IsPercentB ? (nmemb * Loption) / 100: Loption;
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP && Loption != 0) fprintf(OUT, "medium_boundary = %ld\n", medium_boundary);
#endif

//#define   BUFCYCLE    2
    char    **workarea = (char **)malloc(sizeof(char *) * buffer_length);
    char    *workbuff = *workarea;
    int     buffid;
    for (buffid = 0; buffid < buffer_length;) {
        if ((workbuff = calloc(nmemb, size)) == NULL)
        {   /* Can't get work area  */
            perror(NULL);
            return EXIT_FAILURE;
        }
        workarea[buffid++] = workbuff;
    }
#define NextBuffer  workarea[buffid = (buffid + 1) % buffer_length]

    /***** sort with built-in qsort(3) to get sorted array as a sorted_array result. *****/

    if ((sorted_array = calloc(nmemb, size)) == NULL)
    {   /* Can't get work area  */
        perror(NULL);
        return EXIT_FAILURE;
    }
    else {
        const char *name = "qsort(3)";
        const char *description = "Library call. Perhaps, this is a merge sort.";
QSORT:
        fprintf(OUT, "%s", name);
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, " nmemb = %ld\n", nmemb);   // for nmemb.awk
#endif
        begin_timer(repeat_count);
        for (int i = 0; i < repeat_count; i++) {
            workbuff = NextBuffer;
            memcpy(workbuff, srcbuf, memsize);      // memory copy : sorted_array <-- srcbuf
            qsort_comp_str = qsort_called = qsort_moved = 0;    // reset all of counters
            start_timer();
            qsort(workbuff, nmemb, size, cmpstring);
            stop_timer();
        }
        if (elapsed_time(description, skip) > limit) goto QSORT;
    }
    memcpy(sorted_array, workbuff, memsize);

    /* test */

    void **idxtbl;
    long *cache, *clear;
    unsigned seed;
//    struct timespec tp;
//    int rtn = clock_gettime(CLOCK_BOOTTIME,&tp);
    FILE *clk;
    if ((clk = fopen("/proc/uptime", "r")) != NULL) {
        float f1, f2;
        if (fscanf(clk, "%f %f", &f1, &f2)) seed = (unsigned)(f2*100.);    // 10 nano seconds
        fclose(clk);
    }
    else seed = (unsigned)time(NULL);   // fail safe
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "random seed = %d\n", seed);
#endif
    srand(seed);
    random_number = set_random();
    for (info = test,idx = 1; index != 0; idx <<= 1, info++) {
        if (index & idx) {
            index &= ~idx;  // clear bit
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "Start %s : %s\n", info->name, info->description);
#else
REDO:
#endif
#ifdef DEBUG
			if (trace_level == TRACE_NONE)	// don't add a semicolon ";"
#endif
			fprintf(OUT, "%s", info->name);
#define ENOUGH  4000000L
            cache = calloc(sizeof(long), ENOUGH);
            clear = cache;  // really enough?
            for (long l = 0; l < ENOUGH; l++) *clear++ = -1L;
            free(cache);
#ifndef DEBUG
            begin_timer(repeat_count);
#endif
            for (int i = 0; i < repeat_count; i++) {
#ifdef DEBUG
            	begin_timer(1);
#endif
                qsort_comp_str = qsort_called = qsort_moved = 0;    // reset all of counters
                workbuff = NextBuffer;
                memcpy(workbuff, srcbuf, memsize);  // memory copy : workbuff <-- srcbuf
                if (info->pointer_sort) {
                    idxtbl = make_index(workbuff, nmemb, size); // make an index table from srcbuf
                    if (idxtbl == NULL) return EXIT_FAILURE;
                    start_timer();
                    (*info->sort_function)(idxtbl, nmemb, cmpstring);
                    stop_timer();
                    unindex(workbuff, idxtbl, nmemb, size);     // restore index table to workbuff
                    free(idxtbl);
                }
                else {  // array sort or index sort.
                    start_timer();
                    (*info->sort_function)(workbuff, nmemb, size, cmpstring);
                    stop_timer();
                }
#ifdef  DEBUG
                if (trace_level != TRACE_NONE) fprintf(OUT, "%s", info->name);
                elapsed_time(info->description, 0);
#endif
            }
#ifndef DEBUG
            if (elapsed_time(info->description, skip) > limit) goto REDO;
#endif
            if (check_result(info->name, workbuff)) {   // error
                print_out = TRUE;
                break;
            }
        }
    }

    if (print_out) for (p = workbuff, i = 0; i++ < nmemb; p += size) fprintf(OUT, "%s\n", p);

    return EXIT_SUCCESS;
}
