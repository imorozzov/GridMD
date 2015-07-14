#!/bin/sh
echo "Starting prog.sh with arguments: x=$1, y=$2"
echo "$1 + $2 = `expr $1 + $2`" >>permfile.txt
cat infile1.txt >outfile.txt
cat infile2.txt >>outfile.txt
mv indir outdir
ls -lR
sleep 5
echo username: `whoami`, host: `hostname`
WrongCommand   # this generates an output to stderr
