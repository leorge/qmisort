#!/bin/sh -x
cat $0
for N in 1000 100000 1000000 ; do
  src/random.awk $N | Release/test_sort -N $N -TR 10
  echo ""
done
