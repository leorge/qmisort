#!/usr/bin/awk -f
#
#	tim sort killer if N=64*x ?
#
#	n-2, n-1, n-4, n-3, ..... , 2, 3, 0, 1
#
BEGIN {
	num = ARGC > 1 ? (ARGV[1]) : 8;
	fmt = sprintf("%d", num -1);
	fmt = sprintf("%%%dd\n", length(fmt));
	amp = 1;
	while (num > 0) {
		x = --num + (amp = -amp);
		printf(fmt, x < 0? 0: x);
	}
	exit;
}
