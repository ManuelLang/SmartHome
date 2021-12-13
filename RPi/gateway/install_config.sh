# Upgrade HA & config
# -------------------
sudo pip3 install --upgrade homeassistant
sudo systemctl restart home-assistant@homeassistant
sudo systemctl status home-assistant@homeassistant

https://home-assistant.io/docs/configuration/troubleshooting/

sudo hassbian-config install libcec
sudo hassbian-config install Open Z-Wave
https://home-assistant.io/blog/2017/04/30/hassbian-1.21-its-about-time/


# Install MQTT
# ------------
cd /home/homeassistant/.homeassistant
sudo hass --script check_config -c /home/homeassistant/.homeassistant
sudo hassbian-config install mosquitto
sudo mosquitto_passwd -b /etc/mosquitto/pwfile homeassistant ****
mosquitto_sub -h 127.0.0.1 -p 1883 -t "#"
mosquitto_pub -h 127.0.0.1 -p 1883 -u homeassistant -P **** -t "hello" -m world
sudo systemctl enable mosquitto
sudo systemctl start mosquitto

sudo nano /etc/mosquitto/pwfile
ADD:
allow_anonymous false

sudo reboot
sudo systemctl status mosquitto -l
mosquitto_pub -V mqttv311 -u homeassistant -P **** -t "homeassistant/binary_sensor/garden/config" -m '{"name": "garden", "device_class": "motion"}'
mosquitto_pub -h 127.0.0.1 -p 1883 -u homeassistant -P **** -t "homeassistant/binary_sensor/garden/state" -m ON

https://home-assistant.io/docs/mqtt/testing/

tail -f /home/homeassistant/.homeassistant/home-assistant.log


# Configure MySensors gateway
# ---------------------------


sudo ./configure --my-debug=enable --my-transport=nrf24 --my-rf24-channel=76 --my-rf24-pa-level=RF24_PA_MAX --my-rf24-ce-pin=22 --my-rf24-cs-pin=24 --my-rf24-irq-pin=15 --my-rx-message-buffer-size=30 --my-leds-err-pin=12 --my-leds-rx-pin=16 --my-leds-tx-pin=18 --my-gateway=mqtt --my-controller-ip-address=127.0.0.1 --my-port=1883 --my-mqtt-client-id=mygateway1 --my-mqtt-publish-topic-prefix=mysensors-out --my-mqtt-subscribe-topic-prefix=mysensors-in --my-mqtt-user=homeassistant --my-mqtt-password=****


/usr/lib/python3.4