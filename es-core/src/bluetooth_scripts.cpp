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
    if(c) return c;
    return system(scriptpath);
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
    if(c) return c;
    FILE * pipe = popen(scriptpath, "r");
    if (pipe == NULL) return 1;

    char line[1024];
    while (fgets(line, 1024, pipe)) {
        strtok(line, "\n");
        btlist.push_back(std::string(line));
    }
    pclose(pipe);
    return 0;
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
    if(c) return c;
    return system(scriptpath);
#endif
}

int runClearBluetoothCache() {
#ifdef WIN32
	return 1; // do nothing
#else
    return system("clear_bt_cache");
#endif
}


int runBluetoothConnect(const char * c_str) {
#ifdef WIN32
	return 1; // do nothing
#else
    if(strncmp(&c_str[39], "no", 2) == 0) {             // not connected
        if(strncmp(&c_str[21], "no", 2) == 0) {         // not paired
            std::string command("bluetoothctl pair ");
            command.append(c_str, 17);
            if(system(command.c_str()) != 0) return 1;  // pairing fail
            nsleep(0.5);                                // pairing success, wait
        }
        if(strncmp(&c_str[30], "no", 2) == 0) {         // not trusted
            std::string command("bluetoothctl trust "); 
            command.append(c_str, 17);
            if(system(command.c_str()) != 0) return 2;  // trusting fail
            nsleep(0.25);                               // wait
        }
        std::string command("bluetoothctl connect ");
        command.append(c_str, 17);
        if(system(command.c_str()) != 0) return 3;      // connecting fail
    } else {
        std::string command("bluetoothctl disconnect ");
        command.append(c_str, 17);
        if(system(command.c_str()) != 0) return 4;
    }
	return 0;
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
    if(c) return c;
    return system(scriptpath);
#endif
}
