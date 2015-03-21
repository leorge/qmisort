#!/usr/bin/awk -f
#
#   skip if $1 is continuing 
#
$1 != first && last != "" {print last}
{first = $1; last = $0}
END {print last}
