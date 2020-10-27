# coding=utf-8

from mqtt.client import MySensorsMqttClient

client = MySensorsMqttClient(client_id='Consumer')
client.publish_message({
    "sensor_id": 10,
    "value": 22,
    "unit": "°C",
    "sensor_type": "TEMPERATURE"
})
