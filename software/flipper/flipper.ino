#include "flipper.h"
#include "display.h"
#include "flipclock.h"
#include <WiFi.h>
#include "time.h"
#include <WiFiManager.h>

#define MY_TZ "CET-1CEST,M3.5.0,M10.5.0/3"
#define MY_NTP_SERVER "de.pool.ntp.org"

FlipClock* flipClock;
struct tm timeinfo;

void setup() {
  Serial.begin(9600);

  fetchNtpTime();

  flipClock = new FlipClock(*new Display());
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

    configTime(0, 0, MY_NTP_SERVER);
    setenv("TZ", MY_TZ, 1);
    tzset();
    printLocalTime();
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void printLocalTime() {
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
  delay(1000);

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  flipClock->drawTime(timeinfo.tm_hour, timeinfo.tm_min);
}
