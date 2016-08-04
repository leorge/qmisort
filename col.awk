#!/usr/bin/awk -f
#
#   Output a tablist for expand(1).
#
#   usage :
#       $ s=filename; expand -t `path/col.awk $s` $s
#
func max(a, b) {return a > b ? a : b}
{
    maxNF = max(maxNF, NF);
    for (i = 1; i <= NF; i++) {
        a = length($i);
        len[i] = max(a, len[i]);
    }
}
END {
    for (i =1; i <= maxNF; i++) 
        s = sprintf("%s,%d", s, l += len[i] + 1);
    print substr(s, 2);
}
