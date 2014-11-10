#!/usr/bin/awk -f
/^sort.. start/ && /array/ {a++; array=1}			# start array_sort
array && /^sort.. start/ && /merge/ {m++; array=0}	# merge_sort in array_sort
END {print a, m}
