#!/usr/bin/awk -f
#
# Get distribution of nmemb.
#
# usage : Debug/Sort -N 999 -V 2 -options datafile | $0
#
function show() {
    buffer = title; sum = 0;
    for (i = 0; i++ < nmemb; ) {
        p = count[i];
        buffer = buffer OFS p;
        sum += p;
    }
    print buffer, sum
    delete count;
}
BEGIN {log2 = log(2); OFS="\t"}
/^arguments/ {  # make a line header
    for (i = 0; ++i <= NF && ! nmemb; ) {
        if (match($i, /-.*N/)) nmemb = int(log($++i) / log2);
    }
    if (! nmemb) nmemb = 31;    # default
    line_header = "log2(" $4 ")=" nmemb;
    for (i = 0; i++ < nmemb;) line_header = line_header OFS i;
    line_header = line_header OFS "sum"
}
/^Start / { # begining of a new algorithm
    if (title) show(); else print line_header;
    title = $2;     # subroutine name
}
/^sort\(\) start / {count[int(log($7) / log2)]++}   # count up
END {title && show()}   # Flush last data
