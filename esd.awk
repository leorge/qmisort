#!/usr/bin/awk -f
#
#   Calculate Estimated Standard Deviation.
#
#       s=sqrt(sum((Xi-mean)^2)/(n-1))
#
#   Usage : $0 val1 val2 ...
#
#
BEGIN {
    for (i = 1; i < ARGC; i++) sum += ARGV[i];
    n = ARGC - 1;
    mean = sum / n;
    sum = 0;    # reset
    for (i = 1; i < ARGC; i++) {
        delta = ARGV[i] - mean;
        sum += delta * delta;
    }
    esd = sqrt(sum / (n - 1));
    printf("mean = %.0f\tesd = %.0f\t%3.0f %%\n", mean, esd, 100 * esd / mean);
    exit;
}
