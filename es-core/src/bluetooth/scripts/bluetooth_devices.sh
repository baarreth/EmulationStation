#!/bin/bash

shopt -s lastpipe

bluetoothctl devices |
while read l1
  do
    devname=${l1:25}
    macaddr=${l1:7:17}

    BT_CONNECTED=""
    BT_PAIRED=""
    BT_TRUSTED=""
    bluetoothctl info ${macaddr} |
    while read l2
    do
        v1=`echo ${l2} | awk '{print $1}'`
        v2=`echo ${l2} | awk '{print $2}'`
        case $v1 in
            Connected:)
                BT_CONNECTED=$v2
            ;;
            Paired:)
                BT_PAIRED=$v2
            ;;
            Trusted:)
                BT_TRUSTED=$v2
            ;;
        esac
    done
    echo -e "${macaddr}\t${BT_PAIRED}\t${BT_TRUSTED}\t${BT_CONNECTED}\t${devname}"
done > ~/.emulationstation/bluetooth_devices.log

# MAC address        Paired  Trusted  Connected  Name
# 00:71:CC:8A:3A:28    no       no       no     xubuntu-0

if [ -s ~/.emulationstation/bluetooth_devices.log ]
  then
    echo -e "MAC address        Paired  Trusted  Connected  Name"
    awk -F "\t" '{printf("%s   %3s      %3s      %3s     %s\n", $1, $2, $3, $4, $5)}' ~/.emulationstation/bluetooth_devices.log
  else
    echo "No bluetooth device registered"
    exit 1
fi
