

const char * _getHttpClientErrMsg(int err)
{
  if (err == -1)
    return "Could not connect to the server";
  else if (err == -2)
    return "code is using the class incorrectly";
  else if (err == -3)
    return "Spent too long waiting for a reply";
  else if (err == -4)
    return "The response from the server is invalid";
  else
    return "unknown error code";
}

String createJsonResponse(SampleBuffer * sampleBuf)
{
  String jsonResponse;

  openJsonString(jsonResponse);

  addJsonMember("meter_id", meterID.c_str(), jsonResponse);
  addJsonMember("startTime", sampleBuf->firstSampleTime, jsonResponse);
#ifdef USE_SEC
  addJsonArray("currents", &sampleBuf->tenMinutesBuf[0], 10 * 60, jsonResponse, true);
#else
  addJsonArray("currents", &sampleBuf->tenMinutesBuf[0], 10, jsonResponse, true);
#endif

  closeJsonString(jsonResponse);

  Serial.println(String("[HTTPCLIENT] ") + jsonResponse);

  return jsonResponse;

}

void submitData(SampleBuffer * sampleBuf)
{
  connectWifi();

  HTTPClient http;

  http.begin(cfgServer);

  http.addHeader("Content-Type", "application/json");

  String jsonResponse =  createJsonResponse(sampleBuf);

  bool bSubmitted = false;
  for (int i = 0; i < 3; i++) {

    int httpCode = http.POST(jsonResponse.c_str());

    if (httpCode > 0) {
      String s = http.getString();
      switch (httpCode) {
        case 200:
          Serial.println("[HTTPCLIENT] power consumption datat successfully uploaded");
          bSubmitted = true;
          break;
        default:
          Serial.println("[HTTPClIENT] failed to submit data");
          Serial.println(s.c_str());
      }
    } else {
      Serial.println(String("[HTTPClIENT] failed to submit data => ") + _getHttpClientErrMsg(httpCode));
    }

    if (bSubmitted)
      break;
    else {
      Serial.println("[HTTPCLIENT] try one more time");
      delay(5000); // wait for 5 seconds before next try
    }
  }

  http.end();

  delay(5000); // fix error: request aborted
  turnWifiOff(); // to save power

  return;

}

/*
   registerSelf registers the device on the server
*/
bool registerSelf()
{
  bool isRegistered = false;

  HTTPClient http;

  http.begin("");

  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<512> doc;
  doc["meter_id"] = meterID.c_str();
  doc["session_id"] = "";

  char buf[512];
  serializeJson(doc, buf);

  int httpCode = http.POST(buf);

  if (httpCode > 0) {
    String s = http.getString();
    switch (httpCode) {
      case 200:
        isRegistered = true;
        break;
      default:
        Serial.println("failed to register meter");
        Serial.println(s.c_str());
    }

  } else {
    Serial.println("failed to register");
  }

  http.end();

  return isRegistered;

}

void testCreateJsonResponse()
{
  SampleBuffer sample;
  sample.tenMinutesBuf[0] = 0.1;
  sample.tenMinutesBuf[1] = 0.2;
  sample.tenMinutesBuf[2] = 0.1;
  sample.tenMinutesBuf[3] = 0.1;
  sample.tenMinutesBuf[4] = 0.1;
  sample.tenMinutesBuf[5] = 0.1;
  sample.tenMinutesBuf[6] = 0.1;
  sample.tenMinutesBuf[7] = 0.1;
  sample.tenMinutesBuf[8] = 0.1;
  sample.tenMinutesBuf[9] = 0.1;

  strcpy(sample.firstSampleTime, "2019-10-01 10:00:00");

  String r = createJsonResponse(&sample);

  Serial.println(String("[HTTPCLIENT] ") + r);
}
