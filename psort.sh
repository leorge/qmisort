#!/bin/sh
#
# Get comparisons of pointer sorting in various N.
#
#   -P : Pointer sorting. options are below.
#       m - Merge sort.
#       q - Quicksort. pivot is a middle element.
#       i - Linear insertion sort.
#       b - Binary insertion sort.
#       L - sheLL sort
#       h - Heap sort. heapfy by sift up.
#       H - Heap sort. heapfy by sift down.
#       B - Bubble sort.
#       C - Comb sort.
#       u - step Up sort.
#       p - steP sort. step up and down sort.
#
if [ $# -eq 0 ]; then
    echo $0 options numbers
    exit;
fi

options=$1; shift
for N in $*; do
    src/random.awk $N | Debug/Sort -N $N -P $options -V 1
done > tmp$$

awk -f - tmp$$ <<'EOF'
BEGIN {N = "N"}
/^arguments/ {N = N "\t" $4; next}
{
    name = $1; sub(/\(\*\)/, "", name);
    if (! comp[name]) {
        names = names FS name;
        comp[name] = name;
    }
    comp[name] = comp[name] "\t" $10;
}
END {
    print N;
    N = split(names, y);
    for (i = 1; i <= N; i++) print comp[y[i]];
}
EOF
rm tmp$$
