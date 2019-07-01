/usr/lib/python3.4

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

mosquitto_pub -h 127.0.0.1 -p 1883 -u homeassistant -P ***** -t "office/sensor1" -m '{ "temperature": 23.20, "humidity": 43.70 }'