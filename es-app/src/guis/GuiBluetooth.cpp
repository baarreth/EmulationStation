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
        window->removeGui(gs);
        GuiBluetoothListDevices(window);
        if(c != 0) {
            LOG(LogWarning) << "Bluetooth connect with non-zero result!";
            window->pushGui(new GuiMsgBox(window, connected ? "DISCONNECT FAIL" : "CONNECT FAIL", "OK"));
        } else {
            window->pushGui(new GuiMsgBox(window, connected ? "DISCONNECT SUCCESS" : "CONNECT SUCCESS", "OK"));
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
