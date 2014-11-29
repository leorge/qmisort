#!/usr/bin/awk -f
#
#	n, 1, n, n, ..., n
#
BEGIN {
	num = ARGC > 1 ? (ARGV[1]) : 8;
	fmt = sprintf("%d", num);
	fmt = sprintf("%%%dd\n", length(fmt));
	printf(fmt, num);
	for (i = 0; i++ < num - 2; ) printf(fmt, 1);
	printf(fmt, num);
	exit;
}
