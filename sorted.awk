#!/usr/bin/awk -f
BEGIN {
    if (ARGC <= 1) {
        print "sorted : 0, 1, 2, ..., n-2, n-1"
        exit;
    }
    else num = ARGV[1];
    fmt = sprintf("%d", num -1);
    fmt = sprintf("%%0%dd\n", length(fmt));
    for (i = 0; i < num; ) printf(fmt, i++);
    exit;
}
