#include <Arduino.h>

#include "json_settings.hpp"
#include "ota_default.hpp"
#include "ble_scanner.h"
#include "core_state.hpp"

/// External Libraries
#include <WiFi.h>
#include <PubSubClient.h>

/// GLOBAL VARIABLES
core2_state networkState;
core1_state core1State;
TaskHandle_t Task1;  // Second thread for managing network 

WiFiClient espClient;
PubSubClient mqttClient(espClient);

extern const uint8_t settings_json[] asm("_binary_src_settings_json_start");


// Embed some html and css for a basic web page to control this ESP32 / Canon remote
extern const uint8_t index_html[] asm("_binary_src_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_src_index_html_end");


void setup() {
    Serial.begin(115200);
    Serial.println("Startup: Loading JSON settings from file...");

      JsonDocument doc;
    if(!LoadJsonSettings(doc))
    {
        /// TODO HANDLE THIS ERROR - e.g. FLASH THE LED
        Serial.println("ERROR: Investigate what went wrong in loading JSON file");
    }

    JsonObject root = doc.as<JsonObject>();

    if(!LoadDefaultParameters(root))
    {
        Serial.println("ERROR: Investigate what went wrong in processing the JSON object");
    }

    try
    {
    networkState.ssdp_name      =   root["ssdp_name"].as<String>();
    networkState.ssdp_modelname =   root["ssdp_modelname"].as<String>();

    Serial.printf("SSDP Name: (%s), model name (%s)",networkState.ssdp_name.c_str(), networkState.ssdp_modelname.c_str());
    Serial.println("");

    String tmp = root["mqtt_server"].as<String>();

    if(tmp.length())
        networkState.mqtt_server  = tmp;

    tmp = root["mqtt_user"].as<String>();

    if(tmp.length())
        networkState.mqtt_user     = tmp;

    tmp = root["mqtt_password"].as<String>();

    if(tmp.length())
        networkState.mqtt_password = tmp;
        networkState.mqtt_topicOUT = root["mqtt_topic"].as<String>();

        Serial.printf("MQTT Server: (%s), User (%s), Password (%s)",networkState.mqtt_server.c_str(), networkState.mqtt_user.c_str(), networkState.mqtt_password.c_str());
        Serial.println("");
    }
    catch(const std::exception& e)
    {
        Serial.println(e.what());
    }

    // Read in SSDP settings 
    try
    {
        networkState.ssdp_name      =   root["ssdp_name"].as<String>();
        networkState.ssdp_modelname =   root["ssdp_modelname"].as<String>();

        Serial.println(String("SSDP Name: " + networkState.ssdp_name + " model name " + networkState.ssdp_modelname).c_str());    
    }
    catch(const std::exception& e)
    {
        Serial.println(e.what());
    }  

    Serial.println("Setup: Initializing BLE scanner...");
    init_ble_scanner();
}

void loop() 
{

}