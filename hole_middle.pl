#!/usr/bin/perl
#
# generate a bad data for quicksort with hole pivot is the middle element.
#
use strict;
use warnings;
$#ARGV < 0 && die "Usage : $0 count\n";
my $n = shift(@ARGV);   # count
$n < 1 && die "count > 0\n";
my $fmt = sprintf("%%0%dd\n", length($n - 1));
#warn "\$n = $n\t\$fmt = $fmt\n";
my @list = (0, 1);
foreach my $i (2..$n-1) {
    my @middle = splice(@list, $#list / 2 + 1, 1, $i);
    push @list, @middle;
}
foreach my $i (@list) {
    printf $fmt, $i;
}
