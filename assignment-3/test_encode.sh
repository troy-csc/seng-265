#!/bin/bash
for (( i=11; i<20; i++ )); do
	infile='test'$i'.txt'
	outfile='test'$i'.mtf'
	./encode.py $infile
	diff $outfile tests/$outfile > temp.txt
	if [ ! -s temp.txt ]
	then
		echo $infile passed.
	else
		echo $infile failed.
	fi
	rm temp.txt
done
