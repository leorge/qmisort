#!/usr/bin/awk -f
#
#	Make a matrix from output of shell scripts "spread".
#
BEGIN {
    OFS="\t";
    order = 12; nmemb = 2 ** order;
    header = "2**n";
    new_order = 1;
}
{
    if (NF) {
        fnc = $1; sub(/\(.*\)/, "", fnc);   # delet () or (*)
        if (new_order) {
            header = header OFS order++;
            new_order = 0;
        }
        if (match(names OFS, OFS fnc OFS) == 0) {
            names = names OFS fnc;
        }
        if (percent[fnc] == "") percent[fnc] = fnc;
        percent[fnc] = percent[fnc] OFS $(NF-1);
    }
    else new_order = 1;
}
END {
    print header;
    n = split(substr(names, 2), name, OFS);
    for (i = 0; i++ < n; ) print percent[name[i]];
}
