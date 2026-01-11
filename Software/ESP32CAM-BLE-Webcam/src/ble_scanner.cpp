#include "ble_scanner.h"
#include <Arduino.h>

ble_state state = {
    .scanTime = 5 * 1000, 
    .pBLEScan = nullptr, 
    .active = false
};

namespace {
    class ScanCallbacks : public NimBLEScanCallbacks {
        void onDiscovered(const NimBLEAdvertisedDevice* advertisedDevice) override {
            Serial.printf("Discovered Advertised Device: %s \n", advertisedDevice->toString().c_str());
        }

        void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override {
            Serial.printf("Advertised Device Result: %s \n", advertisedDevice->toString().c_str());
        }

        void onScanEnd(const NimBLEScanResults& results, int reason) override {
            Serial.print("Scan Ended; reason = ");
            Serial.println(reason);
            state.active = !state.active;
            state.pBLEScan->setActiveScan(state.active);
            Serial.printf("scan start, active = %u\n", state.active);
            state.pBLEScan->start(state.scanTime);
        }
    };

    ScanCallbacks scanCallbacks;
}

void init_ble_scanner() {
    NimBLEDevice::init("active-passive-scan");
    state.pBLEScan = NimBLEDevice::getScan();
    state.pBLEScan->setScanCallbacks(&scanCallbacks);
    state.pBLEScan->setActiveScan(state.active);
    state.pBLEScan->setInterval(100);
    state.pBLEScan->setWindow(100);
    state.pBLEScan->start(state.scanTime);
}

