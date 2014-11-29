#!/usr/bin/perl
#
# generate a bad data list for qsort_swap3().
#
use strict;
use warnings;
$#ARGV < 0 && die "Usage : $0 count\n";
my $n = shift(@ARGV);	# count
my $fmt = sprintf "%%%dd\n", log($n) / log(10) + 1;
my @list = ($n, 1 .. $n-1);
foreach my $i (@list) {
	printf $fmt, $i;
}
