python3 ~/SmartHome/mqtt_exemple.py

sudo systemctl status mysgw.service -l

cat /home/homeassistant/.homeassistant/configuration.yaml
sudo systemctl status home-assistant@homeassistant.service -l


mosquitto_pub -h 127.0.0.1 -p 1883 -u homeassistant -P **** -t "mysensors-in/99/1/1/1/2" -m 1
mosquitto_pub -h 127.0.0.1 -p 1883 -u homeassistant -P **** -t "mysensors-in/99/1/1/1/2" -m 0

mosquitto_pub -h 127.0.0.1 -p 1883 -u homeassistant -P **** -t "office/sensor1" -m '{"temp":23,"hum":58}'