#!/bin/bash

MAXREAD=30
outread=""
read_coproc() {
    for((readcount=0;readcount<MAXREAD;readcount++)) ; do
        read -t $1 -r outread <&"${COPROC[0]}"
        if [ "$outread" == "" ]; then break; fi
        echo $outread
    done
}

send_coproc() {
    echo "## $1"
    echo -e "$1" >&"${COPROC[1]}"
    sleep 0.5
}

## Scan for devices
if [ -e ~/.emulationstation/bluetooth_scan_list.log ]; then rm ~/.emulationstation/bluetooth_scan_list.log; fi
coproc bluetoothctl
send_coproc "scan on"
read_coproc 0.20
sleep 3
i=0
while [ $i -lt 6 ]
 do
  i=$((i+1))
  read -t 0.5 -r o <&"${COPROC[0]}"
  echo $o
#   V=`echo $o | awk ' $0 ~ "[NEW]" {print $1}'`
  if [ "$o" ]
   then
    echo $o >> ~/.emulationstation/bluetooth_scan_list.log
  fi
done
read_coproc 0.20
send_coproc "scan off"
read_coproc 0.25
send_coproc "quit"
