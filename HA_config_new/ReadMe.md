cat /home/homeassistant/.homeassistant/configuration.yaml

 mosquitto_pub -h 127.0.0.1 -u "sa-ha" -P "****" -t "homeassistant/bedroom/switch1/availability" -m "Online"
 mosquitto_pub -h 127.0.0.1 -u "sa-ha" -P "****" -t "homeassistant/bedroom/switch1/set" -m "ON"
 mosquitto_pub -h 127.0.0.1 -u "sa-ha" -P "****" -t "homeassistant/bedroom/light1/set" -m "ON"
