#include "flipper.h"
#include "display.h"
#include "flipclock.h"
#include <WiFi.h>
#include "time.h"
#include <WiFiManager.h>

const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

FlipClock* flipClock;

void setup() {
  Serial.begin(9600);

  fetchNtpTime();

  time_t now;
  time(&now);

  flipClock = new FlipClock(*new Display());
  flipClock->configureOffsetFromTimestamp(((uint64_t)now)*1000, 2);
  // flipClock->configureOffsetFromHourAndMinute(timeinfo.H, timeinfo.M, 2);
}

void fetchNtpTime() {
  WiFi.mode(WIFI_STA);
  WiFiManager wm;

  wm.setConfigPortalTimeout(45);
  bool res = wm.autoConnect("AutoConnectAP", "password");

  if (!res) {
    Serial.println("Failed to connect");
  } else {
    Serial.println("connected...yeey :)");

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay, 10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
}

void loop() {
  flipClock->tick(millis());
  // digitalWrite(COL_EN, LOW);
  // delay(100);
  // digitalWrite(COL_EN, HIGH);
  delay(100);
}
