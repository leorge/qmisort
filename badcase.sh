#!/bin/bash
if [ $# -eq 0 ] ; then
	echo "$0  N  [options]"
else
	N=$1; shift

	echo random.awk $N
	src/random.awk $N | Release/Sort -N $N -T 5

	for i in median3.awk n111.awk n11n.awk n1n1.awk n1nn.awk nn11.awk nn1n.awk nnn1.awk nnnn.awk tim.awk valley.awk middle.pl swap3.pl trad.pl
	do
		echo $i $N
		src/$i $N | Release/Sort -N $N $*
	done
fi
