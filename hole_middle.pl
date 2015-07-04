#!/usr/bin/perl
#
# generate a bad data for hybrid_array() random is 0.5.
#
use strict;
use warnings;
$#ARGV < 0 && die "Usage : $0 count\n";
my $n = shift(@ARGV);   # count
$n < 1 && die "count > 0\n";
my $fmt = sprintf("%%0%dd\n", length($n - 1));
#warn "\$n = $n\t\$fmt = $fmt\n";
my @list = (--$n);
while ($n--) {
    my @middle = splice(@list, ($#list - 1) / 2, 1, $n);
    my @last = splice(@list, $#list, 1, @middle);
    unshift @list, @last;
}
foreach my $i (@list) {
    printf $fmt, $i;
}
