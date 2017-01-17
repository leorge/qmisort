#!/usr/bin/awk -f
#
#   Create a table of elapsed time.
#
#   usage : ./$0  -v  'x=name'  file
#
#   variable x - item name, for example "N" or "log(N)"
#   file - output of Release/Sort program.
#
BEGIN {OFS="\t"; headline = x}
NF == 1 {
    split($1, a, ":"); seq = a[1]; column = a[2];
    if (seq == 1) headline = headline OFS column;
    next;
}
$2 == "usec" {
    time[name = $1, seq, column] = $4
    if (! index(names OFS, OFS name OFS)) names = names OFS name;
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

