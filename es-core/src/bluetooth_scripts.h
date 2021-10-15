#pragma once

#include <string>
#include <vector>

int setSystemBluetooth();
int runScanBluetooth();
int runBluetoothDevices(std::vector<std::string> & btlist);
int runClearBluetoothCache();
int runBluetoothConnect(const char * c_str);
int runBluetoothRemove();
