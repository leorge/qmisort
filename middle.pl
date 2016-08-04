#!/usr/bin/perl
#
# Generate a worst data for a conventional quicksort with swaps that chooses the middle element as a pivot
#
use strict;
use warnings;
$#ARGV < 0 && die "Usage : $0 count\n";
my $n = shift(@ARGV);   # count
my $fmt = sprintf("%%0%dd\n", length($n - 1));
#warn "\$N = $N\t\$fmt = $fmt\n";
my @list = (--$n);
while ($n--) {
    my @mid = splice(@list, $#list / 2, 1, $n);
    unshift(@list, @mid);
}
foreach my $i (@list) {
    printf $fmt, $i;
}
