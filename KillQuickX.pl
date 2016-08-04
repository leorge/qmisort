#!/usr/bin/perl
#
# Generate a worst data for pseudomedian of nine.
#
#    cf. https://en.wikipedia.org/wiki/Quicksort#History
#
#    example - http://algs4.cs.princeton.edu/23quicksort/QuickX.java.html
#
use strict;
use warnings;
$#ARGV < 0 && die "Usage : $0 count\n";
my $n = shift(@ARGV);	# count
my $fmt = sprintf "%%0%dd\n", log($n) / log(10) + 1;
my @insert = (6, 5, 4, 3, 2, 1, 0);	# wrong data for insertion sort
my @list;
if ($n <= $#insert + 1) {@list = splice(@insert, 0, $n)}
elsif ($n <= 40) {@list = ($n-1, @insert, 7..$n-2)}	# median-of-3 killer
else {
	my $N = 40;
	@list = ($N-1, @insert,7..$N-2);
	do {
		$N += 4;	# add 4 elements
		my $eps = int($N / 8);
		my @x = splice(@list, 0, 1, $N-3);
		my @z = splice(@list, $eps, 1, $N-2);
		my @y = splice(@list, $N - $eps - 1, 1, $N-1);
		push(@list, (@x, @y, @z, $N-4));
	} while ($#list < $n - 1)
}
foreach my $i (@list) {printf $fmt, $i;}
