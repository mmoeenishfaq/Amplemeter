
void startAccessPoint()
{
  Serial.println("[WIFI] Configuring access point...");
  Serial.println(String("[WIFI] ssid:") + APssid);

  Serial.println(String("[WIFI] password:") + APpassword);

  WiFi.mode(WIFI_AP);

  WiFi.softAP(APssid, APpassword);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("[WIFI] AP IP address: ");
  Serial.println(myIP);

}

void connectWifi()
{
  WiFi.mode(WIFI_STA);  // learn from WiFiManager

  StaticJsonDocument<CFG_DOC_SIZE> doc;
  //  DeserializationError err = deserializeJson(doc, cfgString);
  //
  //  if (err) {
  //    Serial.println(String("[WIFI] failed to deserialize configuraiton data =>") + err.c_str());
  //    Serial.println(String("[WIFI] ") + cfgString);
  //    return;
  //  }

  //  const char * ssid = doc["wifi_ssid"];
  //  const char * password = doc["wifi_password"];

  Serial.println(String("[WIFI] cfgwifiSSID:") + cfgWifiSSID);
  Serial.println(String("[WIFI] cfgwifiPassword:") + cfgWifiPassword);

  WiFi.begin(cfgWifiSSID, cfgWifiPassword);
  
  Serial.print("[WIFI] Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("[WIFI] Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void turnWifiOff()
{
  WiFi.mode(WIFI_OFF);
  Serial.println("[WIFI] wifi is turned off");
}

void testWifi()
{
  connectWifi();
}

void testAccessPoint()
{
  startAccessPoint();
  startServer();
  listenForRequests();
}
