#!/usr/bin/awk -f
#
# Generate data for the worst case of median-of-3.
#
BEGIN {
    if (ARGC <= 1) {
        print "median-3-killer : 1, 2, ..., n-1, 0"
        exit;
    }
    else num = ARGV[1];
    fmt = sprintf("%d", num -1);
    fmt = sprintf("%%0%dd\n", length(fmt));
    for (i = 0; i < num - 1; ) printf(fmt, ++i);
    printf(fmt, 0);
    exit;
}
