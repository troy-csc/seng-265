for (( i=0; i<20; i++ )); do
	if [ $i -lt 10 ]
	then
		rm 'test0'$i'.txt'
		rm 'test0'$i'.mtf'
	else
		rm 'test'$i'.txt'
		rm 'test'$i'.mtf'
	fi
done
