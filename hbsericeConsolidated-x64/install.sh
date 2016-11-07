#!/bin/bash
cp -r hbserver /usr/local
cp /usr/local/hbserver/lib/*.so /usr/lib64/
mv /usr/local/hbserver/hbServerCon.sh /etc/init.d/hbSC
chmod +x /etc/init.d/hbSC
ln -sf /etc/init.d/hbSC /etc/rc6.d/S99hbSC
ln -sf /etc/init.d/hbSC /etc/rc5.d/S99hbSC
ln -sf /etc/init.d/hbSC /etc/rc4.d/S99hbSC
ln -sf /etc/init.d/hbSC /etc/rc3.d/S99hbSC
ln -sf /etc/init.d/hbSC /etc/rc2.d/S99hbSC
ln -sf /etc/init.d/hbSC /etc/rc1.d/S99hbSC
ln -sf /etc/init.d/hbSC /etc/rc0.d/S99hbSC




flag=$(ls /etc/sysconfig/ | awk '{if(/^iptables$/)print "iptables exists"}')
if [ -z "$flag" ];
then 
	iptables -F
	service iptables save >/dev/null 2>&1 &
fi
cp -f /usr/local/hbserver/pam_hbla.so /lib64/security/
#sed -i '/hbla_login_checkmsg.sh/d' /etc/rc.d/rc.local
#sed -i \$a\ "/usr/local/hbserver/script/hbla_login_checkmsg.sh > /dev/null 2>&1 &" /etc/rc.d/rc.local

touch /usr/local/hbserver/pam_hblarule
touch /usr/local/hbserver/keyinfo
touch /usr/local/hbserver/hbla_login_savemsg
chmod +x /usr/local/hbserver/script/*
#/usr/local/hbserver/script/hbla_login_checkmsg.sh &

sleep 1

service hbSC start

sleep 1
echo Install sucess!!
