#!/usr/bin/awk -f
BEGIN {
    if (ARGC <= 1) {
        print "few data : n, n, ..., n, 1, 1, ..., 1"
        exit;
    }
    else num = ARGV[1];
    fmt = sprintf("%d", num);
    fmt = sprintf("%%0%dd\n", length(fmt));
    half = num / 2;
    for (i = 0; i++ < half;) printf(fmt, num);
    for (++num; i++ < num;) printf(fmt, 1);
    exit;
}
