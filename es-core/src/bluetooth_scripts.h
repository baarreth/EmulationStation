#pragma once

#include <string>
#include <vector>

int setSystemBluetooth();
int runScanBluetooth();
int runBluetoothDevices(std::vector<std::string> & btlist);
int runClearBluetoothCache();
int runBluetoothConnect(const char * c_str);
int runBluetoothRemove();

enum {
    BLUETOOTH_SUCCESS = 0,
    BLUETOOTH_CHANGING_STATE,
    BLUETOOTH_NOT_PAIRED_YET,
    BLUETOOTH_PAIR_ERROR,
    BLUETOOTH_TRUST_ERROR,
    BLUETOOTH_CONNECT_ERROR,
    BLUETOOTH_DISCONNECT_ERROR,
    BLUETOOTH_TMP_SCRIPT_ERROR,
    BLUETOOTH_PIPE_SCRIPT_ERROR,
    BLUETOOTH_SET_ERROR,
    BLUETOOTH_SCAN_ERROR,
    BLUETOOTH_CLEAR_CACHE_ERROR,
    BLUETOOTH_REMOVE_ERROR
};

extern const char * btErrorMsg[];
