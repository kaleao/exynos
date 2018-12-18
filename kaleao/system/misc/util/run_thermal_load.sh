#!/bin/bash

TEST_NAME=thermal_test
LOAD_PATH=/root/util
LOAD_EXE=run_dummy_load.sh

CPU_MASK=0

USE_STR="Usage: $TEST_NAME <#bigcores> <#littlecores> <time in seconds>"

if [ "$#" -ne 3 ]; then
        printf "$TEST_NAME: %s\n" "$USE_STR"
        exit 0;
fi

if [ $1 -gt 4 ] || [ $1 -lt 0 ]; then
	printf "$TEST_NAME: Wrong number of big cores\n"
	exit 0;
fi


if [ $2 -gt 4 ] || [ $1 -lt 0 ]; then
	printf "$TEST_NAME: Wrong number of little cores\n"
	exit 0;
fi

BIG_CORES=$1
LITTLE_CORES=$2

for i in `seq 1 4`
do
	if [ $i -gt $BIG_CORES ]; then
		break;
	fi
	CPU_MASK=$((CPU_MASK + (0x10 << ($i-1))))
done

for i in `seq 1 4`
do
	if [ $i -gt $LITTLE_CORES ]; then		
		break;
	fi
	CPU_MASK=$((CPU_MASK + (0x1 << ($i-1))))
done

if [ $CPU_MASK -eq 0 ]; then
	printf "$TEST_NAME: No core selected!\n"
	exit 0;
fi

if [ `which taskset` == "" ]; then
	printf "$TEST_NAME: taskset utility not present!\n"
	exit 0;
fi

for i in `seq 1 8`
do
        bits=$((0x1<<($i-1)))

        if [ $(($bits & $CPU_MASK)) -ne 0 ]; then
                printf "$TEST_NAME: Starting on core %s\n" $i
                taskset $bits $LOAD_PATH/$LOAD_EXE &
        fi
done

sleep $3

temp=`cat /sys/class/thermal/thermal_zone0/temp`

printf "$TEST_NAME: Time $3 seconds. Ran on $BIG_CORES BIG cores and $LITTLE_CORES little cores. Temp: %s\n" "$(($temp / 1000))"

for i in `ps | grep $LOAD_EXE | grep -o '[0-9]\+'`
do
	kill $i &> /dev/null
done

exit 0;
