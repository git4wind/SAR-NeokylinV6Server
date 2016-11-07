#!/bin/bash
a=1
usersuccess1=$(who | awk /tty/ | wc -l)
userfailed1=$(lastb | wc -l)
usleep 100
while [ 1 -eq 1 ]
do 
	usersuccess2=$(who | awk /tty/ | wc -l)
	userfailed2=$(lastb | wc -l)

	if [ $usersuccess1 -lt $usersuccess2 ] ; 
	then
		/usr/local/hbserver/script/hbla_login_sendmsg success
	elif [ $userfailed1 -lt $userfailed2 ] ;
	then
		/usr/local/hbserver/script/hbla_login_sendmsg failed
	else
		usleep 100
	fi
	usersuccess1=$usersuccess2
	userfailed1=$userfailed2
done

