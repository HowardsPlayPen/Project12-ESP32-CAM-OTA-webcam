#include <Arduino.h>
#include "ble_scanner.h"

void setup() {
    Serial.begin(115200);
    Serial.println("Scanning...");

    init_ble_scanner();
}

void loop() {}