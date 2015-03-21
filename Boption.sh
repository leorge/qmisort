#!/bin/sh
if [ $# -eq 0 ] ; then
	echo "$0 - Get elapsed time with various -B option"
    echo "   usage : $0  max_power_of_2  options"
    exit;
fi

# execution

P=$1; shift;	# get parameter
if [ $P -le 3 ]; then P=4; fi
N=`echo 2^$P | bc`
src/random.awk $N > random.$P
for power in `seq 3 $P`; do
    B=`echo 2^$power | bc`
    cmd="Release/Sort -N $N $* -B $B"
    echo $cmd | tee /dev/stderr
    $cmd random.$P
done | src/uniq1.awk > Boptions.tmp

# edit output

awk -f - Boptions.tmp << 'EOF'
BEGIN {name = delm = ":"; OFS="\t"; log2 = log(2)}
/-B/ {B = log($NF) / log2; next}	# -B option
{sub(/[(].*[)]/, "", $1);}			# delete (3) or () in a function name
name !~ delm $1 delm {name = name $1 delm}  # append new function name
{a[$1, B] = $4}         			# record elapsed time
END {
    ids = split(substr(name, 2, length(name) - 2), names, delm);
	lineheader = "log2(B)"
    for (x = 3; x <= B;) lineheader = lineheader OFS x++;
	print lineheader;
    for (y = 1; y <= ids; y++) {    # for each function
        buffer = name = names[y];
        for (x = 3; x <= B; x++) buffer = buffer OFS a[name, x];
        print buffer;
    }
}
EOF
