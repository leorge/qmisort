#!/usr/bin/gawk -f
function show() {
	if (pattern == "random.awk") {
		limit = usec * 2;
		print "Time limit = " limit " usec."
	}
	else if (pattern != "") {
		print "Pattern = " pattern;
		for (f in result) {
			usec = result[f];
			if (usec > limit) print f, usec;
		}
	}
}
{
	if (NF == 2) {
		show();
		pattern = $1; sz = $2;
	} else {
		usec = $4; result[$1] = usec;
	}
}
END {show()}
