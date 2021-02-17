#!/bin/bash
for (( i=0; i<20; i++ )); do
    if [ $i -gt 9 ]
    then
        infile='test'$i'.mtf'
        outfile='test'$i'.txt'
    else
        infile='test0'$i'.mtf'
        outfile='test0'$i'.txt'
    fi
    ./mtf2text2 $infile
    diff $outfile tests/$outfile > temp.txt
    if [ ! -s temp.txt ]
    then
        echo $infile passed.
    else
        echo $infile failed.
    fi
    rm temp.txt
done
