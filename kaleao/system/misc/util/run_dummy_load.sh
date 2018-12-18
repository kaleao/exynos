#!/bin/bash

dummyvar=10
interval=1000

while true;
do
	if [ "$i" == "$interval" ]; then
		i=0
		sleep 0.001
	fi
		
	dummyvar=$(($dummyvar+10))
	i=$(($i + 1))
done
