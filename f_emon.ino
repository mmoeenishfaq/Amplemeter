
/*
   Every second the device collects current data and multipy by 220v to get power data
*/

#define CT_PIN 0
#define CALIBRE 34 // 20 suggested by Sam1 and by the sensor manufacture
#define CT_READ_FREQUENCY 1 // every 1 second
#define IRMS_BUF_SIZE 600

EnergyMonitor emon1;

Ticker ctTicker;
double IrmsBuffer[IRMS_BUF_SIZE];  // buffer for RMS current
double IrmsTemBuffer[IRMS_BUF_SIZE];
volatile int IrmsReadPtr = 0;
volatile int IrmsWritePtr = 0;

int currentBuf = 0; // 0: buffer A; 1: buffer B
int minutePtr = 0; //

int secondCount = 0;
double oneMinuteAcc = 0;

void addIrms(double Irms)
{
#ifdef USE_SEC
#else
  oneMinuteAcc += Irms;
  secondCount ++;
#endif

  // Now we've collection one minutes of samples (60 samples)
  if (secondCount == 60) {

    secondCount = 0;

#ifdef USE_SEC
    Serial.println(String("[EMON] minute ") + String(minutePtr) + " passed");
    minutePtr++;
#else
    sampleBufs[currentBuf].tenMinutesBuf[minutePtr++] = oneMinuteAcc / 60;

    Serial.print("Mintue ");
    Serial.print(minutePtr - 1);
    Serial.print(" ");
    Serial.println(oneMinuteAcc / 60);

    oneMinuteAcc = 0;
#endif

    if (minutePtr == 10) {  // 10 minutes data has been collected

      Serial.println("Hey, 10 minutes have passed");

      sampleBufs[currentBuf].isFull = true;    // when the buffer is full, the data will be sent to server
      currentBuf = ++currentBuf % 2; // another buffer will be used to collect data
      if (sampleBufs[currentBuf].isFull == true) {
        Serial.println("something serious happen, please contact developer");
        sampleBufs[currentBuf].isFull == false;
      }

      resetFirstSampleTime();

      minutePtr = 0;
    }
  }

#ifdef USE_SEC
  sampleBufs[currentBuf].tenMinutesBuf[minutePtr * 60 + secondCount] = Irms;
  secondCount++;
#endif


}


void resetFirstSampleTime()
{
  DateTime now = rtc.now();
  strcpy(sampleBufs[currentBuf].firstSampleTime, _toString(now));

  Serial.print("[EMON] first sample time is :");
  Serial.println(sampleBufs[currentBuf].firstSampleTime);
}

void setupEmon()
{
  emon1.current(CT_PIN, CALIBRE);

  sampleBufs[0].isFull = false;
  sampleBufs[1].isFull = false;

  currentBuf = 0;

  secondCount = 0;
  minutePtr = 0;
  oneMinuteAcc = 0.0;

  waitUntilMinuteBoundary();

  resetFirstSampleTime();

  ctTicker.attach(CT_READ_FREQUENCY, isrCT);
}

void waitUntilMinuteBoundary()
{
  DateTime now = rtc.now();
  int s = now.second();
  if (s > 0 && s < 60) {
    Serial.println(String("[EMON] wait ") + String(60 - s) + " seconds to enter minute boundary");
    delay((60 - s) * 1000); // wait until we enter next mintue boundary
  }
}


// ISR for Current Transformer
void isrCT()
{
  double Irms = emon1.calcIrms(1480);  // Calculate Irms only

  noInterrupts();

  addIrms(Irms);

  interrupts();
}
