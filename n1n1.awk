#!/usr/bin/awk -f
#
#	n, 1, n, 1, ..., n, 1
#
BEGIN {
	num = ARGC > 1 ? (ARGV[1]) : 8;
	fmt = sprintf("%d", num);
	fmt = sprintf("%%%dd\n", length(fmt));
	for (i = 0; i++ < num - 1; i++) {
		printf(fmt, num);
		printf(fmt, 1);
	}
	exit;
}
