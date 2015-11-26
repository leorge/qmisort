#!/bin/sh -v

# Download java programs from princeton.edu
wget http://introcs.cs.princeton.edu/java/stdlib/stdlib.jar
wget http://algs4.cs.princeton.edu/23quicksort/QuickX.java

# Download a killer data generator from Github
wget https://github.com/leorge/qmisort/raw/master/KillQuickX.pl
chmod +x *.pl

# Show versions
java -version

perl --version

# compile & test
javac -cp .:stdlib.jar QuickX.java

seq -w 1 16 | shuf | xargs echo  # sample data

seq -w 1 16 | shuf | java -cp .:stdlib.jar QuickX | xargs echo

# generate killer data
./KillQuickX.pl 50 | xargs echo  # sample

./KillQuickX.pl 40000 | java -cp .:stdlib.jar QuickX 2>&1 | head 

nl -ba QuickX.java | grep 94
