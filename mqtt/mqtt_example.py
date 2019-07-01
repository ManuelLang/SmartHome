import paho.mqtt.client as mqtt
import datetime

class MySensorsMqttClient():
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

    # Define event callbacks
    def on_connect(self, client, obj, flags, rc):
        print("Connected with result code {} - client: {} - obj: {} - flags: {}".format(rc, client, obj,flags))

        # Subscribing in on_connect() means that if we lose the connection and
        # reconnect then subscriptions will be renewed.
        # client.subscribe("$SYS/#")

        # Start subscribe, with QoS level 0
        client.subscribe(self.topic, 0)

    def on_message(self, client, userdata, msg):
        if msg.payload:
            print("{} {} {} ".format(datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S"), msg.topic, msg.payload.decode("utf-8")))

    def on_publish(self, mosq, obj, mid):
        print("mid: " + str(mid))

    def on_subscribe(self, mosq, obj, mid, granted_qos):
        print("Subscribed: " + str(mid) + " " + str(granted_qos) + " " + str(obj) + " " + str(mosq))

    def on_log(self, mosq, obj, level, string):
        print(string)

    def listen(self, topic='#'):
        client = mqtt.Client()

        self.topic = topic

        # Assign event callbacks
        client.on_message = self.on_message
        client.on_connect = self.on_connect
        client.on_publish = self.on_publish
        client.on_subscribe = self.on_subscribe

        # Uncomment to enable debug messages
        #client.on_log = on_log

        # Connect
        if self.username:
            client.username_pw_set(self.username, self.password)
        client.connect(self.url, self.port, 60)

        # Continue the network loop, exit when an error occurs
        rc = 0
        while rc == 0:
            rc = client.loop()
        print("rc: " + str(rc))

client = MySensorsMqttClient(url='127.0.0.1', port=1883, username='homeassistant', password='****')
client.listen()
