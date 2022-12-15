#!/bin/bash
# Program:
#	0	 BK TCP Server

Method=("cubic" "bbr")
BK=0

for me in ${Method[@]}
do
	./client 140.117.171.182 $BK $me
	sleep 50
	ps
done
	

