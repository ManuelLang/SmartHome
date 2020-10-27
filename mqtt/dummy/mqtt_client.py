#!/usr/bin/python

import paho.mqtt.client as mqtt
import datetime
#from gpiozero import LED, Button

class MySensorsMqttClient:
    """
    usage:
    python3 mqtt_exemple.py
    """

    def __init__(self, url='127.0.0.1', port=1883, username=None, password=None):
        self.url = url
        self.port = port
        self.username = username
        self.password = password
        self.topic = '#'
        # self.relay = LED(2)
        # self.relay_state = False
        # self.relay.off() if self.relay_state else self.relay.on()
        # self.button_state = False
        # self.motion = Button(3)
        # self.button = Button(4)

    # Define event callbacks
    def on_connect(self, client, obj, flags, rc):
        print("Connected with result code {} - client: {} - obj: {} - flags: {}".format(rc, client, obj,flags))

        # Subscribing in on_connect() means that if we lose the connection and
        # reconnect then subscriptions will be renewed.
        client.subscribe("$SYS/#")
        print('subscribed to system topic')

        # Start subscribe, with QoS level 0
        client.subscribe(self.topic, 0)
        print("subscribed to topic '{}'".format(self.topic))

    def on_message(self, client, userdata, msg):
        value = msg.payload.decode("utf-8").upper() if msg.payload else ""
        print("{} {} {}".format(datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S"), msg.topic, value))
        if msg.topic == "homeassistant/bedroom/light1":
            self.relay_state = True if value == "ON" else False
            print("Relay state: " + value)
            #self.relay.off() if self.relay_state else self.relay.on()

    def handle_button(self):
        self.button_state = not self.button_state
        self.client.publish("homeassistant/bedroom/light1", "ON" if self.button_state else "OFF")

    def handle_motion_on(self):
        if not self.relay_state:
            self.client.publish("homeassistant/bedroom/light1", "ON")

    def handle_motion_off(self):
        if self.relay_state:
            self.client.publish("homeassistant/bedroom/light1", "OFF")

    def on_publish(self, mosq, obj, mid):
        print("mid: " + str(mid))

    def on_subscribe(self, mosq, obj, mid, granted_qos):
        print("Subscribed: " + str(mid) + " " + str(granted_qos) + " " + str(obj) + " " + str(mosq))

    def on_log(self, mosq, obj, level, string):
        print(string)

    def listen(self, topic='#'):
        self.client = mqtt.Client()

        self.topic = topic

        # Assign event callbacks
        self.client.on_message = self.on_message
        self.client.on_connect = self.on_connect
        self.client.on_publish = self.on_publish
        self.client.on_subscribe = self.on_subscribe

        # Uncomment to enable debug messages
        client.on_log = self.on_log

        # self.motion.when_pressed = self.handle_motion_on
        # self.motion.when_released = self.handle_motion_off
        # self.button.when_pressed = self.handle_button

        # Connect
        if self.username:
            self.client.username_pw_set(self.username, self.password)
        self.client.connect(self.url, self.port, 60)

        # Continue the network loop, exit when an error occurs
        rc = 0
        while rc == 0:
            rc = self.client.loop()
        print("rc: " + str(rc))


client = MySensorsMqttClient(url='127.0.0.1', port=1883, username='sa-ha', password='dummy')
client.listen()
print('Listening for messages...')
