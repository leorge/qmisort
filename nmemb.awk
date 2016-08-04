#!/usr/bin/awk -f
#
# Get distribution of logN.
#
# usage : Debug/Sort -N 999 -V 2 -options datafile | $0
#
function show() {
    buffer = title; sum = 0;
    for (i = 0; i++ < logN; ) {
        p = count[i];
        buffer = buffer OFS p;
        sum += p;
    }
    print buffer, sum
    delete count;
}
BEGIN {log2 = log(2); OFS="\t"}
/^qsort\(3\) nmemb = / {  # make a line header
    logN = int(log($NF) / log2);
    line_header = "log2(" $NF ")=" logN;
    for (i = 0; i++ < logN;) line_header = line_header OFS i;
    line_header = line_header OFS "sum"
}
/^Start / { # begining of a new algorithm
    if (title) show(); else print line_header;
    title = $2;     # subroutine name
}
/^sort\(\) partition start/ {count[int(log($6) / log2)]++}   # count up
END {title && show()}   # Flush last data
