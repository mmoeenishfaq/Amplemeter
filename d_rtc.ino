

void initializeRTC()
{
  Serial.println("[RTC] starting RTC module");

  if (!rtc.begin()) {
    Serial.println("[RTC] Couldn't find RTC");
    deviceStatus = 1; // RTC is not functioning
    return;
  }

  Serial.println("[RTC] RTC module started");

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  if (rtc.lostPower()) {
    Serial.println("[RTC] RTC lost power, lets set the time!");
//    // following line sets the RTC to the date &amp; time this sketch was compiled
//    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//    // This line sets the RTC with an explicit date &amp; time, for example to set
//    // January 21, 2014 at 3am you would call:
//    //     rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  DateTime now = rtc.now();
  Serial.println(String("[RTC] Current time is ") + _toString(now));

  delay(3000);
}

// _toString is to replace DateTime.toString, which always returns the same result as the one returned by its first call. Very strange behavior
char _buf[20]; // has to be global, we cannot use a local varialbe and return it
const char* _toString(DateTime dt)
{
  sprintf(_buf, "%04d-%02d-%02d %02d:%02d:%02d",
          dt.year(), dt.month(), dt.day(),
          dt.hour(), dt.minute(), dt.second()
         );
  return _buf;
}

void testRTC()
{
  initializeRTC();

  for (int i = 0; i < 3; i++) {
    DateTime n = rtc.now();
    Serial.println(_toString(n));
    delay(5000);
  }

}
