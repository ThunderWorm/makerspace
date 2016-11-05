
ADC_MODE(ADC_VCC); //vcc read-mode

#include <ESP8266WiFi.h>
//#include <dht11.h>
#include <DHT.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#include "credentials.h"
extern "C" {
#include "user_interface.h" // this is for the RTC memory read/write functions
}
#define RTCMEMORYSTART 66
#define RTCMEMORYLEN 125
#define VCC_ADJ  1.0476   // measure with your voltmeter and calculate that the number mesured from ESP is correct

#define COLLECT 17
#define SEND 66
#define SLEEPTIME 300000000

#define SPARKFUN_BATTERY 1

#define DHT11_PIN 12
#define DHT11_ONPIN 13
#define DHTTYPE DHT22   // DHT 22  (AM2302)

#define BMP_SCK 14                  // Serial clock pin for BMP (10k pullup to +5V)
#define BMP_MISO 2                 // SDO, has 10k pullup
#define BMP_MOSI 4                 // SDI, no pullup
#define BMP_CS 5                  // CS, has 10k pullup

typedef struct {
  int magicNumber;
  int valueCounter;
} rtcManagementStruc;

rtcManagementStruc rtcManagement;

typedef struct {
  float battery;
  float temp;
  float hum;
  float bmptemp;
  float bmpalt;
  float bmpp;
} rtcStore;

rtcStore rtcValues;

int i;
int buckets;
unsigned long startTime;

WiFiClient client;

//Adafruit_BMP280 bme; // I2C
//Adafruit_BMP280 bme(BMP_CS); // hardware SPI
//Adafruit_BMP280 bme(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);


void setup() {
  
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("Booted ");

  /*Wire.begin(2,14); // Search i2c devices 
  byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknow error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
 */

    
  
  rst_info *rsti;
  rsti = ESP.getResetInfoPtr();

  switch (rsti->reason) {

    case 5:
      Serial.println(" from RTC-RESET (ResetInfo.reason = 5)");
      break;
    case 6:
      Serial.println(" from POWER-UP (ResetInfo.reason = 6)");
      rtcManagement.magicNumber = COLLECT;
      rtcManagement.valueCounter = 0;
      break;
  }

  system_get_rst_info();

  buckets = (sizeof(rtcValues) / 4);
  if (buckets == 0) buckets = 1;
  // Serial.print("Buckets ");
  //  Serial.println(buckets);
  system_rtc_mem_read(64, &rtcManagement, 8);
  // Serial.print("Magic Number ");
  //  Serial.println(rtcManagement.magicNumber);

  // initialize System after first start
  if (rtcManagement.magicNumber != COLLECT && rtcManagement.magicNumber != SEND ) {
    rtcManagement.magicNumber = COLLECT;
    rtcManagement.valueCounter = 0;
    system_rtc_mem_write(64, &rtcManagement, 8);
    Serial.println("Initial values set");
    ESP.deepSleep(10, WAKE_RF_DISABLED);
  }
  if (rtcManagement.magicNumber == COLLECT) {   // Read sensor and store
    if (rtcManagement.valueCounter <= 5 /*RTCMEMORYLEN / buckets*/) {
      Serial.println("Sensor reads");
      pinMode(DHT11_ONPIN, OUTPUT);
      digitalWrite(DHT11_ONPIN, HIGH);
      //dht11 DHT;
      DHT dht(DHT11_PIN, DHTTYPE);
      rtcValues.battery = ESP.getVcc() * VCC_ADJ ;
      //rtcValues.other = rtcManagement.valueCounter;
      //DHT.read(DHT11_PIN);
      //rtcValues.temp = DHT.temperature;
      //rtcValues.hum = DHT.humidity;
      dht.begin();
      int ms = millis() + 500;
      while(ms > millis());
      rtcValues.temp = dht.readTemperature();
      rtcValues.hum = dht.readHumidity();

      Adafruit_BMP280 bme; // I2C
      if (!bme.begin()) {  
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
        //while (1);
      }
      else
      {    
        rtcValues.bmptemp = bme.readTemperature();
        rtcValues.bmpp = bme.readPressure();
        rtcValues.bmpalt = bme.readAltitude(662);
      }
      
      int rtcPos = RTCMEMORYSTART + rtcManagement.valueCounter * buckets;
      system_rtc_mem_write(rtcPos, &rtcValues, buckets * 4);
      system_rtc_mem_write(rtcPos, &rtcValues, 4);
      system_rtc_mem_write(64, &rtcManagement, 4);

      Serial.print("Counter : ");
      Serial.print(rtcManagement.valueCounter);
      Serial.print(" Pos: ");
      Serial.print(rtcPos);
      Serial.print(", bat: ");
      Serial.print(rtcValues.battery);
      Serial.print(", temp: ");
      Serial.print(rtcValues.temp);
      Serial.print(", pressure: ");
      Serial.print(rtcValues.bmpp);
      rtcManagement.valueCounter++;
      system_rtc_mem_write(64, &rtcManagement, 8);
      Serial.println("before sleep W/O RF");
      ESP.deepSleep(SLEEPTIME, WAKE_NO_RFCAL);
    }
    else {    // set initial values
      rtcManagement.magicNumber = SEND;
      rtcManagement.valueCounter = 0;
      system_rtc_mem_write(64, &rtcManagement, 8);
      Serial.println("before sleep w RF");
      ESP.deepSleep(10, WAKE_RFCAL);
    }
  }
  else {  // Send to Cloud
    

   String message = "a=a";
  //String ubidotsMsg = "[";
  String tewinfoMsg = "[";
  for (i = 0; i <= 5/*RTCMEMORYLEN / buckets*/; i++) {
      int rtcPos = RTCMEMORYSTART + i * buckets;
      system_rtc_mem_read(rtcPos, &rtcValues, sizeof(rtcValues));
      /*Serial.print("i: ");
      Serial.print(i); 
      Serial.print(" Position ");
      Serial.print(rtcPos);
      Serial.print(", battery: ");
      Serial.print(rtcValues.battery);
      Serial.print(", temp: ");
      Serial.print(rtcValues.temp);
      Serial.print(", hum: ");
      Serial.println(rtcValues.hum);*/
      message.concat("&");
      message.concat(i);
      message.concat("[t]=");
      message.concat(rtcValues.temp);
      message.concat("&");
      message.concat(i);
      message.concat("[h]=");
      message.concat(rtcValues.hum);
      message.concat("&");
      message.concat(i);
      message.concat("[b]=");
      message.concat(rtcValues.battery);
      message.concat("&");
      message.concat(i);
      message.concat("[bt]=");
      message.concat(rtcValues.bmptemp);
      message.concat("&");
      message.concat(i);
      message.concat("[a]=");
      message.concat(rtcValues.bmpalt);
      message.concat("&");
      message.concat(i);
      message.concat("[p]=");
      message.concat(rtcValues.bmpp);

      /*if(ubidotsMsg != "[")
      {
        ubidotsMsg.concat(",");
      }
      ubidotsMsg.concat("{\"temp\":\"");
      ubidotsMsg.concat(rtcValues.temp);
      ubidotsMsg.concat("\",\"hum\":\"");
      ubidotsMsg.concat(rtcValues.hum);
      ubidotsMsg.concat("\",\"v\":\"");
      ubidotsMsg.concat(rtcValues.battery);
      ubidotsMsg.concat("\"}");
     */

      if(tewinfoMsg != "[")
      {
        tewinfoMsg.concat(",");
      }
      tewinfoMsg.concat("{\"temp\":\"");
      tewinfoMsg.concat(rtcValues.temp);
      tewinfoMsg.concat("\",\"hum\":\"");
      tewinfoMsg.concat(rtcValues.hum);
      tewinfoMsg.concat("\",\"v\":\"");
      tewinfoMsg.concat(rtcValues.battery);
      tewinfoMsg.concat("\",\"bt\":\"");
      tewinfoMsg.concat(rtcValues.bmptemp);
      tewinfoMsg.concat("\",\"a\":\"");
      tewinfoMsg.concat(rtcValues.bmpalt);
      tewinfoMsg.concat("\",\"p\":\"");
      tewinfoMsg.concat(rtcValues.bmpp);
      tewinfoMsg.concat("\"}");

      yield();
    }
    //ubidotsMsg.concat("]");
    tewinfoMsg.concat("]");
    Serial.println(message);
    //Serial.println(ubidotsMsg);
    Serial.println(tewinfoMsg);
    startTime = millis(); 
    Serial.println(); 
    Serial.println();
    WiFi.mode(WIFI_STA);
    Serial.print("Start Sending values. Connecting to ");
    Serial.println(TEWASP_ssid);
    WiFi.begin(TEWASP_ssid, TEWASP_password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Sending ");
    Serial.println(message);
    sendBracsekMessage(SPARKFUN_BATTERY, message);
    sendTEWInfoMessage(tewinfoMsg);
    //sendubidotsMessage(ubidotsMsg);
    //sendTEWMessage(SPARKFUN_BATTERY, message);
    /*for (i = 0; i <= 5/*RTCMEMORYLEN / buckets*//*; i++) {
      int rtcPos = RTCMEMORYSTART + i * buckets;
      system_rtc_mem_read(rtcPos, &rtcValues, sizeof(rtcValues));
      /*Serial.print("i: ");
      Serial.print(i);
      Serial.print(" Position ");
      Serial.print(rtcPos);
      Serial.print(", battery: ");
      Serial.print(rtcValues.battery);
      Serial.print(", temp: ");
      Serial.print(rtcValues.temp);
      Serial.print(", hum: ");
      Serial.println(rtcValues.hum);*/
      /*sendSparkfun(SPARKFUN_BATTERY, rtcValues.temp, rtcValues.hum, rtcValues.battery);
      /*ubidotsMsg = "";
      ubidotsMsg.concat("{\"temp\":\"");
      ubidotsMsg.concat(rtcValues.temp);
      ubidotsMsg.concat("\",\"hum\":\"");
      ubidotsMsg.concat(rtcValues.hum);
      ubidotsMsg.concat("\",\"v\":\"");
      ubidotsMsg.concat(rtcValues.battery);
      ubidotsMsg.concat("\"}");
      Serial.println(ubidotsMsg);
      sendubidotsMessage(ubidotsMsg);*/
      /*yield();
      break;
    }*/
    rtcManagement.magicNumber = COLLECT;
    rtcManagement.valueCounter = 0;
    system_rtc_mem_write(64, &rtcManagement, 8);
    Serial.print("Writing to Cloud done. It took ");
    Serial.print(millis() - startTime) / 1000.0;
    Serial.println(" Seconds ");
    ESP.deepSleep(SLEEPTIME, WAKE_NO_RFCAL);
  }
}


void loop() {}

