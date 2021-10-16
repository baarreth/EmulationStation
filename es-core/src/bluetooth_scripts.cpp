#include "bluetooth_scripts.h"
#include <stdlib.h>
#include <boost/filesystem.hpp>
#include <iostream>

#include <time.h>
// this is not a good idea, but we have to do it
inline void nsleep(double t) {
    if (t > 0) {
        int s = int(t);
        double r = t - s;
        struct timespec req, rem;
        req.tv_sec = time_t(s);
        req.tv_nsec = long(r * 1e9);
        nanosleep(&req, &rem);
    }
}

const char * btErrorMsg[] = {
    "Success",
    "Changing state. Try again.",
    "Tried to connect before paired",
    "Pairing error",
    "Trusting error",
    "Connecting error",
    "Disconnecting error",
    "Tmp script error",
    "Pipe error",
    "Set error",
    "Scan error",
    "Clear cache error",
    "Remove devices error"
};

inline int create_tmp_script(const char * fname, const char * data) {
    FILE * fp = fopen(fname, "w");
    if(fp == NULL) return 1;
    size_t len = strlen(data);
    if(fwrite(data, sizeof(char), len, fp) != len) return 2;
    fclose(fp);
    std::string cmd = std::string("chmod +x ") + fname;
    return system(cmd.c_str());
}

const char bluetooth_set_sh[] =
#include "./bluetooth/bluetooth_set_sh.raw"
;

int setSystemBluetooth() {
#ifdef WIN32
	return 1; // do nothing
#else
    const char scriptpath[] = "/tmp/bluetooth_set.sh";
    int c = create_tmp_script(scriptpath, bluetooth_set_sh);
    if(c) return BLUETOOTH_TMP_SCRIPT_ERROR;
    return system(scriptpath) == 0 ? BLUETOOTH_SUCCESS : BLUETOOTH_SET_ERROR;
#endif
}

const char bluetooth_devices_sh[] =
#include "./bluetooth/bluetooth_devices_sh.raw"
;

int runBluetoothDevices(std::vector<std::string> & btlist) {
#ifdef WIN32
	return 1; // do nothing
#else
    const char scriptpath[] = "/tmp/bluetooth_devices.sh";
    int c = create_tmp_script(scriptpath, bluetooth_devices_sh);
    if(c) return BLUETOOTH_TMP_SCRIPT_ERROR;
    FILE * pipe = popen(scriptpath, "r");
    if (pipe == NULL) return BLUETOOTH_PIPE_SCRIPT_ERROR;

    char line[1024];
    while (fgets(line, 1024, pipe)) {
        strtok(line, "\n");
        btlist.push_back(std::string(line));
    }
    pclose(pipe);
    return BLUETOOTH_SUCCESS;
#endif
}

const char bluetooth_scan_sh[] =
#include "./bluetooth/bluetooth_scan_sh.raw"
;

int runScanBluetooth() {
#ifdef WIN32
	return 1; // do nothing
#else
    const char scriptpath[] = "/tmp/bluetooth_scan.sh";
    int c = create_tmp_script(scriptpath, bluetooth_scan_sh);
    if(c) return BLUETOOTH_TMP_SCRIPT_ERROR;
    return system(scriptpath) == 0 ? BLUETOOTH_SUCCESS : BLUETOOTH_SCAN_ERROR;
#endif
}

int runClearBluetoothCache() {
#ifdef WIN32
	return 1; // do nothing
#else
    return system("clear_bt_cache") == 0 ? BLUETOOTH_SUCCESS : BLUETOOTH_CLEAR_CACHE_ERROR;
#endif
}


int runBluetoothConnect(const char * c_str) {
#ifdef WIN32
	return 1; // do nothing
#else
    bool paired =       strncmp(&c_str[21], "no", 2) == 0 ? false : true;
    bool trusted =      strncmp(&c_str[30], "no", 2) == 0 ? false : true;
    bool connected =    strncmp(&c_str[39], "no", 2) == 0 ? false : true;
    if(!paired) {
        if(connected) {
            std::string command("bluetoothctl disconnect ");
            command.append(c_str, 17);
            int c = system(command.c_str());
            return c == 0 ? BLUETOOTH_CHANGING_STATE : BLUETOOTH_DISCONNECT_ERROR;
        } else {
            std::string command("bluetoothctl pair ");
            command.append(c_str, 17);
            int c = system(command.c_str());
            nsleep(1.25);                               // wait before return
            if(c != 0) return BLUETOOTH_PAIR_ERROR;
        }
    }
    if(!trusted) {
        std::string command("bluetoothctl trust ");
        command.append(c_str, 17);
        int c = system(command.c_str());
        if(c != 0) return BLUETOOTH_TRUST_ERROR;
    }
    if(!connected) {
        std::string command("bluetoothctl info ");
        command.append(c_str, 17);
        command.append(" | grep Paired | grep yes");
        int c = system(command.c_str());
        if(c != 0) return BLUETOOTH_NOT_PAIRED_YET;

        command = "bluetoothctl connect ";
        command.append(c_str, 17);
        c = system(command.c_str());
        nsleep(1.25);                               // wait before return
        if(c != 0) return BLUETOOTH_CONNECT_ERROR;
    } else {
        std::string command("bluetoothctl disconnect ");
        command.append(c_str, 17);
        int c = system(command.c_str());
        nsleep(0.75);                               // wait before return
        if(c != 0) return BLUETOOTH_DISCONNECT_ERROR;
    }
    return BLUETOOTH_SUCCESS;
#endif
}

const char bluetooth_remove_sh[] =
#include "./bluetooth/bluetooth_remove_sh.raw"
;

int runBluetoothRemove() {
#ifdef WIN32
	return 1; // do nothing
#else
    const char scriptpath[] = "/tmp/bluetooth_remove.sh";
    int c = create_tmp_script(scriptpath, bluetooth_remove_sh);
    if(c) return BLUETOOTH_TMP_SCRIPT_ERROR;
    return system(scriptpath) == 0 ? BLUETOOTH_SUCCESS : BLUETOOTH_REMOVE_ERROR;
#endif
}
