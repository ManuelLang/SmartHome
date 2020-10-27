#!/usr/bin/python
# coding=utf-8
import threading
import time

from config import *
import paho.mqtt.client as mqtt
import datetime
import logging
import json

logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(threadName)s - %(name)s - %(levelname)s: %(message)s')


class MySensorsMqttClient:

    def __init__(self, client_id, url=SERVER_URL, port=SERVER_PORT, username=USERNAME, password=PASSWORD, timeout_seconds=CONNECTION_TIMEOUT_SECONDS):
        if not client_id:
            raise ValueError("Parameter 'client_id' is missing")

        self.logger = logging.getLogger('mqtt_client')
        self.client_id = client_id
        self.url = url
        self.port = port
        self.username = username
        self.password = password
        self.topic = None
        self.timeout_seconds = timeout_seconds
        self.client = None

    # Define event callbacks
    def on_connect(self, client, obj, flags, rc):
        print("Connected with result code {} - client: {} - obj: {} - flags: {}".format(rc, client, obj,flags))

        # Subscribing in function 'on_connect()' means that subscriptions will be renewed if connection is lost
        # client.subscribe("$SYS/#")
        # self.logger.info('Subscribed to system topic')

        # Start subscribe, with QoS level 0
        client.subscribe(self.topic, 0)
        self.logger.info("Subscribed to topic '{}'".format(self.topic))

    def on_message(self, client, userdata, msg):
        value = msg.payload.decode("utf-8").upper() if msg.payload else ""
        self.logger.info("{} - Message received (client ID: {}): {}, {}"
                         .format(datetime.datetime.now().strftime(DATE_TIME_PATTERN), client._client_id, msg.topic, value))

    def on_publish(self, mosq, obj, mid):
        self.logger.info("Published message - ID: {}".format(mid))

    def on_subscribe(self, mosq, obj, mid, granted_qos):
        print("Subscribed to topic '{}'. Message ID: {}, qos: {}, obj: {}, mosq:{}"
              .format(self.topic, mid,granted_qos, obj, mosq))

    def on_log(self, mosq, obj, level, string):
        print(string)

    def connect(self, topic):
        self.client = mqtt.Client(client_id=self.client_id)

        # Assign event callbacks
        self.client.on_message = self.on_message
        self.client.on_connect = self.on_connect
        self.client.on_publish = self.on_publish
        self.client.on_subscribe = self.on_subscribe

        # Uncomment to enable debug messages
        self.client.on_log = self.on_log

        # Connect
        if self.username:
            self.client.username_pw_set(self.username, self.password)
        self.topic = topic
        self.client.connect(self.url, self.port, self.timeout_seconds)

    def listen(self, arg=None):
        if 'stop' in arg and arg['stop'] is True:
            return
        if not self.client:
            raise ValueError('Please connect prior to listen')
        print("Listening for messages in topic '{}'...".format(self.topic))
        # Loop waiting for messages
        # exit when an error occurs
        rc = 0
        while rc == 0:
            if 'stop' in arg and arg['stop'] is True:
                break
            rc = self.client.loop()
        print("Result code: {} - exit".format(rc))

    def publish_message(self, payload, qos=0, retain=False, properties=None):
        if not self.client:
            raise ValueError('Please connect prior to listen')
        str_msg = json.dumps(payload)
        (rc, mid) = self.client.publish(topic=self.topic, payload=str_msg, qos=qos, retain=retain, properties=properties)
        if rc != 0:
            raise ValueError('Error occurred, return code is: {}'.format(rc))
        self.logger.info('Successfully posted message: {} - Message ID: {}'.format(str_msg, mid))
        return mid


def main():
    topic = "/homeassistant/mysensors-out/122/1/1/0/0/"
    mqtt_client_subscriber = MySensorsMqttClient(client_id='subscriber')
    mqtt_client_subscriber.connect(topic)
    mqtt_client_publisher = MySensorsMqttClient(client_id='publisher')
    mqtt_client_publisher.connect(topic)

    info = {'stop': False}
    thread_listen = threading.Thread(target=mqtt_client_subscriber.listen, args=(info,))
    thread_listen.start()

    message = {
        "sensor_id": 10,
        "value": 22,
        "unit": "°C",
        "sensor_type": "TEMPERATURE"
    }

    logging.debug('Press any key to exit...')
    while True:
        try:
            mqtt_client_publisher.logger.info('Posting message')
            message_id = mqtt_client_publisher.publish_message(message)
            mqtt_client_publisher.logger.info('Posted message {}'.format(message_id))
            time.sleep(2)
        except KeyboardInterrupt:
            info['stop'] = True
            break
    thread_listen.join()


if __name__ == '__main__':
    main()
