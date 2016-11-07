#!/bin/bash
#sed -i  '/opt/hbla_script/keyring_daemon > \/dev\/null 2>&1 &/d' /etc/rc.d/rc.local
#sed -i \$a\ "/opt/hbla_script/keyring_daemon > /dev/null 2>&1 &" /etc/rc.d/rc.local

if !(grep -xq '^auth.*requisite.*pam_hbla.so' /etc/pam.d/login)
then
	sed -i '3 i auth       requisite    pam_hbla.so' /etc/pam.d/login
fi

if !(grep -xq '^auth.*requisite.*pam_hbla.so' /etc/pam.d/gdm-password)
then
	sed -i '3 i auth       requisite    pam_hbla.so' /etc/pam.d/gdm-password
fi

if !(grep -xq '^auth.*requisite.*pam_hbla.so' /etc/pam.d/gnome-screensaver)
then
	sed -i '3 i auth       requisite    pam_hbla.so' /etc/pam.d/gnome-screensaver
fi

