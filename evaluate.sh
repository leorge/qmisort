#!/bin/bash
if [ $# -eq 0 ] ; then
    echo "$0  max_size  max_nmemb  [options]"
    echo "    max_size  : size of element in byte begin with 2^4 up to 2^max_size."
    echo "    max_nmemb : number of element begin with 2^12 up to 2^max_nmemb."
else
    rm -f evaluate.tmp
    min_size=4; max_size=$1; shift      # 2^4(=16) --> 2^max_size
    if [ $max_size -lt $min_size ]; then max_size=$min_size; fi
    min_nmemb=12; max_nmemb=$1; shift   # 2^12(=1024) --> 2^max_nmemb
    if [ $max_nmemb -lt $min_nmemb ]; then max_nmemb=$min_nmemb; fi
    options=$*
    src/random.awk `echo 2^$max_nmemb-1|bc` > data.tmp  # create a data file
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
                echo $cmd | tee /dev/stderr >> evaluate.tmp
                $cmd data.tmp | tee -a evaluate.tmp | \
                    awk -v sz=$size -v nmemb=$nmemb -v OFS="\t" '{print $1, nmemb, sz, $4}'
            fi
        done
    done
fi
