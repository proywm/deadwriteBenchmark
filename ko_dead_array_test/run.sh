#!/bin/bash

kmodN="k_array_test"
insmod $kmodN.ko
sleep 1
cat /sys/module/$kmodN/sections/.text > sections.text 
lsmod > lsmod.text
sleep 1
rmmod $kmodN

dmesg > dmesg.txt

cat dmesg.txt
cat sections.text
cat lsmod.text
