#!/usr/bin/perl
#
#   Create worst data for merge sort.
#
use strict;
use warnings;
$#ARGV < 0 && die "usage : $0 power\n";
my $power = shift(@ARGV);
my $width = 2**$power;
my $fmt = sprintf("%%0%dd\n", length($width - 1));
#die "\$width = $width\t\$fmt = $fmt\n";
my @src = (0..$width-1);
for (; $power-- > 0; $width >>= 1) {
    my @dst = ();
    while (@src) {
        my @part = splice(@src, 0, $width);
        my @left = (); my @right = ();
        while (@part) {
            push(@right, shift(@part));
            push(@left, shift(@part));
        }
        @dst = (@dst, @left, @right);   # push, push
    }
    @src = @dst;
}
#die "@src\n";
while (@src) {
    my $head = shift(@src);
    printf $fmt, $head;
}
