#!/bin/bash

if [ "`which bluetoothctl`" == "" ] || [ "`grep bluetoothd /proc/*/comm`" == "" ]
 then
  echo bluetoothctl not found or bluetoothd not running
  exit 1
fi

export BT_MAC_DEFAULT=`bluetoothctl list | grep default | awk '{print $2}'`

if [ "${BT_MAC_DEFAULT}" == "" ]
 then
  echo no bluetooth controller found
  exit 1
fi

BT_MAC=""
BT_POWERED=""
BT_PAIRABLE=""

# "make the last pipeline segment run in the current shell, so that variable creations/modifications made there are visible to it"
shopt -s lastpipe

bluetoothctl show ${BT_MAC_DEFAULT} |
while read line
  do
    v1=`echo ${line} | awk '{print $1}'`
    v2=`echo ${line} | awk '{print $2}'`
    case $v1 in
        Controller)
            BT_MAC=$v2
        ;;
        Powered:)
            BT_POWERED=$v2
        ;; 
        Pairable:)
            BT_PAIRABLE=$v2
        ;;
    esac
done

if [ "${BT_POWERED}" == "no" ]; then bluetoothctl power on; sleep 0.5; fi
if [ "${BT_PAIRABLE}" == "no" ]; then bluetoothctl pairable on; fi
