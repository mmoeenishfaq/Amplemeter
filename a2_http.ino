
#include <arduino.h>
#include <ESP8266WebServer.h>


// http handlers
void
handleConfigure()
{
  Serial.println("[HTTP] configure request is received");

  char buf[CFG_DOC_SIZE];
  strcpy(buf, server.arg("plain").c_str());

  Serial.println(String("[HTTP] ") + buf);

  // parse json body to get configuration data
  StaticJsonDocument<jsonBufferSize> doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));
  if (err) { //Check for errors in parsing
    Serial.println(String("[HTTP] falied to parse JSON body") + err.c_str());
    server.send(404, "text/plain", String("failed to parse JSON body") + err.c_str());
    return;
  }


  const char * ssid = doc["wifi_ssid"];
  const char * password =  doc["wifi_password"];
  const char * svr = doc["server"];

  if (ssid == nullptr || password == nullptr) {
    // do something
    Serial.println("[HTTP] either wifi_ssid or wifi_password is missing");
    Serial.println("[HTTP] waiting for further requests");
    server.send(404, "text/plain", "ssid or password is not specified");
    return;
  }

  strcpy(cfgString, buf);

  strlcpy(cfgWifiSSID, doc["wifi_ssid"], 32);
  strlcpy(cfgWifiPassword, doc["wifi_password"], 16);
  if (svr != nullptr || svr != "") {
    strlcpy(cfgServer, svr, 128);
  } else {
    Serial.println("[HTTP] server was not specified");
    Serial.println(String("[HTTP] the default server ") + cfgServer + " will be used");
  }

  String res = buildConfigureResponse(meterID.c_str());

  server.send(200, "text/json", res);

  saveConfig();

  isConfigured = true;
}

String buildConfigureResponse(const char * meterID)
{
  String jsonResponse;

  openJsonString(jsonResponse);

  addJsonMember("meter_id", meterID, jsonResponse);


  DateTime now = rtc.now();
  addJsonMember("rtc_time", _toString(now), jsonResponse);

  closeJsonString(jsonResponse);

  Serial.println(String("[HTTP] ") + jsonResponse);

  return jsonResponse;

}




void handleRoot()
{
  server.send(200, "text/plain", "yeah, you are connected");
}

void handleNotFound()
{
  Serial.println("[HTTP] invalid request path");
  server.send(404, "text/plain", "404: Not found");
}

void startServer()
{
  server.on("/", HTTP_GET, handleRoot); // Optional
  server.on("/configure", HTTP_POST, handleConfigure);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("[HTTP] HTTP server started");

}

void listenForRequests()
{
  Serial.println("[HTTP] Waiting for the App to connect");
  while (!isConfigured) {
    server.handleClient();
  }

  // wait until the response actually is sent
  delay(2000);

  turnWifiOff(); // to save power
}
