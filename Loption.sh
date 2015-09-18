#!/bin/sh

# Help message

if [ $# -eq 0 ] ; then	# with no parameter
    echo "$0 - Get elapsed time with various -L option"
    echo "   usage : $0  max_power_of_N log2(N)"
    exit 255	# aborted
fi

# execution

maxP=$1; shift
Z=$1; shift
if [ $maxP -le 3 ]; then maxP=4; fi
N=`echo 2^$maxP | bc`
src/random.awk $N > random.$maxP    # generate a data file
for power in `seq 3 $maxP`; do
    B=`echo 2^$power | bc`          # boundary
    # median of random 3 elements
    cmd="Release/Sort -N $N -h3qC 3 -D 3 -A M -Z $Z -L $B"
    echo $cmd | tee /dev/stderr
    $cmd random.$maxP | sed s/quick_hybrid/hybrid_random3/
    # median of variout elements
    cmd="Release/Sort -N $N -qC v -v 5 -D 3 -A M -Z $Z -L $B"
    echo $cmd | tee /dev/stderr
    $cmd random.$maxP | sed s/quick_hybrid/hybrid_various/
    # median of log2(N) elements
    cmd="Release/Sort -N $N -I m -qC l -D 3 -A M -Z $Z -L $B"
    echo $cmd | tee /dev/stderr
    $cmd random.$maxP | sed s/quick_hybrid/hybrid_log2N/
done > tmp.Loption

# edit output

awk -f - tmp.Loption << 'EOF'
BEGIN {name = delm = ":"; OFS="\t"; log2 = log(2)}
/-L/ {
    B = log($NF) / log2
    if (B < minB || minB == 0) minB = B;
    next
}    # -L option
{sub(/[(].*[)]/, "", $1);}          # delete (3) or () in a function name
name !~ delm $1 delm {name = name $1 delm}  # append new function name
{a[$1, B] = $4}                     # record elapsed time
END {
    ids = split(substr(name, 2, length(name) - 2), names, delm);
    lineheader = "log2(B)"
    for (x = minB; x <= B;) lineheader = lineheader OFS x++;
    print lineheader;
    for (y = 1; y <= ids; y++) {    # for each function
        buffer = name = names[y];
        for (x = minB; x <= B; x++) buffer = buffer OFS a[name, x];
        print buffer;
    }
}
EOF
