#!/bin/bash

shopt -s lastpipe

bluetoothctl devices |
while read line
    do
#     devname=${line:25}
    macaddr=${line:7:17}
    bluetoothctl remove ${macaddr}
done
