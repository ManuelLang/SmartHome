# Hass MQTT config

## Config file
```bash
cat /home/homeassistant/.homeassistant/configuration.yaml
```

```yaml
sensor:
  - platform: mqtt
    state_topic: 'office/sensor1'
    name: 'Temperature'
    unit_of_measurment: 'C'
    value_template: '{{ value_json.temperature }}'
  - platform: mqtt
    state_topic: 'office/sensor1'
    name: 'Humidity'
    unit_of_measurment: '%'
    value_template: '{{ value_json.humidity }}'
```

## Testing
```bash
mosquitto_pub -h 127.0.0.1 -p 1883 -u "sa-ha" -P "dummy" -t "office/sensor1" -m '{ "temperature": 23.20, "humidity": 43.70 }'
mosquitto_pub -h 127.0.0.1 -p 1883 -u "sa-ha" -P "dummy" -t "homeassistant/bedroom/switch1/availability" -m "Online"
mosquitto_pub -h 127.0.0.1 -p 1883 -u "sa-ha" -P "dummy" -t "homeassistant/bedroom/switch1/set" -m "ON"
mosquitto_pub -h 127.0.0.1 -p 1883 -u "sa-ha" -P "dummy" -t "homeassistant/bedroom/light1/set" -m "ON"
```
