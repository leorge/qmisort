#!/usr/bin/perl
#
#   Create best data for quick_hole().
#
use strict;
use warnings;
$#ARGV < 0 && die "usage : $0 power\n";
my $power = shift(@ARGV);
my $fmt = sprintf("%%0%dd\n", length(2**($power + 1) - 1));
#die "\$fmt = $fmt\n";
my @left = (0);
while ($power--) {
    my $center = $#left + 1;
    my @right = @left;
    foreach (@right) {$_ += $center + 1;}
    my $tail = pop(@right);
    @left = (@left, $tail, @right, $center);
}
#die "@left\n";
while (@left) {
    my $head = shift(@left);
    printf($fmt, $head);
}
