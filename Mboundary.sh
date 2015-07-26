#!/bin/sh
if [ $# -eq 0 ] ; then
    echo "$0 - Get elapsed time with various -L option"
    echo "   usage : $0  max_power_of_N  options"
    exit 1;
fi

# execution

maxP=$1; shift # get parameter
if [ $maxP -le 3 ]; then maxP=4; fi
N=`echo 2^$maxP | bc`
src/random.awk $N > random.$maxP   # generate a data file
for power in `seq 3 $maxP`; do
    L=`echo 2^$power | bc`
    cmd="Release/Sort -N $N $* -L $L"
    echo $cmd | tee /dev/stderr
    $cmd random.$maxP
done > Lopt.tmp

# edit output

awk -f - Lopt.tmp << 'EOF'
BEGIN {name = delm = ":"; OFS="\t"; log2 = log(2)}
/-L/ {L = log($NF) / log2; next}    # -L option
{sub(/[(].*[)]/, "", $1);}          # delete (3) or () in a function name
name !~ delm $1 delm {name = name $1 delm}  # append new function name
{a[$1, L] = $4}                     # record elapsed time
END {
    ids = split(substr(name, 2, length(name) - 2), names, delm);
    lineheader = "log2(L)"
    for (x = 3; x <= L;) lineheader = lineheader OFS x++;
    print lineheader;
    for (y = 1; y <= ids; y++) {    # for each function
        buffer = name = names[y];
        for (x = 3; x <= L; x++) buffer = buffer OFS a[name, x];
        print buffer;
    }
}
EOF
