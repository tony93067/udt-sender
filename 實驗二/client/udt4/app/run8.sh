#!/bin/bash
# Program:
#	8	 BK TCP Server
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
LB_PATH=/home/tony/實驗code/論文code/實驗二/client/udt4/src
TCP_PATH=/home/tony/實驗code/論文code/實驗一/TCP/client/memcpy/test_tcp
UDT_PATH=/home/tony/實驗code/論文code/實驗二/client/udt4/app
MSS=("1500" "1250" "1000" "750" "500" "250" "100")
MSS1=("1500" "750" "100")
BK=8
export PATH
for (( c=2; c<=3; c++ ))
do
	for str in ${MSS[@]}
	do
		cd $TCP_PATH
		./uplink_run8.sh
		ps
		export LD_LIBRARY_PATH=$LB_PATH
		cd $UDT_PATH
		./udtclient 140.117.171.182 5000 $str $c $BK
		sleep 50
		date -R
	done
	
done

