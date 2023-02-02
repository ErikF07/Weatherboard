/*Used IDE:
			Visual Studio Code(PlatformIO)
Framework:
			Arduino
Used Libraries:
			Blynk by Volodymyr Shymanskyy
			Adafruit BME280 Library by Adafruit
Used Hardware Components:
			firebeetle ESP32
			Adafruit_BME280

Main Code Under:
			src/main.cpp

Project created by:
			Connor Lacy
			Erik Frank
      
*/



//including librearies
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

//sensor libraries
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//Wifi and Blynk lib
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

//defining Pins for Bme Sensor
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

//Wifi name/password
const char ssid[] = "FRITZ!Box 7583 ZO";
const char pass[] = "49371636178067577689";

//Blynk informations
#define BLYNK_TEMPLATE_ID "TMPLv_UhZLvX"
#define BLYNK_DEVICE_NAME "Weatherboard2000V2"
#define BLYNK_AUTH_TOKEN "5Ai4XoDiD-GkQHDBlrIEIhgM5wZw31Xn"
char auth[] = BLYNK_AUTH_TOKEN;

//Specifying Sensor
Adafruit_BME280 bme; 

//creating Variables
//RTC_DATA_ATTR Saves the Variables information while in sleep mode
RTC_DATA_ATTR int tAlt;
RTC_DATA_ATTR int hAlt;
RTC_DATA_ATTR int pAlt;

//only needed for the blinking three times on first boot feature, delete for longer battery lifetime
RTC_DATA_ATTR boolean booted=false;

//void Setup:called on startup, also after sleep mode
void setup() {
  //for lower energy consumtion, can be deleted
  setCpuFrequencyMhz(10);
  //only needed for the blinking feature, delete for longer battery lifetime
  if (booted==false){
    delay(2000);
    pinMode(LED_BUILTIN,OUTPUT);
    digitalWrite(LED_BUILTIN,HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN,LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN,HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN,LOW);
    booted=true;
  }
  //activating the sensor and setting the mode to forced, that it only measures when told to
  bme.begin(0x76);
  bme.setSampling(
    Adafruit_BME280::MODE_FORCED,
    Adafruit_BME280::SAMPLING_X1,
    Adafruit_BME280::SAMPLING_X1,
    Adafruit_BME280::SAMPLING_X1,
    Adafruit_BME280::FILTER_OFF 
    );
  //measuring
  bme.takeForcedMeasurement();
  //introducing Variables needed for knowing to send or not
  bool changed=false;
  int tNeu = bme.readTemperature();
  int hNeu = bme.readHumidity();
  int pNeu = bme.readPressure() / 100.0F+60;
  //testing for change in the measurements
  if (hAlt-hNeu>5||hNeu-hAlt>5||tAlt-tNeu>2||tNeu-tAlt>2||pAlt-pNeu>3||pNeu-pAlt>3){
    //Sending isnt possible without a CPU frequenzy of 160
    setCpuFrequencyMhz(160);
    //starting Blynk and sending data
    Blynk.begin(auth, ssid, pass);
    Blynk.virtualWrite(V0,tNeu);
    tAlt=tNeu;
    Blynk.virtualWrite(V1,hNeu);
    hAlt=hNeu;
    Blynk.virtualWrite(V2,pNeu);
    pAlt=pNeu;
    delay(500);
  }
  //letting device sleep for time in brackets
  esp_sleep_enable_timer_wakeup(1000);
  esp_deep_sleep_start();
}
//Arduino needs this to function 
void loop(){}
