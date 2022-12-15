#!/bin/bash
# Program:
#	3	 BK TCP Server

Method=("cubic" "bbr")
BK=3

for me in ${Method[@]}
do
	./downlink_run3.sh
	ps
	./client 140.117.171.182 $BK $me
	killall -9 background_client_downlink
	sleep 50
	ps
done
	

