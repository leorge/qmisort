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
#include    <sys/resource.h>
#include    <time.h>
#include    <unistd.h>
#include	<limits.h>
#include    "sort.h"

/*
 * Uncomment or define symbols to examine algorithm.
 */

/****   Public  ****/
Trace   trace_level = TRACE_NONE;   // to debug
int     pivot_number = 5;			// for -v option
size_t  median1 = 8;				// if N <= pivot_1 then middle element is a pivot
size_t  median3 = 31;				// if N <= pivot_3 then median of 3 is a pivot
size_t  median5 = 1023;				// if N <= Pivot_5 then median of 5 is a pivot else median of log2(N).
size_t  random_number;              // variable type is same to nmemb
RANDOM_DEPTH random_depth = 3;
bool    reuse_random = FALSE;       // reuse random number or not

size_t  threshold = 0;        //  nmemb to alternate to merge sort.
void    (*medium_func)() = merge_hybrid;
size_t  small_boundary = 8;         //  nmemb to alternate from merge sort.
void    (*small_func)() = ai_sort;

size_t  *gaplist = NULL;
int     gap_count = 0;

size_t set_random(void) {
    size_t  rtn = rand();   // [0..RAND_MAX]
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "random = %ld  (%lf)\n", rtn, (double)rtn / RAND_BASE);
#endif
    return  rtn;
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
    DEBUG_SORT,
    DUMMY_SORT,
    MERGE_ARRAY,
    MERGE_INDR,
    MERGE_HYBRID,
    SWAP_FIRST,
    SWAP_MIDDLE,
    SWAP_KR,
    SWAP_MED3,
    QSORT_HOLE,
    QSORT_RANDOM,
    QSORT_HYBRID,
    QSORT_STABLE,
    QSORT_DUAL,
} SORT_TYPE;

typedef struct {
    int         option;             // command option
    SORT_TYPE   type;               // see above.
    const char *name;               // function name to display
    void        (*sort_function)();
    const char  *description;
} INFO;

// Estimate time in microseconds

static struct timeval   start_time, core_time;  // time stamp
//#define RUSAGE 1
#ifdef  RUSAGE
static long     time_from;  // to test getrusage()
static long usertime() {
    struct  rusage  usage;
    getrusage(RUSAGE_SELF, &usage);
#ifdef  DEBUG
    if (trace_level >= TRACE_DUMP)
        fprintf(OUT, "timestamp() %ld.06%ld\n", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
#endif
    return  usage.ru_utime.tv_sec * 1000000 + usage.ru_utime.tv_usec; // micro sec.
}
#endif

static void start_timer(struct timeval *from) {
    assert(from!= NULL);
    gettimeofday(from, NULL);
#ifdef  RUSAGE
    time_from = usertime();
#endif
}

static long stop_timer(struct timeval *from) {
    assert(from != NULL);
    struct timeval to;
#ifdef  RUSAGE
    long time_to = usertime();
#endif
    gettimeofday(&to, NULL);
#ifdef  RUSAGE
    fprintf(stderr, "getrusage() : %ld - %ld = %ld\n", time_to, time_from, time_to - time_from);
#endif
    long rtn = (to.tv_sec - from->tv_sec) * 1000000. + to.tv_usec - from->tv_usec;
    return  rtn;
}

static long show_result(const char *comment, long *usec, int size, int skip, double index_time) {
    int percent = 0;
#ifdef  DEBUG
    fprintf(OUT, "\tusec = %ld", *usec);
    if (trace_level >= TRACE_COUNT) {
        fprintf(OUT, "\tcall = %ld\tcompare = %ld\tcopy = %ld"
                , qsort_called, qsort_comp_str, qsort_moved);
    }
#else
    int     peak = 0;
    double  esd = 0.0, mean = 0.0;
    if (size > skip) {
        long    y, max_y = 0, n = 0;
        double  sum = 0., delta;
        for (int i = skip; i < size; n++, i++) {
            sum += (y = usec[i]);
            if (y > max_y) {
                max_y = y;
                peak = i;
            }
        }
        if (n > 1) {
            mean = (sum -= max_y) / --n;
            for (int i = skip; i < size; i++) {
                if (i != peak) {
                    delta = (double)usec[i] - mean;
                    esd += delta * delta;
                }
            }
            esd = --n ? sqrt(esd / n) : 0;
        }
        else mean = sum;    // m == 1
    }

    fprintf(OUT, "\tusec = %.0f", mean);
    if (size > 1) {
        percent = 100. * esd / mean + .5;
        fprintf(OUT, "\tspread = %.0f\t%3d %%", esd, percent);
        int     i;
        long    *tlen = usec;
        fprintf(OUT, " [%ld]", *tlen++);
        for (i = 1; i < skip; i++) fprintf(OUT, " [%ld]", *tlen++);
        for (; i < size; i++, tlen++) {
            if (i == peak) fprintf(OUT, " (%ld)", *tlen);
            else fprintf(OUT, " %ld", *tlen);
        }
    }
#endif
    if (index_time && trace_level) fprintf(OUT, " %.1f", (double)index_time / size);
    fprintf(OUT, "\n");
    return percent;
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
            // aray sort.
            {'3', SWAP_MED3, "qsort_med3()", qsort_med3,
                "quicksort : pivot is the median of 3 elements with sWaps."},
            {'d', SWAP_MIDDLE, "qsort_middle()", qsort_middle,
                "quicksort : pivot is the miDDle element with swaps."},
//            {'E', MERGE_HYBRID, "merge_hybrid()", merge_hybrid,
//                "hybrid sort of mErgE sort : indirect sort."},
            {'f', SWAP_FIRST, "qsort_first()", qsort_first,
                "quicksort : pivot is the First element with swaps."},
            {'h', QSORT_HOLE, "quick_hole()", quick_hole,
                "quicksort : prototype with Hole."},
			{'j', QSORT_DUAL, "dual_pivot()", dual_pivot,
				"quicksort : implemented dualpivot quicksort in Java."},
#ifdef  DEBUG
            {'K', SWAP_KR, "qsort_kr()", qsort_kr,
                "quicksort : pivot is the middle element with swaps in K&R style."},
#endif
            {'m', MERGE_ARRAY, "merge_sort()", merge_sort,
                "Merge sort."},
//            {'M', MERGE_INDR, "merge_index()", merge_index,
//                "Merge sort : indirect sort."},
            {'q', QSORT_HYBRID, "quick_hybrid()", quick_hybrid,
                "Quicksort : hybrid sort."},
            {'r', QSORT_RANDOM, "quick_random()", quick_random,
                "quicksort : pivot is a Random element with hole."},
            {'s', QSORT_STABLE, "stable_array()", stable_array,
                "quicksort : Stable hybrid sort."},
            {'U', DUMMY_SORT, "dummy_sort()", dummy_sort,
                "dUmmy sort : do nothing to cause error."},
    };
    INFO    test_indirect[] = { // order to show help
            {'m', 0, "merge_pointer(*)", merge_pointer, "Merge sort."},
            {'M', 1, "merge_phybrid(*)", merge_phybrid, "hybrid Merge sort."},
            {'q', 0, "quick_pmiddle(*)", quick_pmiddle, "Quicksort with hole. pivot is a middle element."},
            {'Q', 1, "quick_phybrid(*)", quick_phybrid, "Quicksort and hybrid merge sort."},
            {'s', 0, "stable_pointer(*)", stable_pointer, "Stable hybrid quicksort."},
            {'i', 0, "insert_linear(*)", insert_linear, "Insertion sort with linear search."},
            {'b', 0, "insert_binary(*)", insert_binary, "insertion sort with Binary search."},
            {'a', 0, "ai_sort(*)", ai_sort, "Accelerated linear insertion sort."},
            {'L', 0, "shell_sort(*)", shell_sort, "shell sort."},
            {'h', 0, "heap_top(*)", heap_top, "Heap sort. build a heap by top-down."},
            {'H', 0, "heap_bottom(*)", heap_bottom, "Heap sort. build a heap by bottom-up."},
#ifdef  DEBUG   // impractical below
            {'B', 0, "bubble_sort(*)", bubble_sort, "Bubble sort."},
            {'T', 0, "cocktail_sort(*)", cocktail_sort, "cockTail sort."},
            {'C', 0, "comb_sort(*)", comb_sort, "Comb sort."},
            {'r', 0, "rabbit_sort(*)", rabbit_sort, "rabbit sort."},
#endif
    };
    // prepare to analyze command arguments
    qsort(test, sizeof(test) / sizeof(INFO), sizeof(INFO), cmp_info);   // sort a table according to the SORT_TYPE.
    char    c, *p, optstring[sizeof(test) / sizeof(INFO) + 100];   // enough long
    size_t  i;
    memset(optstring, 0, sizeof(optstring));
    for (info = test, p = optstring, i = 0; i++ < sizeof(test) / sizeof(INFO); info++) *p++ = (char)info->option;
    strcat(optstring, "?A:D:I:L:l:N:pP:R:T:uV:v:Y:y:Z:");
    /**** Analyze command arguments ****/
    char    *prg = strrchr(argv[0], '/') + 1;   // Program name without path
    char    *indirect_option = NULL;
    if (prg == NULL) prg = argv[0];
    char    *fin = NULL;            // file name to input
    typedef long INDEX;
    INDEX   index = 0, idx;
    bool    print_out = FALSE;
    bool    IsPercentB = FALSE;
    int     opt, repeat_count, buffer_length = 1;
    double  Loption = 0.0;  // temporary variable for medium_boundary
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
            printf("Usage : %s [options] [filename]\n", prg);
            for (info = test, i = 0; i++ < sizeof(test) / sizeof(INFO); info++) {
                printf("\t-%c : %s\n", info->option, info->description);
            }
            puts("\t-I : Indirect sort.");
            for (info = test_indirect, i = 0; i++ < sizeof(test_indirect) / sizeof(INFO); info++) {
                printf("\t       %c %c %s\n", info->option, info->type ? '*' : '-', info->description);
            }
            puts(
                "\n"
                "\t-A : Algorithm when N is medium.\n"
                "\t       3 - conventional median-of-3 quicksort.\n"
                "\t       M - indirect Merge sort.\n"
                "\t       m - array sort of Merge sort.\n"
                "\t       h - indirect Hybrid merge sort (default).\n"
                "\t-D : Depth of recusion to generate a random number (default depth is 3)\n"
                "\t-l : boundary to change algorithm when N is smaLL (default is 8).\n"
                "\t-L : boundary to change algorithm from N is Large (default is 8192).\n"
                "\t       If the value is less than 0 then value means depth.\n"
                "\t       Else if % is added then value means depth in percent.\n"
                "\t-N : Number of members (default is 31).\n"
                "\t-p : print Out the last result.\n"
                "\t-P l,m,n : threshold to change the choice of Pivot.\n"
                "\t       N <= l -- middle element.\n"
			    "\t       N <= m -- median of 3 elements.\n"
			    "\t       N <= n -- median of 5 elements.\n"
                "\t-R : Repeat count "
#ifndef DEBUG
                "of sampling to calculate Stdev (default is 12).\n"
#else
                "to test (default is 1).\n"
#endif
#ifndef DEBUG
                "\t-T : uncerTainTy percenT to pass a test (default is 2%).\n"
                "\t-V : trace level for indexing time.\n"
                "\t       0 - indirect sort time includes indexing time).\n"
                "\t       1 - pointer sort time followed by indexing time.\n"
                "\t       2 - show every indexing time.\n"
#else
                "\t-u : reUse random number (default is FALSE).\n"
                "\t-V : trace level for Debugging.\n"
                "\t       1 - Counts.\n"
                "\t       2 - and actions.\n"
                "\t       3 - and copies.\n"
                "\t       4 - and comparisons.\n"
                "\t       5 - and Others.\n"
#endif
                "\t-v : number of elements to choose a pivot for -C v option (default is 5).\n"
                "\t-Y : cYclic work buffer length.\n"
                "\t-y : algorithm when N is small in hybrid merge sort.\n"
                "\t       same to the value of -I option followed by \"-\". Default is \"b\".\n"
                "\t-Z : siZe of an array element."
            );
            return EXIT_SUCCESS;
            break;
        case 'A':   // Algorithm when nmemb is medium for Array sort
            switch(*optarg) {
            case '3':
                medium_func = qsort_med3;   // with swaps
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
        case 'D':
            random_depth = strtol(optarg, NULL, 0);
            if (random_depth < 0) random_depth = 0;
            break;
        case 'I':
            indirect_option = optarg;
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
        case 'p':
            print_out = TRUE;
            break;
        case 'P':
        	p = strtok(optarg, ",");
        	if (p != NULL) {
        		if(*p != '\0') median1 = strtoul(p, NULL, 0);
        		p = strtok(NULL, ",");
        		if (p != NULL) {
        			if(*p != '\0') median3 = strtoul(p, NULL, 0);
            		p = strtok(NULL, ",");
            		if (p != NULL) {
            			if(*p != '\0') median5 = strtoul(p, NULL, 0);
            		}
        		}
        	}
            break;
        case 'R':
            repeat_count = strtoul(optarg, NULL, 0);
            break;
        case 'T':
            limit = atoi(optarg);
            break;
        case 'u':
            reuse_random = TRUE;
            break;
        case 'V':   // ignored in Release
            trace_level = strtoul(optarg, NULL, 0);
            break;
        case 'v':
            pivot_number = ((int)strtoul(optarg, NULL, 0) - 1) | 1; // decrease to odd number
            break;
        case 'Y':
            buffer_length = (int)strtoul(optarg, NULL, 0);
            break;
        case 'y':   // Algorithm when N is small
            c = *optarg;
            for (info = test_indirect, idx = 0; idx < sizeof(test_indirect) / sizeof(INFO); idx++, info++) {
                if (c == info->option && ! info->type) {
                    small_func = info->sort_function;
                    break;
                }
            }
            break;
        case 'Z':
            size = (size_t)strtoul(optarg, NULL, 0);
            break;
        default:    // select sorting algorithm
            for (info = test, idx = 0; idx < sizeof(test) / sizeof(INFO); idx++, info++) {
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
        if (fin) fprintf(stderr, "Filename = %s\n", fin);
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

    // medium boundary
    int skip = repeat_count > 1 ? 1: 0;
    if (Loption < 0) {  // depth for hybrid sorting boundary
        threshold = nmemb / pow(2.0, -Loption / (IsPercentB ? 100.0 / log2(nmemb): 1.0));
    }
    else if (Loption > 0) { // size
        threshold = IsPercentB ? (nmemb * Loption) / 100: Loption;
    }
    if (threshold == 0) threshold = 8192;   // default
//    if (medium_boundary > nmemb) medium_boundary = nmemb;
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP && Loption != 0) fprintf(OUT, "medium_boundary = %ld\n", threshold);
#endif

    // gap list
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "small_boundary = %ld\n", small_boundary);
#endif
    size_t  f1 = 1, f2 = 1, fib = 1;
    gap_count = 1;
    if (small_boundary > nmemb) small_boundary = nmemb;
    while ((fib = f1 + f2) < small_boundary) {
        if (trace_level >= TRACE_DEBUG) fprintf(OUT, "f1 = %ld  f2 = %ld  fib = %ld\n", f1, f2, fib);
        f1 = f2; f2 = fib;
        gap_count++;
    }
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "gap_count = %d\tfibonacci = %ld\tf2 = %ld\n", gap_count, fib, f2);
    size_t G[gap_count]; gaplist = G;   // gap_count is not huge.
    for (i = 0; i < gap_count; i++) {
        f1 = fib - f2;
        G[i] = fib = f2;
        f2 = f1;
        if (trace_level >= TRACE_DEBUG) fprintf(OUT, "f1 = %ld  f2 = %ld  fib = %ld\n", f1, f2, G[i]);
    }

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

    long *usec = NULL;
#ifdef DEBUG
    usec = malloc(sizeof(long));
#else
    usec = calloc(sizeof(long),repeat_count);
#endif

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
        for (int i = 0; i < 1; i++) {   // once
#else
        for (int i = 0; i < repeat_count; i++) {
#endif
            workbuff = NextBuffer;
            memcpy(workbuff, srcbuf, memsize);      // memory copy : sorted_array <-- srcbuf
            qsort_comp_str = qsort_called = qsort_moved = 0;    // reset all of counters
            start_timer(&start_time);
            qsort(workbuff, nmemb, size, cmpstring);
            usec[i] = stop_timer(&start_time);
        }
        if (show_result(description, usec, repeat_count, skip, 0) > limit) goto QSORT;
    }
    memcpy(sorted_array, workbuff, memsize);

    /* test */

#define ENOUGH  4000000L
    long *cache, *clear;
    unsigned seed = (unsigned)time(NULL) + (unsigned)clock();
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP) fprintf(OUT, "random seed = %d\n", seed);
#endif
    srand(seed);
    random_number = set_random();
    // test array sort or indeirect sort
    long sorting_time;
    for (info = test,idx = 1; index != 0; idx <<= 1, info++) {
        if (index & idx) {
            index &= ~idx;  // clear bit
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "Start %s : %s\n", info->name, info->description);
#else
REDO:
#endif
#ifdef DEBUG
            if (trace_level == TRACE_NONE)  // don't add a semicolon ";"
#endif
                fprintf(OUT, "%s", info->name);
            cache = calloc(sizeof(long), ENOUGH);
            clear = cache;  // really enough?
            for (long l = 0; l < ENOUGH; l++) *clear++ = -1L;
            free(cache);
            sorting_time = 0;
            for (int i = 0; i < repeat_count; i++) {
                qsort_comp_str = qsort_called = qsort_moved = search_pivot = 0;    // reset all of counters
                workbuff = NextBuffer;
                memcpy(workbuff, srcbuf, memsize);  // memory copy : workbuff <-- srcbuf
                start_timer(&start_time);
                (*info->sort_function)(workbuff, nmemb, size, cmpstring);
                sorting_time = stop_timer(&start_time);
#ifdef  DEBUG
                usec[0] = sorting_time;
                if (trace_level != TRACE_NONE) fprintf(OUT, "%s", info->name);
                show_result(info->description, usec, 1, 0, 0);
#else
                usec[i] = sorting_time;
#endif
            }
#ifndef DEBUG
            if (show_result(info->description, usec, repeat_count, skip, 0) > limit) goto REDO;
#endif
            if (check_result(info->name, workbuff)) {   // error
                print_out = TRUE;
                break;
            }
        }
    }
    // test pointer sort
    if (indirect_option) {  // != NULL
#ifndef DEBUG
        long *shell_time = calloc(sizeof(long), repeat_count);
#endif
        long index_sum, index_time;
        for (p = indirect_option; *p; p++) {
            for (info = test_indirect, idx = 0; idx < sizeof(test_indirect) / sizeof(INFO); idx++, info++) {
                if (info->option == *p) {   // jound
#ifdef DEBUG
                    if (trace_level >= TRACE_DUMP) fprintf(OUT, "Start %s : %s\n", info->name, info->description);
#else
REDO_P:
#endif
#ifdef DEBUG
                    if (trace_level == TRACE_NONE)  // don't add a semicolon ";"
#endif
                        fprintf(OUT, "%s", info->name);
                    sorting_time = index_sum = index_time = 0;
                    for (int i = 0; i < repeat_count; i++) {
                        qsort_comp_str = qsort_called = qsort_moved = search_pivot = 0;    // reset all of counters
                        workbuff = NextBuffer;
                        memcpy(workbuff, srcbuf, memsize);  // memory copy : workbuff <-- srcbuf

                        start_timer(&start_time);
                        void **idxtbl = make_index(workbuff, nmemb, size); // make an index table from srcbuf
                        if (idxtbl == NULL) return EXIT_FAILURE;
                        start_timer(&core_time);
                        (*info->sort_function)(idxtbl, nmemb, cmpstring);
                        sorting_time = stop_timer(&core_time);
                        unindex(workbuff, idxtbl, nmemb, size);     // restore index table to workbuff
                        index_time = stop_timer(&start_time);
#ifdef  DEBUG
                        usec[0] = index_time;
                        if (trace_level != TRACE_NONE) fprintf(OUT, "%s", info->name);
                        show_result(info->description, usec, 1, 0, 0);
#else
                        index_sum += (shell_time[i] = index_time) - (usec[i] = sorting_time);
                        if (trace_level >= TRACE_DUMP) {
                            if (i == 0) fprintf(OUT, "\n");
                            fprintf(OUT, "indexing time = %ld\n", shell_time[i]);
                        }
#endif
                        free(idxtbl);
                    }
#ifndef DEBUG
                    long *dump_timer = trace_level ? usec : shell_time;
                    if (trace_level >= TRACE_DUMP) fprintf(OUT, "%s\t", info->name);
                    if (show_result(info->description, dump_timer, repeat_count, skip, trace_level ? index_sum : 0) > limit) goto REDO_P;
#endif
                    if (check_result(info->name, workbuff)) {   // error
                        print_out = TRUE;
                        break;
                    }
                    break;
                }
            }
        }
    }
    // output result
    if (print_out) for (p = workbuff, i = 0; i++ < nmemb; p += size) fprintf(OUT, "%s\n", p);
    return EXIT_SUCCESS;
}
