#!/bin/sh
#
# Search the best small_boundary.
#
N=100000
for Z in 16 64 256; do          # size of an element
    for l in `seq 2 32`; do     # small_boundary
        echo $Z $l
        for i in `seq 1 10`; do # Repeat 10 times
            src/random.awk $N | Release/Sort -N $N -Z $Z -l $l -E
        done
        echo ""
    done
done | grep -v qsort | tee tmp$$

awk -f - tmp$$ << 'EOF'
NF==2 {z=$1; l=$2} # element size, small boundary
NF>3 && $8<3 {     # Stdev is less than 3%
    a[z,l] += $4;  # elapsed time in micro seconds
}
END {
    for (l=2;l<=32;l++) printf "\t%d", l;
    print "";
    for (z=16; z<=256; z*=4) {  # 16, 64, 256
        printf "%d", z;
        for (l=2;l<=32;l++) printf "\t%d", a[z,l];
        print "";
    }
}
EOF

