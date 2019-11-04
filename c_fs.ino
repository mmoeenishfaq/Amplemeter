
/*
   We are not using SD, instead, we are using flash that comes with the board.
   There is library to access flash file system
*/

// write configuration data to a file, note the confiugrations are stored in a global jsonDocument
void saveConfig()
{
  //  Serial.println("write configuraiton data to file");
  //  Serial.println();

  //  char toWrite[256];
  //  serializeJson(cfgDoc, toWrite);

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File f = SPIFFS.open("/config.json", "w");
  if (f) {
    f.print(cfgString);
    f.close();
    Serial.println("[FS] configuration data was written successfully");
  } else {
    Serial.print("failed to write configuration file =>");
  }
}

// load configuration data from flash into the global cfgDoc json object
bool readConfig()
{

  File f = SPIFFS.open("/config.json", "r");
  if (f) {
    Serial.println("[FS] found configuration file and opened");

    size_t sz = f.size();

    size_t bytesRead = f.readBytes(cfgString, sz);

    StaticJsonDocument<CFG_DOC_SIZE> doc;
    DeserializationError err = deserializeJson(doc, cfgString);
    if (err) {
      Serial.println("[FS] failed to deserialize configuration data");
      return false;
    }    

    f.close();

    const char * ssid = doc["wifi_ssid"];
    const char * password =  doc["wifi_password"];
    const char * svr = doc["server"];

    if (ssid == nullptr || password == nullptr || svr == nullptr) {
      Serial.println("[FS] wifi_ssid or wifi_password or server is not set");
      return false;
    }

    strlcpy(cfgWifiSSID, ssid, 32);
    strlcpy(cfgWifiPassword, password, 16);
    strlcpy(cfgServer, svr, 128);

    Serial.println(String("[FS] SSID:") + cfgWifiSSID + "   password:" + cfgWifiPassword + "  server:" + cfgServer);

    isConfigured = true;

    return true;

  } else {
    Serial.println("[FS] failed to open file config.json");
    return false;
  }
}

void populateTestConfig()
{
  Serial.println("populating test configuration data");

  //  cfgDoc["wifi_ssid"] = "NETGEAR";
  //  cfgDoc["wifi_password"] = "liting1981";
  //  cfgDoc["server"] = "xxx.amplemeter.com";
  //  cfgDoc["location"] = "china";
  //  cfgDoc["user"] = "hankgao";
  //  cfgDoc["label"] = "test information";
}

void testFsTab() {

  populateTestConfig();
  saveConfig();
  readConfig();

  //  char buf[256];
  //  serializeJson(cfgDoc, buf);
  //  Serial.println(buf);

}
