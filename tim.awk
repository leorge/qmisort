#!/usr/bin/awk -f
#
#	tim sort run killer if N=64*x ?
#
BEGIN {
	if (ARGC <= 1) {
		print "nearly reversed : n-2, n-1, n-4, n-3, ..... , 2, 3, 0, 1"
		exit;
	}
	else num = ARGV[1];
	fmt = sprintf("%d", num -1);
	fmt = sprintf("%%%dd\n", length(fmt));
	amp = 1;
	while (num > 0) {
		x = --num + (amp = -amp);
		printf(fmt, x < 0? 0: x);
	}
	exit;
}
