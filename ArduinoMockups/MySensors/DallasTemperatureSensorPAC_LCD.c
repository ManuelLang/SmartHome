/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * DESCRIPTION
 *
 * Example sketch showing how to send in DS1820B OneWire temperature readings back to the controller
 * http://www.mysensors.org/build/temp
 */


// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable and select radio type attached
#define MY_RADIO_RF24

// Enabled repeater feature for this node
#define MY_REPEATER_FEATURE

#define MY_TRANSPORT_WAIT_READY_MS 1000 

// For RF-NANO
#define MY_RF24_CE_PIN 10 
#define MY_RF24_CS_PIN 9

#define MY_NODE_ID 23

#include <SPI.h>
#include <MySensors.h>  
#include <DallasTemperature.h>
#include <OneWire.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


#define COMPARE_TEMP 1 // Send temperature only if changed? 1 = Yes 0 = No

#define ONE_WIRE_BUS 3 // Pin where dallase sensor is connected 
#define MAX_ATTACHED_DS18B20 5
unsigned long SLEEP_TIME = 5000; // Sleep time between reads (in milliseconds)
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire); // Pass the oneWire reference to Dallas Temperature. 
float lastTemperature[MAX_ATTACHED_DS18B20];
int numSensors=0;
bool receivedConfig = false;
bool metric = true;
// Initialize temperature message
MyMessage msg(0,V_TEMP);

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display


void before()
{
  // Startup up the OneWire library
  sensors.begin();
}

int my_id;

void setup()  
{ 
    my_id = getNodeId() ;             // mon node id
  
#ifdef MY_DEBUG
    Serial.print("Node_ID: ");
    Serial.println(my_id);
#endif

  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("********************");
  lcd.setCursor(0,1);
  lcd.print("PAC Temp. regulation");
  lcd.setCursor(0,2);
  lcd.print("    Hello, world!   ");
  lcd.setCursor(0,3);
  lcd.print("********************");
  
  // requestTemperatures() will not block current thread
  sensors.setWaitForConversion(false);
}

void presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("PAC monitoring RF-NANO", "1.2");

  // Fetch the number of attached temperature sensors  
  numSensors = sensors.getDeviceCount();
#ifdef MY_DEBUG
    Serial.print("Found sensors: ");
    Serial.println(numSensors);
#endif

  // Present all sensors to controller
  for (int i=0; i<numSensors && i<MAX_ATTACHED_DS18B20; i++) {   
     present(i, S_TEMP);
  }
}

void loop()     
{
#ifdef MY_DEBUG
    Serial.println("Loop...");
#endif

  // sendHeartbeat();

  // Fetch temperatures from Dallas sensors
  sensors.requestTemperatures();

  // query conversion time and sleep until conversion completed
  int16_t conversionTime = sensors.millisToWaitForConversion(sensors.getResolution());
  // sleep() call can be replaced by wait() call if node need to process incoming messages (or if node is repeater)
  wait(conversionTime);

#ifdef MY_DEBUG
    Serial.print("numSensors: ");
    Serial.println(numSensors);
#endif

  // Read temperatures and send them to controller 
  for (int i=0; i<numSensors && i<MAX_ATTACHED_DS18B20; i++) {
#ifdef MY_DEBUG
    Serial.print("sensor #");
    Serial.println(i);
#endif
    // Fetch and round temperature to one decimal
    float temperature = static_cast<float>(static_cast<int>((getControllerConfig().isMetric?sensors.getTempCByIndex(i):sensors.getTempFByIndex(i)) * 10.)) / 10.;
#ifdef MY_DEBUG
    Serial.print("Temperature for sensor ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(temperature);
    Serial.println("°C");
#endif

    lcd.setCursor(0,i);
    char float_str_temp[5] = "";
    
    char lcd_line[20] = "";
    if (i==0){
      temperature += 0.1;  // correction sonde
      dtostrf(temperature,5,2,float_str_temp);
      sprintf(lcd_line, "Temp D.PAC: %s C ", float_str_temp);
    } else if (i==1){
      temperature += 0.0;  // correction sonde
      dtostrf(temperature,5,2,float_str_temp);
      sprintf(lcd_line, "Temp R.PAC: %s C ", float_str_temp);
    } else if (i==2){
      temperature += 0.0;  // correction sonde
      dtostrf(temperature,5,2,float_str_temp);
      sprintf(lcd_line, "Temp D.RAD: %s C ", float_str_temp);
    } else if (i==3){
      temperature += 0.0;  // correction sonde
      dtostrf(temperature,5,2,float_str_temp);
      sprintf(lcd_line, "Temp R.RAD: %s C ", float_str_temp);
    }
    lcd.print(lcd_line);

    // Only send data if temperature has changed and no error
    #if COMPARE_TEMP == 1
    if (lastTemperature[i] != temperature && temperature != -127.00 && temperature != 85.00) {
    #else
    if (temperature != -127.00 && temperature != 85.00) {
    #endif
 
      // Send in the new temperature
      send(msg.setSensor(i).set(temperature,1));
      // Save new temperatures for next compare
      lastTemperature[i]=temperature;
    }
  }
  wait(SLEEP_TIME);
}
