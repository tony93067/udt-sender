#!/bin/bash
# Program:
#	execute 2000 sender at the time
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
export PATH
i=1
while [ "$i" <= "2000" ]
do
echo "Client" $i "generate"
./background_client 140.117.171.182 &
sleep 1
i=$(($i+1))
done

