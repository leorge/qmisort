#!/usr/bin/awk -f
BEGIN {
    if (ARGC <= 1) {
        print "sorted : n-1, n-2,..., 2, 1, 0"
        exit;
    }
    else num = ARGV[1];
    fmt = sprintf("%d", num -1);
    fmt = sprintf("%%0%dd\n", length(fmt));
    for (i = num; i--; ) printf(fmt, i);
    exit;
}
