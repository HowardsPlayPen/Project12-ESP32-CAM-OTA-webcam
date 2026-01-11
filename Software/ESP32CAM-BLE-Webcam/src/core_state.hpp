#pragma once

#include "json_settings.hpp"

#include <vector>
#include <freertos/semphr.h>


struct core2_state
{
  bool networkConnected = false;
  bool mqttConnected = false;

  /// MQTT details
  String mqtt_server = "";

  String mqtt_user = ""; // Again, this is hardwired to an MQTT user on Pidesktop1
  String mqtt_password = "";

  String mqtt_topicOUT = "";
  char mqtt_client_id[23]; // This is auto generated in connection functions below 

  String ssdp_name;
  String ssdp_modelname;

  
};

struct core1_state
{
  
  bool action_event = false; // Set to true when an action is required - eg. user turned fan on manually
  SemaphoreHandle_t mutex;
};