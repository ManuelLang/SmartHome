/*
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2019 Sensnology AB
 * Full contributor list: https://github.com/mysensors/MySensors/graphs/contributors
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
 * REVISION HISTORY
 * Version 1.0 - Henrik Ekblad
 *
 * DESCRIPTION
 * Example sketch showing how to control physical relays.
 * This example will remember relay state after power failure.
 * http://www.mysensors.org/build/relay
 */

// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_RF24

// For RF-NANO
#define MY_RF24_CE_PIN 10 
#define MY_RF24_CS_PIN 9

// Enable repeater functionality for this node
#define MY_REPEATER_FEATURE

#define MY_NODE_ID 29

#include <SPI.h>
#include <MySensors.h>

#define RELAY_PIN 4  // Arduino Digital I/O pin number for first relay (second on pin+1 etc)
#define NUMBER_OF_RELAYS 1 // Total number of attached relays
#define RELAY_ON 0  // GPIO value to write to turn on attached relay
#define RELAY_OFF 1 // GPIO value to write to turn off attached relay

int my_id;
static unsigned long last_loop_time = 0;
static bool initialized = false;
unsigned long SLEEP_TIME = 6000000;  // sleep time between reads (seconds * 1000 milliseconds)
bool pulse_mode = true;
unsigned long PULSE_TIME = 250;
bool ack = 1;
MyMessage relay_msg;

void before()
{
    for (int sensor=1, pin=RELAY_PIN; sensor<=NUMBER_OF_RELAYS; sensor++, pin++) {
    // Then set relay pins in output mode
    pinMode(pin, OUTPUT);
    // Set relay to last known state (using eeprom storage)
    // digitalWrite(pin, loadState(sensor)?RELAY_ON:RELAY_OFF);
    setState(sensor, false);
  }
}

void setup()
{
  my_id = getNodeId();
  
#ifdef MY_DEBUG
    Serial.print("Node_ID: ");
    Serial.println(my_id);
#endif
}

void presentation()
{
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo("Relay", "1.1");

	for (int sensor=1, pin=RELAY_PIN; sensor<=NUMBER_OF_RELAYS; sensor++, pin++) {
		// Register all sensors to gw (they will be created as child devices)
		present(sensor, S_BINARY, "Telerupteur", ack);
	}
}


void loop()
{
  unsigned long loop_time = millis();
  if ((loop_time - last_loop_time) > SLEEP_TIME) {
    last_loop_time = loop_time;
    sendValues();
  }
}

bool getState(int relayId) {
  unsigned int relay_pin = relayId-1+RELAY_PIN;
  return loadState(relay_pin);
}

void setState(int relayId, bool newState) {
  int oldState = getState(relayId);
  #ifdef MY_DEBUG
    Serial.print("setState: relayId=");
    Serial.print(relayId);
    Serial.print(", oldState=");
    Serial.print(oldState);
    Serial.print(", newState=");
    Serial.println(newState);
  #endif
  if (oldState != newState || !initialized) {
    initialized = true;
    unsigned int relay_pin = relayId-1+RELAY_PIN;
    #ifdef MY_DEBUG
      Serial.print("Setting new state for relay_pin: ");
      Serial.println(relay_pin);
    #endif
    digitalWrite(relay_pin, newState?RELAY_ON:RELAY_OFF);
    // Store state in eeprom
    saveState(relay_pin, newState);
    relay_msg_constructor(relayId, V_STATUS);
    if (pulse_mode){
      if (newState) {
        sleep(PULSE_TIME);
        digitalWrite(relay_pin, RELAY_OFF);
      } else {
        digitalWrite(relay_pin, RELAY_ON);
        sleep(PULSE_TIME);
        digitalWrite(relay_pin, RELAY_OFF);
      }
    }
  }
  send(relay_msg.set(newState), true);
}

void relay_msg_constructor(int sensor, uint8_t type)
{
  relay_msg.setSensor(sensor);
  relay_msg.setType(type);
}

void sendValues() {
#ifdef MY_DEBUG
    Serial.println("Sending values...");
#endif
  /*** 
   * Heart beat
   */
  sendHeartbeat();
}

void receive(const MyMessage &message)
{
#ifdef MY_DEBUG
    Serial.print("Incoming message from gateway: ");
    Serial.print(message.getCommand());
    Serial.print("/");
    Serial.println(message.getInt());
#endif
  if (message.isAck()) {
#ifdef MY_DEBUG
    Serial.println("This is an ack from gateway");
#endif
  } else {
  	if (message.getType()==V_STATUS) {
#ifdef MY_DEBUG
    Serial.print("Incoming change for sensor: ");
    Serial.print(message.sensor);
    Serial.print(" - New status: ");
    Serial.println(message.getBool() ? "true" : "false");
#endif
  		setState(message.getSensor(), message.getBool());
  	}
  }
}
