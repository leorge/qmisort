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

#include	<ctype.h>
#include	<math.h>
#include	<sys/time.h>
#include	<time.h>
#include	<unistd.h>
#include	"sort.h"
#include	"log2s.h"

/*
 * Uncomment or define symbols to examine algorithm.
 */
//#define	HEAP_SORT 1
//#define	INSERTION_SORT 1
//#define	LOG2_ALGORITHM 1

/****	Public	****/
Trace	trace_level = TRACE_NONE;	// to debug
int		log2_boundary;				//	nmemb to alternate to merge sort.
long	qsort_called, qsort_comp_str, qsort_moved;	// counters
int		pivot_number;
int		random_depth;
double	random_number;

void	(*small_array)();
void	(*small_index)();
void	(*pivot_sort)();
int		(*func_log2)();

// Dump an element.
const char *dump_data(const void *data) {
	return ispointer? *(const char **)data: data ;
}

void set_random(void) {
    if (random_number == 0.0) {
    	random_number = rand()/(double)(RAND_MAX - 1);	// 0 <= random_number < 1
#ifdef	DEBUG
    	if (trace_level >= TRACE_DUMP) fprintf(OUT, "random = %f\n", random_number);
#endif
    }
}

/****	Private	****/

// You can edit this function for other data structure.
int		length_compare;
static int cmpstring(const void *p1, const void *p2)	// Function to compare
{
    int rtn = strncmp(p1, p2, length_compare);
#ifdef	DEBUG
    qsort_comp_str++;
    if (trace_level >= TRACE_COMPARE) fprintf(OUT, "cmpstring(%s, %s) --> %d\n", (char *)p1, (char *)p2, rtn);
#endif
    return  rtn;
}

typedef enum {
	DUMMY,
	SWAP_HEAD,
	SWAP_HEAD1,
	SWAP_MIDDLE,
	TRADITIONAL,
	SWAP_3,
	HOLE_TAIL,
	HOLE_RANDOM,
	HOLE_VARIOUS,
	HOLE_LOG2,
	HEAP_SORT,
	MERGE_ARRAY,
	MERGE_INDEX,
	MERGE_POINTER,
	INSERT_SORT,
	INSERT_PSORT,
	TREE_SORT,
	MERGE_INSERT_INDEX,
	MERGE_INSERT_POINTER,
 	ARRAY_SORT,
	INDEX_SORT,
	POINTER_SORT,
} TEST_TYPE;

typedef struct {
	int			option;				// command option
	TEST_TYPE	type;				// see above.
	const char *name;				// function name to display
	void 		(*sort_function)();
	bool		pointer_sort;		// True : pointer sort		False : array sort or index sort
	const char	*description;
} INFO;

// Estimate time
static int elapsed_time(const char *comment, int skip)
{
	int	percent = 0;	// return value
	int datacount = end_timer(skip);
	fprintf(OUT, "\tusec = %.0f", usec);
#ifdef	DEBUG
	if (trace_level >= TRACE_COUNT) fprintf(OUT, "\tcall = %ld\tcompare = %ld\tcopy = %ld"
		, qsort_called, qsort_comp_str, qsort_moved);
	if (datacount) {}	// dummy statement to avoid warning
#else
	if (datacount > 1) {
		percent = 100. * esd / usec + .5;
		fprintf(OUT, "\tspread = %.0f\t%3d %%", esd, percent);
		int		i;
		long	*tlen = microsec;
		fprintf(OUT, " [%ld]", *tlen++);
		for (i = 0; i < skip - 1; i++) fprintf(OUT, " [%ld]", *tlen++);
		for (; i <= datacount; i++, tlen++) {
			if (i == peak - 1) fprintf(OUT, " (%ld)", *tlen);
			else fprintf(OUT, " %ld", *tlen);
		}
	}
#endif
	fprintf(OUT, "\n");
	return	percent;
}

// examine
char *sorted_array;
size_t memsize;
static int check_result(const char *name, const void *workbuffer)
{
    int rtn = memcmp(workbuffer, sorted_array, memsize);
    if (rtn) fprintf(stderr, "Not sorted in %s\n", name);
    return	rtn;
}

static int	cmp_info(const void *t1, const void *t2) {	// to sort test[] in main().
	return ((INFO*)t1)->type - ((INFO*)t2)->type;
}

void dummy_sort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
}

/*****************************************************************************/
int main(int argc, char *argv[])
{
	extern int getopt(int argc, char * const argv[], const char *optstring);
	extern	int	optind;
	extern	char *optarg;
    INFO *info, test[] = {	// alphabetic order in symbol names of enum for each block.
    		// simple in-place sort.
			{'1', SWAP_HEAD1, "qsort_tail1()", qsort_tail1, FALSE,
				"Traditional quick sort with swapping. Pivot is a last element. Single loop."},
			{'2', HOLE_LOG2, "qsort_log2()", qsort_log2, FALSE,
				"Quick sort with a hole. Pivot is median of log2(N) elements."},
			{'3', SWAP_3, "qsort_swap3()", qsort_swap3, FALSE,
				"Traditional quick sort with swapping. Pivot is median of sorted 3 elements."},
			{'a', ARRAY_SORT, "array_sort()", array_sort, FALSE,
				"Quick and merge sort.(array sort)"},
			{'d', SWAP_MIDDLE, "qsort_middle()", qsort_middle, FALSE,
				"Traditional quick sort with swapping. Pivot is a miDDle element."},
			{'G', MERGE_POINTER, "merge_pointer(*)", merge_pointer, TRUE,
				"MerGe sort.(except indexing time)"},
			{'h', SWAP_HEAD, "qsort_head()", qsort_head, FALSE,
				"Traditional quick sort with swapping. Pivot is a Head element. Nested loop."},
#ifdef	HEAP
			{'H', HEAP_SORT, "heap_sort()", heap_sort, FALSE,
				"Heap sort."},
#endif
			{'i', INSERT_SORT, "insert_sort()", insert_sort, FALSE,
			"Conventional Insertion sort."},
			{'I', INSERT_PSORT, "insert_psort(*)", insert_psort, TRUE,
			"Conventional Insertion pointer sorting."},
			{'j', MERGE_INSERT_INDEX, "mi_isort()", mi_isort, FALSE,
				"Merge and insertion sort.(index sort)"},
#ifdef	TREE
			{'J', TREE_SORT, "tree_sort()", tree_sort, FALSE,
			"Insertion sort with median node tree."},
#endif
			{'k', MERGE_INSERT_POINTER, "mi_psort(*)", mi_psort, TRUE,
				"Merge and insertion sort.(except indexing time)"},
			{'m', MERGE_ARRAY, "merge_sort()", merge_sort, FALSE,
			"Merge sort.(double buffer)"},
			{'M', MERGE_INDEX, "imerge_sort()", imerge_sort, FALSE,
				"Merge sort.(double index)"},
			{'q', INDEX_SORT, "index_sort()", index_sort, FALSE,
				"Quick and merge sort.(index sort)"},
			{'Q', POINTER_SORT, "pointer_sort(*)", pointer_sort, TRUE,
				"Quick and merge sort.(except indexing time)"},
			{'r', HOLE_RANDOM, "qsort_random()", qsort_random, FALSE,
				"Quick sort with a hole. Pivot is a Random element."},
			{'s', TRADITIONAL, "qsort_trad()", qsort_trad, FALSE,
				"Traditional quick sort with swapping. Pivot is median of 3 elements."},
			{'t', HOLE_TAIL, "qsort_tail()", qsort_tail, FALSE,
				"Quick sort with a hole. Pivot is a Tail element."},
			{'U', DUMMY, "dummy_sort()", dummy_sort, FALSE,
				"dUmmy sort do nothing."},
			{'v', HOLE_VARIOUS, "qsort_various()", qsort_various, FALSE,
				"Quick sort with a hole. Pivot is median of Various elements."},
    };
    // set default value to global variables.
    log2_boundary = 8;
    pivot_number = 3;
    random_depth = 5;
    small_array = imerge_sort;
    small_index = merge_pointer;
    pivot_sort = small_index;
    func_log2 = log2G;		// default is built-in log2()
    ispointer = FALSE;		// index_sort.c

    // prepare to analyze command arguments
    qsort(test, sizeof(test) / sizeof(INFO), sizeof(INFO), cmp_info);	// sort a table according to enumeration
    char	*p, optstring[sizeof(test) / sizeof(INFO) + 100];	// enough long
    size_t	i;
    memset(optstring, 0, sizeof(optstring));
    for (info = test, p = optstring, i = 0; i++ < sizeof(test) / sizeof(INFO); info++) *p++ = (char)info->option;
    strcat(optstring, "?A:B:D:pP:L:N:R:ST:V:W:X:Y:Z:");
	/**** Analyze command arguments	****/
    char	*prg = strrchr(argv[0], '/') + 1;	// Program name without path
    if (prg == NULL) prg = argv[0];
    char	*fin = NULL;			// file name to input
    typedef	long INDEX;
    INDEX	index = 0, idx;
    bool	print_out = FALSE, zzz = FALSE;	// don't sleep
	int		opt, repeat_count, buffer_length = 1;
    size_t	nmemb = 31, size = 0;
    int		limit = 2;		// boundary precent to pass

#ifdef	DEBUG	// Default
	repeat_count = 1;
#else
	repeat_count = 12;
#endif
	while ((opt = getopt(argc, argv, optstring)) != -1) {
		switch (opt) {
		case '?':
			printf("Usage : %s [options]"
				" [-Z element_size] [-R repeat_count] [-NB nmemb] [-D depth] [-AXP func] [-L log] [-W width]"
#ifdef	DEBUG
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
				"\t-B : Boundary number to switch algorithm for -2 option.\n"
				"\t-D : Limit of Depth for randomize.\n"
				"\t-N : Number of members (default is 31).\n"
				"\t-p : Print out the last result.\n"
				"\t-R : Repeat count.\n"
#ifndef	DEBUG
				"\t-T : UncerTainTy percenT to pass.\n"
#endif
				"\t-W : Number of elements to select a pivot for -v option.\n"
				"\t-Y : CYclic work buffer length.\n"
				"\t-Z : siZe of an array element.\n"
			"\nAlgorithm option :\n"
				"\t-A : Algorithm when nmemb is small for Array sort.\n"
				"\t-X : Algorithm when nmemb is small for indeX sort.\n"
				"\t-P : Algorithm of Pointer sort to find a Pivot.\n"
				"\n\tfunc : function for algorithm option\n"
				"\t    3 : Built-in function qsort(3).\n"
				"\t    j : Merge and insertion sort.\n"
				"\t    M : Merge sort.(default)\n"
				"\t    a : Merge sort for -A option.(not index sort)\n"
#ifdef	LOG2_ALGORITHM
			"\nLog2 option : to search a highest \"1\" bit in nmemb.\n"
				"\t-L a : Assembly.\n"
				"\t-L b : Binary search.\n"
				"\t-L r : Right shift count.\n"
				"\t(default) : Built-in function log2(3).\n"
#endif
#ifdef	DEBUG
			"\nTrace option :\n"
				"\t-V 1 : Show Count.\n"
				"\t-V 2 : Trace action.\n"
				"\t-V 3 : Trace data copy.\n"
				"\t-V 4 : Trace comparison.\n"
				"\t-V 5 : Trace to debug.\n"
#endif
	        );
			return EXIT_SUCCESS;
			break;
		case 'B':
			log2_boundary = (int)strtoul(optarg, NULL, 0);
			break;
		case 'D':
			random_depth = (int)strtoul(optarg, NULL, 0);
			break;
		case 'N':
			nmemb = strtoul(optarg, NULL, 0);			// Check a definition of size_t
			break;
		case 'p':
			print_out = TRUE;
			break;
		case 'R':
			repeat_count = strtoul(optarg, NULL, 0);	// Check a definition of size_t
			break;
		case 'S':
			zzz = TRUE;
			break;
		case 'T':
			limit = atoi(optarg);
			break;
		case 'V':	// ignored in Release
			trace_level = strtoul(optarg, NULL, 0);
			break;
		case 'W':
			pivot_number = (int)strtoul(optarg, NULL, 0) | 1;	// make an odd number
			break;
		case 'Y':
			buffer_length = (int)strtoul(optarg, NULL, 0);
			break;
		case 'Z':
			size = (size_t)strtoul(optarg, NULL, 0);
			break;
		case 'A':	// select a sorting function when nmemb <= small_boundary.
			switch(*optarg) {
			case '3':
				small_array = qsort;
				break;
			case 'a':
				small_array = merge_sort;
				break;
			case 'j':
				small_array = mi_isort;
				break;
			case 'M':
				small_array = imerge_sort;
				break;
			default:
				fprintf(stderr, "Illegal value for -A option \"%s\"\n", optarg);
				return EXIT_FAILURE;
				break;
			}
			break;
		case 'P':	// select an index sort function when nmemb > small_boundary.
			switch(*optarg) {
			case '3':
				pivot_sort = iqsort;
				break;
			case 'j':
				pivot_sort = mi_psort;
				break;
			case 'M':
				pivot_sort = merge_pointer;
				break;
			default:
				fprintf(stderr, "Illegal value for -P option \"%s\"\n", optarg);
				return EXIT_FAILURE;
				break;
			}
			break;
		case 'X':	// select an index sort function when nmemb > small_boundary.
			switch(*optarg) {
			case '3':
				small_index = iqsort;
				break;
			case 'j':
				small_index = mi_psort;
				break;
			case 'M':
				small_index = merge_pointer;
				break;
			default:
				fprintf(stderr, "Illegal value for -I option \"%s\"\n", optarg);
				return EXIT_FAILURE;
				break;
			}
			break;
#ifdef	LOG2_ALGORITHM
		case 'L':
			switch(toupper(*optarg)) {
			case 'A':
				func_log2 = log2A;
				break;
			case 'B':
				func_log2 = log2B;
				break;
			case 'R':
				func_log2 = log2R;
				break;
			}
			break;
#endif
	    default:	// select sorting algorithm
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
#ifdef	DEBUG
	if (trace_level > TRACE_NONE) {
		fprintf(OUT, "arguments :");
		char **p = argv;
		for (int i = 1; i++ < argc;) fprintf(OUT, " %s", *++p);
		fprintf(OUT, "\n");
	}
#endif

    /**** Read data	****/

	// Open file
    FILE *fp = fin ? fopen(fin, "r") : stdin;	// open file or use stdin
    if (fp == NULL) {
    	perror(NULL);		// Output system error message to OUT.
		return EXIT_FAILURE;
    }

    // Read first line to get a record size
    char read_buff[1024];
    if (! fgets(read_buff, sizeof(read_buff) - 1, fp)) return EXIT_SUCCESS;	// EOF
    int buflen = strlen(read_buff);	// with EOL(End Of Line : LF or CR+LF)
    if (! isprint((int)read_buff[0])) {			// control character CR, LF, TAB etc.
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
    memsize = nmemb * size;

    // read data
    i = 0;
    p = srcbuf;
    buflen--;
    do {
    	char *q;
    	if ((q = strrchr(read_buff, '\n')) != NULL) *q = '\0';	// chop LF
    	if ((q = strrchr(read_buff, '\r')) != NULL) *q = '\0';	//      CR
    	strncpy(p, read_buff, buflen);
    	i++;
    	if (! fgets(read_buff, sizeof(read_buff), fp)) break;		// EOF
    	p += size;
    } while (i < nmemb);
    if (i <= 1) return EXIT_SUCCESS;
    memsize = (nmemb = i) * size;
    srcbuf = realloc(srcbuf, memsize);

    /***** Prepare *****/

	int	skip = repeat_count > 1 ? 1: 0;

//#define	BUFCYCLE	2
	char	**workarea = (char **)malloc(sizeof(char *) * buffer_length);
	char	*workbuff = *workarea;
	int		buffid;
	for (buffid = 0; buffid < buffer_length;) {
		if ((workbuff = calloc(nmemb, size)) == NULL)
		{   /* Can't get work area  */
			perror(NULL);
			return EXIT_FAILURE;
		}
		workarea[buffid++] = workbuff;
	}
#define	NextBuffer	workarea[buffid = (buffid + 1) % buffer_length]

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
			if (trace_level >= TRACE_DUMP) fprintf(OUT, " nmemb = %ld\n", nmemb);	// for nmemb.awk
#endif
        begin_timer(repeat_count);
        for (int i = 0; i < repeat_count; i++) {
        	workbuff = NextBuffer;
        	memcpy(workbuff, srcbuf, memsize);		// memory copy : sorted_array <-- srcbuf
        	qsort_comp_str = qsort_called = qsort_moved = 0;	// reset all of counters
    		start_timer();
        	qsort(workbuff, nmemb, size, cmpstring);
        	stop_timer();
        	if (zzz) sleep(1);
        }
		if (elapsed_time(description, skip) > limit) goto QSORT;
    }
	memcpy(sorted_array, workbuff, memsize);

    /* test */

    void **idxtbl;
    srand((unsigned)time(NULL));
    set_random();
    for (info = test,idx = 1; index != 0; idx <<= 1, info++) {
    	if (index & idx) {
			index &= ~idx;	// clear bit
#ifdef DEBUG
			if (trace_level >= TRACE_DUMP) fprintf(OUT, "Start %s : %s\n", info->name, info->description);
#endif
REDO:
			fprintf(OUT, "%s", info->name);
#ifdef	DEBUG
			if (trace_level >= TRACE_DUMP) fprintf(OUT, "\n");
#endif
			begin_timer(repeat_count);
			for (int i = 0; i < repeat_count; i++) {
				qsort_comp_str = qsort_called = qsort_moved = 0;	// reset all of counters
				if (repeat_count > 1) set_random();
				workbuff = NextBuffer;
				memcpy(workbuff, srcbuf, memsize);	// memory copy : workbuff <-- srcbuf
				if (info->pointer_sort) {
					idxtbl = make_index(workbuff, nmemb, size);	// make an index table from srcbuf
					if (idxtbl == NULL) return EXIT_FAILURE;
					start_timer();
					(*info->sort_function)(idxtbl, nmemb, cmpstring);
					stop_timer();
					unindex(workbuff, idxtbl, nmemb, size);		// restore index table to workbuff
					free(idxtbl);
				}
				else {	// array sort or index sort.
					start_timer();
					(*info->sort_function)(workbuff, nmemb, size, cmpstring);
					stop_timer();
				}
	        	if (zzz) sleep(1);
			}
			if (elapsed_time(info->description, skip) > limit) goto REDO;
			if (check_result(info->name, workbuff)) {	// error
				print_out = TRUE;
				break;
			}
    	}
    }

    if (print_out) for (p = workbuff, i = 0; i++ < nmemb; p += size) fprintf(OUT, "%s\n", p);

    return EXIT_SUCCESS;
}
