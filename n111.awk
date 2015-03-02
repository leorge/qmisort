#!/usr/bin/awk -f
#
#   n, 1, 1, 1, ...
#
BEGIN {
    if (ARGC <= 1) {
        print "first is big : n, 1, 1, 1, ..., 1"
        exit;
    }
    else num = ARGV[1];
    fmt = sprintf("%d", num);
    fmt = sprintf("%%0%dd\n", length(fmt));
    printf(fmt, num);
    for (i = 0; i++ < num - 1; ) printf(fmt, 1);
    exit;
}
