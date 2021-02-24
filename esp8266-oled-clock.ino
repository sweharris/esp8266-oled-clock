// We require "Adafruit GFX" and "Adafruit SSD1306" for talking to the
// oled board.
//
// These are available from the Arduino library manager.
//
// Also needs https://github.com/rstephan/TimeZoneInfo
//   cd ~/Arduino/libraries
//   git clone https://github.com/rstephan/TimeZoneInfo
//
// From a Unix machine create your timezone file
// eg
//  xxd -i /usr/share/zoneinfo/America/New_York | sed -e 's/unsigned char.*/unsigned char PROGMEM mytz[]={/' > mytz.h

#include "network_conn.h"

#include <ESP8266WiFi.h>
#include <time.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Timezone stuff
#include <TimeZoneInfo.h>
#include "mytz.h"
TimeZoneInfo tzi;

WiFiClient espClient;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// Using the default Wire values built into the wire library for your board
// We don't need to define a reset
#define OLED_RESET -1
Adafruit_SSD1306 display(128,32,&Wire,OLED_RESET);

void setup() {
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  while(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    delay(1000);
  }

  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  display.print("MAC: ");
  display.println(WiFi.macAddress());
  display.display();

  WiFi.mode(WIFI_STA);  // Ensure we're in station mode and never start AP
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.print(": ");

  // Wait for the Wi-Fi to connect
  int i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(++i);
    Serial.print(' ');
  }

  Serial.println();
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  // Get the current time.  We're using TimeZoneInfo data for timezones,
  // so this sets GMT
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  tzi.setLocation_P(mytz);

  delay(1000);
  
#ifdef NETWORK_UPDATE
    __setup_updater();
#endif
}

void loop() { 
  time_t now = time(nullptr);
  String tm = ctime(&now);
  tm.trim();
  Serial.print("UTC time: " + tm + " --> ");
  now = tzi.utc2local(now);
  tm = ctime(&now);
  tm.trim();
  Serial.println("Local time: "+tm);

  // Put a \n after the date and time (which should cause year to not display)
  tm[10]='\n';
  tm[19]='\n';

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print(tm);
  display.display();

#ifdef NETWORK_UPDATE
  __netupdateServer.handleClient();
#endif

  delay(1000);
}
