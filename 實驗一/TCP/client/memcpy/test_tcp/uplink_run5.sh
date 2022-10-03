#!/bin/bash
# Program:
#	execute 5 receivers at the time
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
export PATH
i=1
p=8888
for (( c=1; c<=5; c++ ))
do
./background_client_uplink 140.117.171.182 $c $p&
p=$(($p+1))
sleep 1
done
echo "Background Create Finish"

