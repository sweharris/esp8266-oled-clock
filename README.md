# esp8266-oled-clock

This is a simple test program to display a clock on an SSD1306 based
OLED from a NodeMCU ESP8266-12E

The display I used was https://www.amazon.com/gp/product/B085NHJYBV
but any similar SSD1306 should work.

Wire the board up:

    GND -- GND
    VCC -- 3.3V
    SCL -- D1 -- GPIO5
    SDA -- D2 -- GPIO4

You will need to make a file `network_conn.h` with contents similar to

    const char* ssid       = "your-ssid";
    const char* password   = "your-wpa-passwd";
    const char* mqttServer = "your-MQTT-server";
    const int mqttPort     = 1883;

You'll also need to generate a Timezone file for your local timezone.

From a Unix machine this is how I generated one for New York

    xxd -i /usr/share/zoneinfo/America/New_York | sed -e 's/unsigned char.*/unsigned char PROGMEM mytz[]={/' > mytz.h

This file has been included in the repo for simple testing.

The code will connect to your network, start NTP and then display the
current time in GMT and localtime.
