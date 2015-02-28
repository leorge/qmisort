#!/usr/bin/perl
#
# generate a bad data list for qsort_swap3().
#
use strict;
use warnings;
$#ARGV < 0 && die "Usage : $0 count\n";
my $n = shift(@ARGV);   # count
my $fmt = sprintf "%%%dd\n", log($n) / log(10) + 1;
my @list = ($n);
unshift(@list, $n - 2); # (N-2, N)
unshift(@list, --$n);   # (N-1, N-2, N)
while (--$n > 1) {
    my @mid = splice(@list, $#list / 2 - 1, 1, $n - 2);
    unshift(@list, (--$n, @mid));
}
foreach my $i (@list) {
    printf $fmt, $i;
}
