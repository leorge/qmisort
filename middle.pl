#!/usr/bin/perl
#
# generate a bad data for qsort_middle().
#
use strict;
use warnings;
$#ARGV < 0 && die "Usage : $0 count\n";
my $n = shift(@ARGV);   # count
my $fmt = sprintf "%%0%dd\n", log($n) / log(10) + 1;
my @list = ($n);
while ($n > 1) {
    my @mid = splice(@list, $#list / 2, 1, --$n);
    unshift(@list, @mid);
}
foreach my $i (@list) {
    printf $fmt, $i;
}
