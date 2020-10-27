from mqtt.client import MySensorsMqttClient

client = MySensorsMqttClient(client_id='Consumer')
client.listen()
