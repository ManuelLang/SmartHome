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
 * REVISION HISTORY
 * Version 1.0 - Henrik Ekblad
 *
 * DESCRIPTION
 * Example sketch for a "relay switch" where you can control binary or something
 * else from both HA controller and a local physical button
 * (connected between digital pin 3 and GND).
 * This node also works as a repeader for other nodes
 * http://www.mysensors.org/build/relay
 */

// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_RF24

// For RF-NANO
#define MY_RF24_CE_PIN 10 
#define MY_RF24_CS_PIN 9

#define MY_NODE_ID 21
#define MY_TRANSPORT_WAIT_READY_MS 1000  // important pour fontionnement en local en cas de gateway hs
//uint8_t parent_id = 0;
//#define MY_PARENT_NODE_ID parent_id
//#define MY_PARENT_NODE_IS_STATIC

// Enabled repeater feature for this node
#define MY_REPEATER_FEATURE

#include <SPI.h>
#include <MySensors.h>
#include <Bounce2.h>

#define RELAY_PIN  4  // Arduino Digital I/O pin number for relay
#define RADIO_PIN_IRQ 2
#define BUTTON_PIN  3  // Arduino Digital I/O pin number for button
#define CHILD_ID_RELAY 1
#define CHILD_ID_PONG 2
#define RELAY_ON 0
#define RELAY_OFF 1

int my_id;

Bounce debouncer = Bounce();
static int buttonOldValue = -1;

int BATTERY_SENSE_PIN = A0;  // select the input pin for the battery sense point

unsigned long SLEEP_TIME = 900000;  // sleep time between reads (seconds * 1000 milliseconds)
int oldBatteryPcnt = 0;

static unsigned long last_interrupt_time = 0;
static bool initialized = false;
MyMessage msgRelayStatus(CHILD_ID_RELAY,V_STATUS);
MyMessage msgPong(CHILD_ID_PONG,V_TEXT);

void setup()
{
  my_id = getNodeId() ;             // mon node id
  
#ifdef MY_DEBUG
    Serial.print("Node_ID: ");
    Serial.println(my_id);
#endif
  
    // use the 1.1 V internal reference
#if defined(__AVR_ATmega2560__)
  analogReference(INTERNAL1V1);
#else
  analogReference(INTERNAL);
#endif

  // Setup the button
  pinMode(BUTTON_PIN,INPUT);
  // Activate internal pull-up
  digitalWrite(BUTTON_PIN,HIGH);

  // After setting up the button, setup debouncer
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(5);

  // Make sure relays are off when starting up
  digitalWrite(RELAY_PIN, RELAY_OFF);
  // Then set relay pins in output mode
  pinMode(RELAY_PIN, OUTPUT);

  // Set relay to last known state (using eeprom storage)
  setState(getState());

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), processInterrupt, FALLING);
}

void presentation()  {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Relay & Button", "1.0");

  // Register all sensors to gw (they will be created as child devices)
  present(CHILD_ID_RELAY, S_BINARY);
  present(CHILD_ID_PONG, S_INFO);

  // Send 'pong' message
  send(msgPong.set("pong"), true);
}

void processInterrupt() {
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if ((interrupt_time - last_interrupt_time) > 200)
  {
#ifdef MY_DEBUG
    Serial.print("=== Interrupt: ");
    Serial.println(millis());
#endif
    sendValues();
  }
  last_interrupt_time = interrupt_time;
}

bool getState() {
  return loadState(CHILD_ID_RELAY);
}

void setState(bool newState) {
  int oldState = getState();
  if (oldState != newState || !initialized) {
    initialized = true;
#ifdef MY_DEBUG
    Serial.print("New Relay state: ");
    Serial.println(newState);
#endif
    digitalWrite(RELAY_PIN, newState?RELAY_ON:RELAY_OFF);
    // Store state in eeprom
    saveState(CHILD_ID_RELAY, newState);
    send(msgRelayStatus.set(newState), true); // Send new state and request ack back
  }
}

/*
*  Example on how to asynchronously check for new messages from gw
*/

void loop()
{
  sendValues();
  sleep(SLEEP_TIME);
}

void sendValues() {
#ifdef MY_DEBUG
    Serial.println("Sending values...");
#endif
  /*** 
   * Heart beat
   */
  sendHeartbeat();
  
  /*** 
   * Get the battery Voltage
   */
  int sensorValue = analogRead(BATTERY_SENSE_PIN);
#ifdef MY_DEBUG
    Serial.print("Battery sensor value: ");
    Serial.println(sensorValue);
#endif

  // 1M, 470K divider across battery and using internal ADC ref of 1.1V
  // Sense point is bypassed with 0.1 uF cap to reduce noise at that point
  // ((1e6+470e3)/470e3)*1.1 = Vmax = 3.44 Volts
  // 3.44/1023 = Volts per bit = 0.003363075

  int batteryPcnt = sensorValue / 10;
  float batteryV  = sensorValue * 0.003363075;
#ifdef MY_DEBUG
    Serial.print("Battery voltage: ");
    Serial.print(batteryV);
    Serial.println("V");
    Serial.print("Battery percent: ");
    Serial.print(batteryPcnt);
    Serial.println("%");
#endif

  if (oldBatteryPcnt != batteryPcnt) {
    // Power up radio after sleep
    sendBatteryLevel(batteryPcnt);
    oldBatteryPcnt = batteryPcnt;
  }
  
  /*** 
   * Button
   */
  int value = digitalRead(BUTTON_PIN);
#ifdef MY_DEBUG
    Serial.print("Button value: ");
    Serial.println(value);
    Serial.print("Button old value: ");
    Serial.println(buttonOldValue);
#endif
  if (value != buttonOldValue) {
#ifdef MY_DEBUG
    Serial.println("Value changed!");
#endif
      if (value == LOW) {
#ifdef MY_DEBUG
    Serial.println("Setting new state...");
#endif
        setState(!getState());
      }
      buttonOldValue = value;
  }
}

void receive(const MyMessage &message) {
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
    if (message.type == V_STATUS) {
      // Write some debug info
#ifdef MY_DEBUG
    Serial.print("Incoming change for sensor: ");
    Serial.print(message.sensor);
    Serial.print(" - New status: ");
    Serial.println(message.getBool() ? "true" : "false");
#endif
      // Change relay state
      setState(message.getBool());
    }
    if (message.type == V_TEXT) {
#ifdef MY_DEBUG
    Serial.print("Incoming change for sensor: ");
    Serial.println(message.getString());
#endif
      if(strcmp(message.getString(),"ping") == 0){
#ifdef MY_DEBUG
    Serial.println("Reply: 'pong'");
#endif
        send(msgPong.set("pong"), true);
      }
    }
  }
  sendValues(); // sends heartbeat & battery level to the gateway
}
