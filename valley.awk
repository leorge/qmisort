#!/usr/bin/awk -f
#
#   n-1, n-3, ..., 3, 1, 0, 2, 4, ..., n-4, n-2 or
#   n-1, n-3, ..., 4, 2, 0, 1, 3, ..., n-4, n-2
#
BEGIN {
    if (ARGC <= 1) {
        print "valley data : n-1, n-3, ..., 3, 1, 0, 2, 4, ..., n-4, n-2 or";
        print "n-1, n-3, ..., 4, 2, 0, 1, 3, ..., n-4, n-2";
        exit;
    }
    else num = ARGV[1];
    fmt = sprintf("%d", num - 1);
    fmt = sprintf("%%0%dd\n", length(fmt));
    for (i = num; --i >= 0;) printf(fmt, i--);
#print "turn i =", i
    if (i < -1) i = 0;
    for (; ++i < num;) printf(fmt, i++);
    exit;
}
