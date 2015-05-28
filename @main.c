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
QsortAlogrithm	QA = RANDOM3;
Trace   trace_level = TRACE_NONE;   // to debug
size_t  medium_boundary = 1000;     //  nmemb to alternate to merge sort.
size_t  small_boundary = 8;        	//  nmemb to alternate from merge sort.
long    qsort_called, qsort_comp_str, qsort_moved, search_pivot;  // counters
int     pivot_number = 3;
size_t  random_depth = 5;
double  random_number;

void    (*small_pointer)() = imerge_sort;
void    (*medium_index)() = merge_pointer;
void    (*pivot_sort)() = mi_psort;

void set_random(void) {
    if (random_number == 0.0) {
        random_number = rand()/(double)(RAND_MAX - 1);  // 0 <= random_number < 1
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "random = %f\n", random_number);
#endif
    }
}

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
    HOLE_RANDOM,
    HOLE_RANDOM3,
    HOLE_VARIOUS,
    HOLE_LOG2,
    HEAP_SORT,
    INSERT_LINEAR,
    INSERT_BINARY,
    TREE_SORT,
    MERGE_INSERT_INDEX,
    MERGE_INSERT_POINTER,
    QUICK_SORT,
    ARRAY_SORT,
    INDEX_SORT,
    POINTER_SORT,
    BUBBLE_ARRAY,
    BUBBLE_POINTER,
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
#ifdef  DEBUG
            {'1', SWAP_KR, "qsort_kr()", qsort_kr, FALSE,
                "Quick sort : pivot is a middle element with swaps in K&R style. Single loop."},
#endif
            {'2', HOLE_LOG2, "qsort_log2()", qsort_log2, FALSE,
                "Quick sort : pivot is median of log2(N) with a hole."},
            {'3', SWAP_MED3, "qsort_med3()", qsort_med3, FALSE,
                "Quick sort : pivot is median of 3 elements with swaps."},
            {'a', ARRAY_SORT, "array_sort()", array_sort, FALSE,
                "hybrid sorting of quicksort : array sorting."},
//            {'b', MERGE_INSERT_BINARY, "mi_pbin(*)", mi_pbin, TRUE,
//                "MI sort : pointer sorting with binary search."},
            {'c', INDEX_QSORT3, "iqsort()", iqsort, FALSE,
                "index sorting of qsort(3)."},
            {'C', POINTER_QSORT3, "pqsort()", pqsort, TRUE,
                "Pointer sorting of qsort(3)."},
            {'d', SWAP_MIDDLE, "qsort_middle()", qsort_middle, FALSE,
                "Quick sort : pivot is a miDDle element with swaps."},
			{'E', BUBBLE_ARRAY, "bubble_array()", bubble_sort, FALSE,
				"Bubble sort : array sorting."},
			{'e', BUBBLE_POINTER, "pbubble_pointer()", bubble_pointer, TRUE,
				"Bubble sort : pointer sorting."},
            {'f', SWAP_FIRST, "qsort_first()", qsort_first, FALSE,
                "Quick sort : pivot is a First element with swaps."},
            {'G', MERGE_POINTER, "merge_pointer(*)", merge_pointer, TRUE,
                "merGe sort : pointer sorting (index sorting except indexing time)."},
			{'h', HOLE_LAST, "qsort_hole()", qsort_hole, FALSE,
				"Quick sort : pivot is a Last element with a hole."},
#ifdef  DEBUG
            {'H', HEAP_SORT, "heap_sort()", heap_sort, FALSE,
                "Heap sort."},
            {'i', INSERT_LINEAR, "insert_sort()", insert_sort, FALSE,
            "Insertion sort : linear search."},
            {'t', TREE_SORT, "tree_sort()", tree_sort, FALSE,
            "Insertion tree sort : median node tree."},
#endif
            {'j', MERGE_INSERT_INDEX, "mi_isort()", mi_isort, FALSE,
                "MI sort : index sorting of Merge sort and conventional insertion sort."},
            {'k', MERGE_INSERT_POINTER, "mi_psort(*)", mi_psort, TRUE,
                "MI sort : pointer sorting."},
            {'m', MERGE_ARRAY, "merge_sort()", merge_sort, FALSE,
                "Merge sort : array sorting."},
            {'M', MERGE_INDEX, "imerge_sort()", imerge_sort, FALSE,
                "Merge sort : index sorting."},
            {'q', QUICK_SORT, "quick_sort()", quick_sort, FALSE,
                "Quick sort : entire array sorting."},
            {'Q', POINTER_SORT, "pointer_sort(*)", pointer_sort, TRUE,
                "hybrid sorting of quicksort : pointer sorting."},
            {'r', HOLE_RANDOM, "qsort_random()", qsort_random, FALSE,
                "Quick sort : pivot is a Random element with a hole."},
            {'s', STABLE_ARRAY, "stable_array()", stable_array, FALSE,
                "Stable QM or QMI sort : array sorting."},
            {'S', STABLE_POINTER, "stable_pointer(*)", stable_pointer, TRUE,
                "Stable QM or QMI sort : pointer sorting."},
            {'U', DUMMY, "dummy_sort()", dummy_sort, FALSE,
                "dUmmy sort : do nothing."},
            {'v', HOLE_VARIOUS, "qsort_various()", qsort_various, FALSE,
                "Quick sort : Pivot is median of Various elements with a hole."},
			{'w', HOLE_RANDOM3, "qsort_random3()", qsort_random3, FALSE,
				"Quick sort : pivot is a median of 3 random element with a hole."},
			{'x', INDEX_SORT, "index_sort()", index_sort, FALSE,
				"hybrid sorting of quicksort : index sorting."},
    };

    // prepare to analyze command arguments
    qsort(test, sizeof(test) / sizeof(INFO), sizeof(INFO), cmp_info);   // sort a table according to enumeration
    char    *p, optstring[sizeof(test) / sizeof(INFO) + 100];   // enough long
    size_t  i;
    memset(optstring, 0, sizeof(optstring));
    for (info = test, p = optstring, i = 0; i++ < sizeof(test) / sizeof(INFO); info++) *p++ = (char)info->option;
    strcat(optstring, "?A:B:D:F:L:N:pP:R:ST:V:W:X:Y:Z:");
    /**** Analyze command arguments ****/
    char    *prg = strrchr(argv[0], '/') + 1;   // Program name without path
    if (prg == NULL) prg = argv[0];
    char    *fin = NULL;            // file name to input
    typedef long INDEX;
    INDEX   index = 0, idx;
    bool    print_out = FALSE;
    bool	IsPercentB = FALSE;
    int     opt, repeat_count, buffer_length = 1;
    double	Boption = 0.0;
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
                " [-Z element_size] [-R repeat_count] [-NB nmemb] [-D depth] [-AXP func] [-L log] [-W width]"
#ifdef  DEBUG
                    " [-V trace_level]"
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
                "\t-D : Limit of Depth for randomize.\n"
                "\t-N : Number of members (default is 31).\n"
                "\t-p : Print out the last result.\n"
                "\t-R : Repeat count.\n"
#ifndef DEBUG
                "\t-T : UncerTainTy percenT to pass (default is 2 [%]).\n"
#endif
                "\t-W : Number of elements to select a pivot for -v option (default is 3).\n"
                "\t-Y : CYclic work buffer length.\n"
                "\t-Z : siZe of an array element.\n\n"
				"\t-B : Boundary to switch algorithm or choice of pivot in hole scheme. (default is 8)\n"
				"\t       If the value is less than 0 then value means depth.\n"
				"\t       Else if the value is followed by % 0 then value means depth in percent.\n"
                "\t-P : Algorithm to Find a pivot while N is large.\n"
                "\t       r - Random element. (default)\n"
                "\t       3 - median of random 3 elements.\n"
                "\t       2 - median of random log2(n) elements.\n"
            "\nAlgorithm option :\n"
                "\t-A : Algorithm when nmemb is medium for array sort.\n"
                "\t-X : Algorithm when nmemb is medium for indeX sort.\n"
                "\t-F : Algorithm of pointer sort to Find a pivot.\n"
                "\n\tfunc : function for algorithm option\n"
                "\t       G - GNU library qsort(3).\n"
                "\t       b - Merge and bubble sort.\n"
                "\t       l - Merge and insertion sort with linear search.\n"
                "\t       m - Index sorting of merge sort. (default)\n"
				"\t       a - Array sorting of merge sort for -A option.\n"
#ifdef DEBUG
            "\nTrace option :\n"
                "\t-V 1 : Show Count.\n"
                "\t-V 2 : Trace action.\n"
                "\t-V 3 : Trace data copy.\n"
                "\t-V 4 : Trace comparison.\n"
                "\t-V 5 : Others to debug."
#endif
            );
            return EXIT_SUCCESS;
            break;
        case 'B':
			p = &optarg[strlen(optarg) - 1];
			if (*p == '%') IsPercentB = TRUE;
            Boption= atof(optarg);
            break;
        case 'D':
            random_depth = (int)strtoul(optarg, NULL, 0);
            break;
        case 'N':
            nmemb = strtoul(optarg, NULL, 0);           // Check a definition of size_t
            break;
        case 'p':
            print_out = TRUE;
            break;
        case 'R':
            repeat_count = strtoul(optarg, NULL, 0);    // Check a definition of size_t
            break;
        case 'T':
            limit = atoi(optarg);
            break;
        case 'V':   // ignored in Release
            trace_level = strtoul(optarg, NULL, 0);
            break;
        case 'W':
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
            case 'G':
                small_pointer = qsort;
                break;
            case 'a':
                small_pointer = merge_sort;
                break;
            case 'l':
                small_pointer = mi_isort;
                break;
            case 'm':
                small_pointer = imerge_sort;
                break;
            default:
                fprintf(stderr, "Illegal value \"%s\" for -A option.\n", optarg);
                return EXIT_FAILURE;
                break;
            }
            break;
        case 'F':   // Algorithm when nmemb is medium for indeX sort.
            switch(*optarg) {
            case 'G':
                pivot_sort = pqsort;
                break;
            case 'l':
                pivot_sort = mi_psort;
                break;
            case 'm':
                pivot_sort = merge_pointer;
                break;
            default:
                fprintf(stderr, "Illegal value \"%s\" for -F option.\n", optarg);
                return EXIT_FAILURE;
                break;
            }
            break;
        case 'X':   // Algorithm of Pointer sort to find a Pivot.
            switch(*optarg) {
            case 'G':
                medium_index = pqsort;
                break;
            case 'l':
                medium_index = mi_psort;
                break;
            case 'm':
                medium_index = merge_pointer;
                break;
            default:
                fprintf(stderr, "Illegal value \"%s\" for -X option.\n", optarg);
                return EXIT_FAILURE;
                break;
            }
            break;
		case 'P':   // Algorithm to Find a pivot while N is large in hybrid sorting
			switch(*optarg) {
			case '2':
				QA = LOG2;
				break;
			case '3':
				QA = RANDOM3;
				break;
			case 'r':
				QA = RANDOM;
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
    if (Boption < 0) {	// depth for hybrid sorting boundary
    	medium_boundary = nmemb / pow(2.0, -Boption / (IsPercentB ? 100.0 / log2(nmemb): 1.0));
    }
    else if (Boption > 0) {	// size
    	medium_boundary = IsPercentB ? (nmemb * Boption) / 100: Boption;
    }
#ifdef DEBUG
    if (trace_level >= TRACE_DUMP && Boption != 0) fprintf(OUT, "medium_boundary = %ld\n", medium_boundary);
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
    srand((unsigned)time(NULL));
    set_random();
    for (info = test,idx = 1; index != 0; idx <<= 1, info++) {
        if (index & idx) {
            index &= ~idx;  // clear bit
#ifdef DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "Start %s : %s\n", info->name, info->description);
#endif
REDO:       fprintf(OUT, "%s", info->name);
#define ENOUGH  4000000L
            cache = calloc(sizeof(long), ENOUGH);
            clear = cache;  // really enough?
            for (long l = 0; l < ENOUGH; l++) *clear++ = -1L;
            free(cache);
#ifdef  DEBUG
            if (trace_level >= TRACE_DUMP) fprintf(OUT, "\n");
#endif
            begin_timer(repeat_count);
            for (int i = 0; i < repeat_count; i++) {
                qsort_comp_str = qsort_called = qsort_moved = 0;    // reset all of counters
                if (repeat_count > 1) set_random();
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
            }
            if (elapsed_time(info->description, skip) > limit) goto REDO;
            if (check_result(info->name, workbuff)) {   // error
                print_out = TRUE;
                break;
            }
        }
    }

    if (print_out) for (p = workbuff, i = 0; i++ < nmemb; p += size) fprintf(OUT, "%s\n", p);

    return EXIT_SUCCESS;
}
