#!/usr/bin/awk -f
#
#	n, n, ..., n, 1, 1, ..., 1
#
BEGIN {
	num = ARGC > 1 ? (ARGV[1]) : 8;
	fmt = sprintf("%d", num);
	fmt = sprintf("%%%dd\n", length(fmt));
	half = num / 2;
	for (i = 0; i++ < half;) printf(fmt, num);
	for (++num; i++ < num;) printf(fmt, 1);
	exit;
}
