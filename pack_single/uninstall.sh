#!/bin/bash

service hbla stop
rpm -e hbla
rm -rf /usr/local/hb
rm -f /usr/lin/libhbla_*
rm -f /etc/init.d/hbla
rm -f /etc/rc5.d/S100hbla

