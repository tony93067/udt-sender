#!/bin/bash
# Program:
#	5	 BK TCP Server

Method=("cubic" "bbr")
BK=5
for me in ${Method[@]}
do
	./downlink_run5.sh
	ps
	./client 140.117.171.182 $BK $me
	killall -9 background_client_downlink
	sleep 50
	ps
done
	

