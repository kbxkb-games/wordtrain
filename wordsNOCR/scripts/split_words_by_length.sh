#!/bin/bash

let maxlen=`wc -L $1 | cut -f 1 -d " "`
filename_suffix="_length_words"

for i in `seq 5 $maxlen`
do
	splitfilename="$i$filename_suffix"

	echo "Processing words of length "$i"..."
	sed -n 's/^\(.\{'"$i"'\}\)$/\1/p' $1 > $splitfilename
	numwords=`wc -l $splitfilename`
	echo "Wrote "$numwords" into file "$splitfilename
	echo ""
done
