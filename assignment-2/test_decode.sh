#!/bin/bash
for (( i=0; i<11; i++ )); do
	infile='test0'$i'.mtf'
	outfile='test0'$i'.txt'
	if [ $i -eq 10 ]
	then
		infile='test'$i'.mtf'
		outfile='test'$i'.txt'
	fi
	./mtf2text.py $infile
	diff $outfile tests/$outfile > temp.txt
	if [ ! -s temp.txt ]
	then
		echo $infile passed.
	else
		echo $infile failed.
	fi
	rm temp.txt
done
