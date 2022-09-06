#!/bin/bash
# Program:
#	execute 20 receivers at the time
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
export PATH
i=1
for (( c=1; c<=20; c++ ))
do
./background_client_uplink 140.117.171.182 $c &
sleep 1
done
echo "Background Create Finish"

