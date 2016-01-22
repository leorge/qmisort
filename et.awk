#!/usr/bin/awk -f
#
#   Create a table of elapsed time.
#
#	usage : ./$0  -v  'x=name'  file
#
#	variable x - item name, for example "N" or "log(N)"
#	file - output of Release/Sort program.
#
BEGIN {OFS="\t"}
NF == 1 {
    split($1, a, ":");
    if (a[1] > seq) {
        seq = a[1];       # update
        headline = x;     # clear
    } 
    k = a[2];
    headline = headline OFS k;
    next;
}
! /^arguments/ {
    name = $1;
    time[name, seq, k] = $4  # overwritten if stddev is large.
    if (! index(names OFS, OFS name OFS)) {  # new name
        names = names OFS name;
    } 
}
END {
    X = split(headline, threshold, OFS);
    Y = split(substr(names, 2), funcs, OFS);  # function names
    for (i = 0; i++ < seq; ) {
        print i;    # seq. #
        print headline;
        for (y = 0; y++ < Y; ) {
            buffer = name = funcs[y];
            for (x = 1; x++ < X; ) {
                buffer = buffer OFS time[name, i, threshold[x]];
            }
            print buffer;
        }
        print "";   # new line
    }
}

