#!/bin/bash

# execution

if [ $# -eq 0 ] ; then
    echo "$0  max_size  max_nmemb  [options]"
    echo "    max_size  : size of element in byte begin with 2^4 up to 2^max_size."
    echo "    max_nmemb : number of element begin with 2^12 up to 2^max_nmemb."
    exit 1
else
    Result=evaluate.tmp
    rm -f $Result
    min_size=4; max_size=$1; shift      # 2^4(=16) --> 2^max_size
    if [ $max_size -lt $min_size ]; then max_size=$min_size; fi
    min_nmemb=12; max_nmemb=$1; shift   # 2^12(=1024) --> 2^max_nmemb
    if [ $max_nmemb -lt $min_nmemb ]; then max_nmemb=$min_nmemb; fi
    options=$*
    src/random.awk `echo 2^$max_nmemb | bc` > random.$max_nmemb  # create a data file
    for i in `seq $min_size $max_size`  # size of an element
    do
        if [ $i -gt $min_size ]; then echo ; fi # make a gap
        size=`echo 2^$i | bc`       # exp2(N)   16, 32, 64, 128, ...
#       size=`echo 2^$i-1 | bc`     # exp2(N)-1 15, 31, 63, 127, ...
        for j in `seq $min_nmemb $max_nmemb`
        do
            if [ $j -gt $min_nmemb ]; then echo ; fi    # make a line gap
            nmemb=`echo 2^$j| bc`   # number of elements
            cmd="Release/Sort -Z $size -N $nmemb $options"
            if [ $# -eq 0 ]; then   # no options
                echo "$cmd options"
            else
                echo $cmd | tee /dev/stderr >> $Result
                $cmd random.$max_nmemb | tee -a $Result | \
                    awk -v sz=$size -v nmemb=$nmemb -v OFS="\t" '{print $1, nmemb, sz, $4}'
            fi
        done
    done > tmp.evaluate
fi

# formatting

awk -f - tmp.evaluate << 'EOF'
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
EOF
