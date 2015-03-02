#!/usr/bin/awk -f
BEGIN {
    if (ARGC <= 1) {
        print "last is small : n, n, ..., n, 1"
        exit;
    }
    else num = ARGV[1];
    fmt = sprintf("%d", num);
    fmt = sprintf("%%0%dd\n", length(fmt));
    for (i = 0; i++ < num - 1; ) printf(fmt, num);
    printf(fmt, 1);
    exit;
}
