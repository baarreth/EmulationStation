#include "Window.h"
#include "Log.h"
#include "guis/GuiMsgBox.h"
#include "guis/GuiSettings.h"
#include "guis/GuiBluetooth.h"
#include "bluetooth_scripts.h"

void GuiBluetoothConnectDevice(Window * window, const std::string & st, GuiSettings * gs) {
    const bool connected = !st.substr(39,2).compare("no") ? false : true;
    window->pushGui(new GuiMsgBox(window, connected ? "REALLY TRY DISCONNECT?" : "REALLY TRY CONNECT?", "YES", 
    [window, st, gs, connected] { 
        int c = runBluetoothConnect(st.c_str());
        window->removeGui(gs);      // remove the outdated table
        if(c != 0) {
            LOG(LogWarning) << "Bluetooth connect with non-zero result! " << btErrorMsg[c];
//             it is pretty aggressive to remove all scanned devices just because we had a connection error
//             if(runBluetoothRemove() != 0)
//                 LOG(LogWarning) << "Bluetooth remove devices terminated with non-zero result!";
            window->pushGui(new GuiMsgBox(window, btErrorMsg[c], "OK"));
        } else {
            GuiBluetoothListDevices(window);
            window->pushGui(new GuiMsgBox(window, "SUCCESS", "OK"));
        }
    }, "NO", nullptr));
}

void GuiBluetoothListDevices(Window * window) {
    std::vector<std::string> btlist;
    if(runBluetoothDevices(btlist) != 0) {
        LOG(LogWarning) << "Bluetooth devices terminated with non-zero result!";
        window->pushGui(new GuiMsgBox(window, "LIST DEVICES FAILED", "OK"));
    } else if( ! btlist.empty() ) {
        auto gs = new GuiSettings(window, "BLUETOOTH DEVICES");
        ComponentListRow r;
        for(auto it = btlist.begin(); it != btlist.end(); it++) {
            if(it != btlist.begin()) {  // ignore table header
                std::string & st = *it;
                r.makeAcceptInputHandler([window, st, gs] { GuiBluetoothConnectDevice(window, st, gs); });
            }
            r.addElement(std::make_shared<TextComponent>(window, it->c_str(), Font::get(FONT_SIZE_TINY, FONT_PATH_MONO), 0x777777FF), true);
            gs->addRow(r);
            r.elements.clear();
        }
        window->pushGui(gs);
    } else {
        window->pushGui(new GuiMsgBox(window, "NO DEVICE FOUND", "OK"));
    }
}

void GuiBluetoothScanDevices(Window * window) {
    window->pushGui(new GuiMsgBox(window, "IT WILL TAKE ABOUT 10s", "OK", 
    [window] {
        if(runScanBluetooth() != 0) {
            LOG(LogWarning) << "Scan bluetooth terminated with non-zero result!";
            window->pushGui(new GuiMsgBox(window, "BLUETOOTH SCAN FAILED", "OK"));
        } else {
            GuiBluetoothListDevices(window);
        }
    }, "BACK", nullptr));
}

void GuiBluetoothRemoveDevices(Window * window) {
    window->pushGui(new GuiMsgBox(window, "REALLY REMOVE DEVICES?", "YES", 
    [window] { 
        if(runBluetoothRemove() != 0) {
            LOG(LogWarning) << "Bluetooth remove devices terminated with non-zero result!";
            window->pushGui(new GuiMsgBox(window, "REMOVE DEVICES FAIL", "OK"));
        }
    }, "NO", nullptr));
}

void GuiBluetoothClearCache(Window * window) {
    window->pushGui(new GuiMsgBox(window, "IT WILL TAKE ABOUT 10s", "OK", 
    [window] { 
        if(runClearBluetoothCache() != 0) {
            LOG(LogWarning) << "Clear bluetooth cache terminated with non-zero result!";
            window->pushGui(new GuiMsgBox(window, "BLUETOOTH CLEAR CACHE FAILED", "OK"));
        }
    }, "BACK", nullptr));
}


const char * btHelpElement[] = {
    "Put this in your mind before anything!",
    "Connecting for the first time",
    "Failed after trying to connect",
    "Safely disconnecting your controller",
    "Reconnecting your controller",
    "Still unable to connect",
    "Clearing the bluetooth cache",
    "\0"
};

const char * btHelpMsg[] = {
    "You are trying to connect two devices through a system that usually uses opensource and proprietary software. It has a long history of security concerns. It shares the same bandwidth as your wi-fi, and can even use the same hardware (circuit and anthenna). So, many problems happen frequently. You must be responsive to minimize them, and patient to overcome them.",
    
    "Put your game controller in Pairing Mode and then select SCAN DEVICES in the Bluetooth Menu. It takes about 10s until the Device List be automatically loaded. If necessary, renew your controller's Pairing Mode to avoid a timeout. Select your controller in the Device List. It will try to pair, trust and finally connect.",
    
    "Look at the controller's status after selecting LIST DEVICES. If it's not paired, put it on Pairing Mode, and select it from the list. However, if it is already paired and trusted, turn it off, turn it on, and then wait a few seconds. It should connect by itself. If it doesn't, select it (as if it wasn't paired). If it still fails, read \"Still unable to connect\".",
    
    "Most Bluetooth Systems are paranoid, and if you turn off your controller without disconnecting it, it may freak out and even freeze your wi-fi connection. So, before turning off your controller, select LIST DEVICES in the Bluetooth Menu, and then select your controller. Pay attention to the controller's status! You don't need to do that if your controller isn't connected.",
    
    "Select LIST DEVICES in the Bluetooth Menu and check if your controller is there. If it is already paired and trusted, turn it on and wait a few seconds. If the controller does not connect by itself, try to select it. If your controller is not paired, put it in Pairing Mode and select it. If your controller is not present in the Device List, read \"Connecting for the first time\".",
    
    "Select REMOVE DEVICES in the Bluetooth Menu, and read \"Connecting for the first time\". If this procedure still fails, try restarting the Bluetooth Service on your computer (if you don't know how to do that, just restart the system) or you can try \"Clearing the bluetooth cache\".",
    
    "Select CLEAR CACHE in the Bluetooth Menu. If this option is not present, this version of EmulationStation was not installed system-wide. However, on linux, you can easily delete it by hand (it is usually located at /var/lib/bluetooth).",
    
    "\0"
};

void GuiBluetoothHelp(Window * window) {
    auto gs = new GuiSettings(window, "BLUETOOTH INSTRUCTIONS");
    ComponentListRow r;
    const char ** bt_help_element = btHelpElement;
    const char ** bt_help_msg = btHelpMsg;
    while (**bt_help_element != '\0' && **bt_help_msg != '\0') {
        r.elements.clear();
        r.makeAcceptInputHandler([window, bt_help_msg, bt_help_element] {
            window->pushGui(new GuiMsgBox(window, *bt_help_msg, "OK", nullptr));
        });
        r.addElement(std::make_shared<TextComponent>(window, *bt_help_element, Font::get(FONT_SIZE_MEDIUM, FONT_PATH_REGULAR), 0x777777FF), true);
        gs->addRow(r);
        bt_help_msg++;
        bt_help_element++;
    }
    window->pushGui(gs);
}
