#!/bin/sh

# Help message

if [ $# -eq 0 ] ; then	# with no parameter
    echo "$0 - Get elapsed time with various -C option"
    echo "   usage : $0  max_power_of_N element_size"
    exit 255	# aborted
fi

# execution

OUT=tmp.`basename $0`

maxP=$1; shift
Z=$1; shift
if [ $maxP -le 3 ]; then maxP=4; fi
N=`echo 2^$maxP | bc`
src/random.awk $N > random.$maxP    # generate a data file
for power in `seq 10 $maxP`; do
    B=`echo 2^$power | bc`          # boundary
    # median of random 3 elements
    cmd="Release/Sort -Z $Z -rC 3 -D 3 -N $B"
    echo $cmd | tee /dev/stderr
    $cmd random.$maxP | sed s/quick_random/quick_random3/
    # median of variout elements
    cmd="Release/Sort -Z $Z -rC v -D 3 -N $B"
    echo $cmd | tee /dev/stderr
    $cmd random.$maxP | sed s/quick_random/quick_various/
    # median of log2(N) elements
    cmd="Release/Sort -Z $Z -rC l -D 3 -N $B"
    echo $cmd | tee /dev/stderr
    $cmd random.$maxP | sed s/quick_random/quick_log2N/
    # hybrid of random3 and log2(N)
    cmd="Release/Sort -Z $Z -rC h -D 3 -N $B"
    echo $cmd | tee /dev/stderr
    $cmd random.$maxP | sed s/quick_random/quick_hybrid/
done > $OUT

# edit output

awk -f - $OUT << 'EOF'
BEGIN {name = delm = ":"; OFS="\t"; log2 = log(2)}
/-N/ {
    B = log($NF) / log2
    if (B < minB || minB == 0) minB = B;
    next
}    # -N option
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
