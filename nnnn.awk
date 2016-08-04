#!/usr/bin/awk -f
BEGIN {
    if (ARGC <= 1) {
        print "equal data : n, n, ..., n"
        exit;
    }
    else num = ARGV[1];
    fmt = sprintf("%d", num);
    fmt = sprintf("%%0%dd\n", length(fmt));
    for (i = 0; i++ < num;) printf(fmt, num);
    exit;
}
