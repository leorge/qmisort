#!/usr/bin/awk -f
#
#  Print a expression "=express(cell1, cell2, .....)" for Calc or Excel.
#  Default of express is "Average".
#
#  Usage   : $0  head_cell second_row count [express]
#  example : $0  B3  7  3  --> =Average(B3,B10,B17)
#
BEGIN {
    if (ARGC < 4) {
        print "Usage : " ARGV[0] " first  2nd_row  count  [express]";
        exit;
    }
    first = ARGV[1]; gap = ARGV[1]; count = ARGV[3];
    sub(/^[^0-9]*/, "", gap);
    gap = ARGV[2] - gap;
    row = match(first, /[0-9]/);
    column = substr(first, 1, row - 1);
    row = substr(first, row);
    express = (ARGC > 4) ? ARGV[4] : "Average";
    for (i = 0; i++ < count; row += gap) cells = cells "," column row;
    printf "=%s(%s)\n", express, substr(cells, 2);
    exit;
}
