/*
LED Rose Bush

Microcontroller: Wemos R1 Mini

Author: Matthew Picone
Date: 18/Feb/2022

Description: Controls RGB Leds embedded inside 3d Printed Roses
*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#define OTA_HOSTNAME "LED Rose Bush"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

const char* ssid = "WIFI SSID";
const char* password = "WIFI PW";

// Create Instance of PWM Driver
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Set starting states of LED's
int REDLED = 0;
int GREENLED = 0;
int BLUELED = 0;
int LEDOFF = 0;
int PWMMINLED  0
int PWMMAXLED  4096
int CYCLESTATE = 1;
int CYCLEMAX 3

// Timer Setup
unsigned long previousMillis = 0;
const long interval = 20000;
unsigned long previousMillisLED = 0;
const long intervalLED = 10;

void setup() {
// Start serial and PWM driver init.
  Serial.begin(9600);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency
  Wire.setClock(400000);

  // Start Wifi / OTA
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
void loop() {
  ArduinoOTA.handle();

  // Get current Millis
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // Check cycle state - Changes color chan to cycle
    previousMillis = currentMillis;
    if (CYCLESTATE <= CYCLEMAX) {
      CYCLESTATE ++;
    } else {
      CYCLESTATE = 1;
    }
  }

  unsigned long currentMillisLED = millis();
  // Cycle the color chan's
  if (currentMillisLED - previousMillisLED >= intervalLED) {
    
    previousMillisLED = currentMillisLED;

    if (CYCLESTATE == 1) {
      if (GREENLED <= 4092) {
        GREENLED ++;
      } else if (REDLED >= 3) {
        REDLED --;
      }
    }

    if (CYCLESTATE == 2) {
      if (BLUELED <= 4092 ) {
        BLUELED ++;
      } else if (GREENLED >= 3) {
        GREENLED --;
      }
    }

    if (CYCLESTATE == 3) {
      if (REDLED <= 4092) {
        REDLED ++;
      } else if (BLUELED >= 3) {
        BLUELED --;
      }
    }
  }

  // Write the colors to the PWM driver
  pwm.setPWM(1, REDLED, (PWMMAXLED - REDLED));
  pwm.setPWM(2, GREENLED, (PWMMAXLED - GREENLED));
  pwm.setPWM(3, BLUELED, (PWMMAXLED - BLUELED));

  pwm.setPWM(4, REDLED, (PWMMAXLED - REDLED));
  pwm.setPWM(5, BLUELED, (PWMMAXLED - BLUELED));
  pwm.setPWM(6, GREENLED, (PWMMAXLED - GREENLED));

  pwm.setPWM(7, BLUELED, (PWMMAXLED - BLUELED));
  pwm.setPWM(8, REDLED, (PWMMAXLED - REDLED));
  pwm.setPWM(9, GREENLED, (PWMMAXLED - GREENLED));

  pwm.setPWM(10, GREENLED, (PWMMAXLED - GREENLED));
  pwm.setPWM(11, REDLED, (PWMMAXLED - REDLED));
  pwm.setPWM(12, BLUELED, (PWMMAXLED - BLUELED));

  pwm.setPWM(13, GREENLED, (PWMMAXLED - GREENLED));
  pwm.setPWM(14, BLUELED, (PWMMAXLED - BLUELED));
  pwm.setPWM(15, REDLED, (PWMMAXLED - REDLED));
}
