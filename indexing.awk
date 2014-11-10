#!/usr/bin/awk -f
#
#	Get ratio of indexing time in a index sort.
#
#	usage
#		$ src/evaluate 10 20 -MG | src/matrix.awk -v OFS="\t" | src/indexing.awk
#
/^size=/	{	# headline
	if (! headline) {
		printf "rate of idx";
		for (i = 0; ++i < NF; ) printf "\t%4d", 9 + i;
		print "";
		headline = 1;
	}
	split($1, sz, /=/)
	printf "size =%5d", sz[2];
}
/^imerge_sort/ {split($0, a)}
/^merge_pointer/ {
	split($0, b);
	for (i = 1; i++ < NF;)  printf "\t%4d", int(100 * (1 - b[i] / a[i]));
	print "";	# New line
}
