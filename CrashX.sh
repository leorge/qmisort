#!/bin/sh -v

# Show version
java -version
perl --version

# Download from princeton.edu and compile
wget http://introcs.cs.princeton.edu/java/stdlib/stdlib.jar
wget http://algs4.cs.princeton.edu/23quicksort/QuickX.java
jar xf stdlib.jar 
javac -cp :. QuickX.java 

# Download from Github and test scripts
wget https://github.com/leorge/QuickJava/raw/master/KillQuickX.pl
wget https://github.com/leorge/QuickJava/raw/master/QuickX.pl
chmod +x *.pl
#./KillQuickX.pl 64 | xargs ./QuickX.pl 

# Crash
./KillQuickX.pl 40000 | java -cp :. QuickX 2>&1 | head 

