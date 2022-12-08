#!/bin/bash
# Program:
#	execute 3 senders at the time
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
export PATH
i=1
p=8888
for (( c=1; c<=3; c++ ))
do
./background_client_downlink 140.117.171.182 $c $p "cubic" &
p=$(($p+1))
sleep 1
done
echo "Background Create Finish"

