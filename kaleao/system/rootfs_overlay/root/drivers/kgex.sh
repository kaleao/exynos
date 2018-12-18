#!/bin/bash

insmod /root/drivers/kmax_pci.ko
insmod /root/drivers/kgex.ko ex_nr=$1 tx_coalesce=10 rx_coalesce=10

ifconfig eth0 192.168.3.10$1
