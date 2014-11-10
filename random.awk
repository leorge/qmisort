#!/usr/bin/awk -f
#
#	usage : $0 [count]
#
#	This script print a sequence of numbers like a following one-liner generates.
#
#		seq 0 `echo count-1|bc` | shuf | awk '{printf "%10d/n", $0}' 
#
#	But the one-liner generate no duplicated number.
#
BEGIN {
	srand()
	num = ARGC > 1 ? (ARGV[1] + 0) : 7
	fmt = sprintf("%d", num - 1)
	fmt = sprintf("%%%dd\n", length(fmt))
	for (i = 0; i < num ; i++)
		printf(fmt, num * rand())
}
