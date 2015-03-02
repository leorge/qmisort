#!/usr/bin/awk -f
BEGIN {
    if (ARGC <= 1) {
        print "first and last is big : n, 1, ..., 1, n"
        exit;
    }
    else num = ARGV[1];
    fmt = sprintf("%d", num);
    fmt = sprintf("%%0%dd\n", length(fmt));
    printf(fmt, num);
    for (i = 0; i++ < num - 2; ) printf(fmt, 1);
    printf(fmt, num);
    exit;
}
