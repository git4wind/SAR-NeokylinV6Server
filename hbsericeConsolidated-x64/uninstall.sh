#!/bin/bash
service hbSC stop
/usr/local/hbserver/script/auth_syspassword.sh 
rm -rf /usr/local/hbserver
rm -rf /usr/lib64/libhbla_hostinfo.so /usr/lib64/libhbla_sslcon.so
rm -f /etc/rc6.d/S99hbSC
rm -f /etc/rc5.d/S99hbSC
rm -f /etc/rc4.d/S99hbSC
rm -f /etc/rc3.d/S99hbSC
rm -f /etc/rc2.d/S99hbSC
rm -f /etc/rc1.d/S99hbSC
rm -f /etc/rc0.d/S99hbSC
rm -f /etc/init.d/hbSC
rm -f /lib64/security/pam_hbla.so
#sed -i '/\/usr\/local\/hbserver\/script\/hbla_login_checkmsg.sh > \/dev\/null 2>&1 &/d' /etc/rc.d/rc.local
sed -i '/pam_hbla.so/'d /etc/pam.d/login
sed -i '/pam_hbla.so/'d /etc/pam.d/gdm-password
sed -i '/pam_hbla.so/'d /etc/pam.d/gnome-screensaver
rmmod dev
