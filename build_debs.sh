#!/bin/bash

rm -rf kaleao/updates/debs/*

for p in "recovery" "ubuntu"; do
	echo "Package set: $p"
	for f in kaleao/updates/packages/$p/*
	do	
		if [ -d $f ]; then
			for desc in $f/*.pdesc
			do
			       	args="`basename $f` $(echo `basename $desc` | cut -f 1 -d '.') $p"
				./build_deb.sh $args & 
				if [ "$?" != 0 ]; then
					exit 1
				fi
			done
		fi
	done
done

while (ps -a | grep -q "build_deb.sh"; ); do
	sleep 1
done
echo "All deb build jobs finished"
