#!/usr/bin/awk -f
#
#   Make matrixes from output of shell scripts "evaluate" or "nmemb".
#
BEGIN {name = num = size = delm = ":"; OFS="\t"}
NF != 4 {next}  # skip an echo of command line or vacant line
{sub(/[(].*[)]/, "", $1);}  # delete (3) or () in a function name
name !~ delm $1 delm {name = name $1 delm}  # append new function name
num !~ delm $2 delm {num = num $2 delm}     # append new nmemb(Number of member)
size !~ delm $3 delm {size = size $3 delm}  # append new array size
{a[$1, $2, $3] = $4}                        # record elapsed time
END {
    if (name == delm) exit; # invalid data
    # list --> array
    sz = split(substr(size, 2, length(size) - 2), sizes, delm);
    columns = split(substr(num, 2, length(num) - 2), nmemb, delm);
    ids = split(substr(name, 2, length(name) - 2), names, delm);
    # make a line header
    log2 = log(2);
    for (x = 1; x <= columns;) lineheader = lineheader OFS log(nmemb[x++]) / log2;
    # output for each size
    for (z = 1; z <= sz; z++) {
        if (z > 1) print "";
        print "size=" (size = sizes[z]) lineheader;
        for (y = 1; y <= ids; y++) {    # for each function
            buffer = name = names[y];
            for (x = 1; x <= columns; x++) buffer = buffer OFS a[name, nmemb[x], size];
            print buffer;
        }
    }
}
