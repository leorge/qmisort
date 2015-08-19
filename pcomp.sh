#!/bin/sh
#
# Get comparisons of pointer sorting in various N.
#
if [ $# -eq 0 ]; then
	echo $0 numbers
	exit;
fi
for N in $*; do
    src/random.awk $N | Debug/Sort -N $N -l $N -P mqibLhHBCup -V 1
done | tee tmp$$

awk -f - tmp$$ <<'EOF'
BEGIN {N = "name"}
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
