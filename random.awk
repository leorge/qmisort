#!/usr/bin/awk -f
#
#   usage : $0 [count]
#
#   This script print a sequence of numbers like a following one-liner generates.
#
#       seq 0 10 | shuf | awk '{printf "%02d\n", $0}' 
#
#   But the one-liner generate no duplicated number.
#
BEGIN {
    "date +%N" | getline seed
    srand(seed)
    num = ARGC > 1 ? (ARGV[1] + 0) : 7
    fmt = sprintf("%d", num - 1)
    fmt = sprintf("%%0%dd\n", length(fmt))
    for (i = 0; i < num ; i++)
        printf(fmt, num * rand())
}
