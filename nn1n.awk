#!/usr/bin/awk -f
BEGIN {
    if (ARGC <= 1) {
        print "2nd last is small : n, n, n, ..., n, 1, n"
        exit;
    }
    else num = ARGV[1];
    fmt = sprintf("%d", num);
    fmt = sprintf("%%0%dd\n", length(fmt));
    for (i = 0; i++ < num - 2; ) printf(fmt, num);
    printf(fmt, 1);
    printf(fmt, num);
    exit;
}
