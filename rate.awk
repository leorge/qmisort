#!/usr/bin/gawk -f
#
# Get distribution of split ratio.
#
# usage : Debug/leosort -V 2 -N 999 -options datafile | $0
#
BEGIN {log2 = log(2); fmt = "%02d"; OFS="\t"}
function show(i) {  # i is a local variable. Others are global variables.
    buffer = nmemb;
    for (i = 0; i <= 10;) buffer = buffer OFS cnt[sprintf(fmt,i++)];
    print buffer;
    delete cnt;
}
function dump() {
    print title line_header;    # concatenate and print
    m = asorti(count, idx);     # idx is 1-d array
    for (i = 0; i++ < m; ) {    # m is the number of elements in array idx
        p = idx[i];             # idx[] is sorted indices
        split(p, pos, SUBSEP);
        x = pos[1];             # log2(nmemb)
        y = pos[2];             # rate : 0, 0.1, 0.2, ..., 1.0
#       print "count[" x "," y "]=" count[p];
        if (i == 1) nmemb = x;  # first line
        else if (x != nmemb) {  # new line
            show();             # output one line
            nmemb = x;          # update
        }
        cnt[y] = count[p];
    }
    show(); # Flush
    delete count;
}

/^qsort\(3\) nmemb = / {
    line_header = "\n";
    for (i = "0.0"; i <= 1; i += 0.1) line_header = line_header OFS i;
}
/^Start / {
    if (title) {dump(); print ""}
    title = $2;     # subroutine name
}
/sub partition/ && ($4 || $6) { # True if anterior or posterior is not 0.
    count[sprintf(fmt, int(log($4 + $6) / log2)) , sprintf(fmt, int($10 * 10 + 0.5))]++;
}
END {title && dump()}   # Flush last data
