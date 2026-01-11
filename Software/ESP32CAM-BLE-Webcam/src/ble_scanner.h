#ifndef BLE_SCANNER_H
#define BLE_SCANNER_H

#include <NimBLEDevice.h>

struct ble_state {
    int         scanTime; // Scan period in ms
    NimBLEScan* pBLEScan;
    bool        active;   // Is there a scan active - note NimBLE creates a new thread for scanning
};

extern ble_state state;

void init_ble_scanner();

#endif // BLE_SCANNER_H
