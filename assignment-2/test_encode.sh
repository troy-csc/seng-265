#!/bin/bash
for (( i=0; i<11; i++ )); do
	infile='test0'$i'.txt'
	outfile='test0'$i'.mtf'
	if [ $i -eq 10 ]
	then
		infile='test'$i'.txt'
		outfile='test'$i'.mtf'
	fi
	./text2mtf.py $infile
	diff $outfile tests/$outfile > temp.txt
	if [ ! -s temp.txt ]
	then
		echo $infile passed.
	else
		echo $infile failed.
	fi
	rm temp.txt
done
