#!/usr/bin/awk -f
BEGIN {
    if (ARGC <= 1) {
        print "median-3-killer : n-1, 0, 1, 2, ..., n-2"
        exit;
    }
    else num = ARGV[1];
    fmt = sprintf("%d", num -1);
    fmt = sprintf("%%%dd\n", length(fmt));
    printf(fmt, num - 1);
    for (i = 0; i < num - 1; ) printf(fmt, i++);
    exit;
}
