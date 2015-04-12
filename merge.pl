#!/usr/bin/perl
#
#   Create worst data for merge sort.
#
use strict;
use warnings;
$#ARGV < 0 && die "usage : $0 N\n";
my $N = shift(@ARGV);
our $fmt = sprintf("%%0%dd\n", length($N - 1));
#warn "\$N = $N\t\$fmt = $fmt\n";
&merge(0..$N-1);
sub merge {
    my @src = @_;
#warn "(@src)\n";
    if (! $#src) {printf $fmt, $src[0];}
    else {
        my @right = ();
        my @left = ();
        while(@src) {
            unshift(@left, pop(@src));
            if (@src) {unshift(@right, pop(@src));}
        }
        &merge(@left);
        if (@right) {&merge(@right);}
    }
}
