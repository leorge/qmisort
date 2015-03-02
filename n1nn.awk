#!/usr/bin/awk -f
BEGIN {
    if (ARGC <= 1) {
        print "2nd is small : n, 1, n, n, ..., n"
        exit;
    }
    else num = ARGV[1];
    fmt = sprintf("%d", num);
    fmt = sprintf("%%0%dd\n", length(fmt));
    printf(fmt, num);
    printf(fmt, 1);
    for (i = 0; i++ < num - 2; ) printf(fmt, num);
    exit;
}
