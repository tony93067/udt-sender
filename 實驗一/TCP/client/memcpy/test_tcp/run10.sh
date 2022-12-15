#!/bin/bash
# Program:
#	10	 BK TCP Server

Method=("cubic" "bbr")
BK=10
for me in ${Method[@]}
do
	./downlink_run10.sh
	ps
	./client 140.117.171.182 $BK $me
	killall -9 background_client_downlink
	sleep 50
	ps
done
	

