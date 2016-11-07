#!/bin/bash
if !(grep -xq '^auth.*include.*system-auth' /etc/pam.d/login)
then
    sed -i '/^account.*required.*pam_nologin.so/ i auth       include      system-auth' /etc/pam.d/login
fi
#sed -i '/pam_hbla.so/'d /etc/pam.d/login
if !(grep -xq '^auth.*substack.*password-auth' /etc/pam.d/gdm-password)
then
    sed -i '/^auth.*optional.*pam_gnome_keyring.so/ i auth       substack      password-auth' /etc/pam.d/gdm-password
fi
#sed -i '/pam_hbla.so/'d /etc/pam.d/gdm-password
if !(grep -xq '/^auth.*include.*system-auth/' /etc/pam.d/gnome-screensaver)
then
    sed -i '/^auth.*optional.*pam_gnome_keyring.so/ i auth       include      system-auth' /etc/pam.d/gnome-screensaver
fi
#sed -i '/pam_hbla.so/'d /etc/pam.d/gnome-screensaver
