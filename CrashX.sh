#!/bin/sh -v

# Download java programs from princeton.edu & compile
wget http://introcs.cs.princeton.edu/java/stdlib/stdlib.jar
wget http://algs4.cs.princeton.edu/23quicksort/QuickX.java
javac -cp .:stdlib.jar QuickX.java

# Download a killer data generator from Github
wget https://github.com/leorge/qmisort/raw/master/KillQuickX.pl
chmod +x *.pl

# Show versions
java -version

perl --version

# test
seq -w 1 16 | shuf | xargs echo

seq -w 1 16 | shuf | java -cp .:stdlib.jar QuickX | xargs echo

./KillQuickX.pl 50 | xargs echo

# Crash
./KillQuickX.pl 40000 | java -cp .:stdlib.jar QuickX 2>&1 | head 

nl -ba QuickX.java | grep 94
