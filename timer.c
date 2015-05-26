/*
 * timer.c
 *
 *  measure elapsed time.
 *
 *  Created on: 2013/02/19
 *      Author: leo
 */
#include    <sys/time.h>
#include    <math.h>
#include    <time.h>
#include    "sort.h"

double      usec, esd;  // Estimated Standard Deviation = Sqrt(Sum(Square(Xi-m))/(n-1))
long        *microsec;
int         peak;

static int  max_count, index;
static struct timeval from;

bool    begin_timer(int count) {
    bool    rtn = FALSE;
    index = 0;
    if (count > 0) {
        microsec = calloc(max_count = count, sizeof(long));
        rtn = (microsec == NULL) ? FALSE : TRUE;
    }
    else perror("begin_timer() in __FILE__");
    return  rtn;
}

void    start_timer() {
    if (index < max_count) gettimeofday(&from, NULL);
}

long stop_timer() {
    long    rtn = 0;
    if (index < max_count) {
        struct timeval to;
        gettimeofday(&to, NULL);
        rtn = (to.tv_sec - from.tv_sec) * 1000000. + to.tv_usec - from.tv_usec;
        microsec[index++] = rtn;
    }
    return  rtn;
}

int end_timer(int skip) {
    int     n = 0;  // data count to return
    esd = 0.;
    n = index - skip;
    if (n > 0) {
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) {
            fprintf(OUT, "microsec =");
            for (int i = 0; i < index; i++) fprintf(OUT, " %ld", microsec[i]);
            fprintf(OUT, "\n");
        }
#endif
        long    y, max_y = 0;
        double sum = 0., delta;
        for (int i = skip; i < index; i++) {
            sum += (y = microsec[i]);
            if (y > max_y) {
                max_y = y;
                peak = i;
            }
        }
        if (n > 1) {
            usec = (sum -= max_y) / --n;    // mean
            for (int i = skip; i < index; i++) {
                if (i != peak) {
                    delta = microsec[i] - usec;
                    esd += (double)delta * delta;
                }
            }
            if (n > 1) esd = sqrt(esd / (n - 1));
        }
        else usec = sum;
#ifdef  DEBUG
        if (trace_level >= TRACE_DUMP) fprintf(OUT, "sum = %f\tindex = %d\tave = %.0f\tesd = %.0f\n", sum, index, usec, esd);
#endif
    }
    return  n;  // data count
}
