
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <FS.h> // Include the SPIFFS library
#include <ESP8266HTTPClient.h>
#include <time.h>
#include <Ticker.h>  //Ticker Library
#include "RTClib.h"
#include "EmonLib.h"

#ifndef APSSID
#define APSSID "ample" // the SSID name could be changed after discussion with Andy
#define APPSK "123456789"
#endif

String meterID;

const char *APssid = APSSID;
const char *APpassword = APPSK;

const char * Version = "0.1.6";

const int jsonBufferSize = 500;

ESP8266WebServer server(80); // port could be changed in the future

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

int deviceStatus = 0; //

#define CFG_DOC_SIZE 512
char cfgString[CFG_DOC_SIZE];
char cfgWifiSSID[32];
char cfgWifiPassword[16];
char cfgServer[128]; // http://122.144.212.237:5000/api/submit

bool isConfigured = false;

// The following is an example of configuration object
// {
//    "wifi_ssid": "",
//    "wifi_password": "",
//    "server": "xx.xx.xx.xx",
//    "location": "china",
//    "label": "other information"
// }

// SampleBuffer is to store 10 minutes of samples
#define USE_SEC    // for now, we collect data of every second of AI analysis
struct SampleBuffer {
#ifdef USE_SEC
  double tenMinutesBuf[10 * 60];
#else
  double tenMinutesBuf[10];
#endif
  char firstSampleTime[20]; // 2019-09-30 15:00:00
  bool isFull;
};

// we have two ten minutes buffers
SampleBuffer sampleBufs[2];

String getChipId()
{
  char id[10];
  sprintf(id, "%08X", ESP.getChipId());
  String chipid = id;

  return chipid;
  
}


void setup()
{
  meterID = getChipId();
  
  strcpy(cfgServer, "http://95.169.21.35:5000/api/submit"); // default server path, can be configured from App

  delay(1000);
  Serial.begin(115200);
  Serial.flush();
  Serial.println();

  Serial.println(String("[MAIN] Version: ") + Version); 
  Serial.printf("[MAIN] Meter ID (chip ID) is %s\n", meterID.c_str());

  initializeRTC();

  if (SPIFFS.begin()) {
    Serial.println("[MAIN] SPIFFS successfully initiated");
  } else {
    Serial.println("[MAIN] failed to initiate SPIFFS");
    return;
  }

  readConfig();

  // Test code, will be removed later
  isConfigured = false;

  if (!isConfigured) {
    configure();
  }

  setupEmon();

}

void configure()
{
  startAccessPoint();
  startServer();

  listenForRequests();

}

void loop()
{
  // submit data according to the time set by the server
  for (int i =  0; i < 2; i++) {
    if (sampleBufs[i].isFull) {
      Serial.println("[MAIN] submit data to the server");
      submitData(&sampleBufs[i]);
      sampleBufs[i].isFull = false;
    }
  }
}
