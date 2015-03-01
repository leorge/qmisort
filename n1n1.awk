#!/usr/bin/awk -f
BEGIN {
    if (ARGC <= 1) {
        print "zigzag : n, 1, n, 1, ..., n, 1"
        exit;
    }
    else num = ARGV[1];
    fmt = sprintf("%d", num);
    fmt = sprintf("%%0%dd\n", length(fmt));
    for (i = 0; i++ < num - 1; i++) {
        printf(fmt, num);
        printf(fmt, 1);
    }
    exit;
}
