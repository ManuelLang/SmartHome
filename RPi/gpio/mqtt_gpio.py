from gpiozero import LED, Button
from time import sleep

mqtt_gpio_write_mapping = {
    "homeassistant/bedroom/light1/set": LED(17)
}
mqtt_gpio_read_mapping = {
    "homeassistant/bedroom/switch1/set": Button(2)
}

class MqttGpioMapper():

    gpio_state = {}

    def __init__(self, subscribers):
        """
        subscribers: list of observers. Each observer must have a function 'notify(mqtt_topic, new_val)'
        """
        # init the states: https://gpiozero.readthedocs.io/en/stable/api_input.html
        for button in mqtt_gpio_read_mapping.values():
            gpio_state[button.pin] = button.value
        for led in mqtt_gpio_write_mapping.values():
            gpio_state[led.pin] = led.value
        self.subscribers = subscribers
        x = threading.Thread(target=self.__read_gpios)
        x.start()

    def on_message(self, mqtt_topic, payload):
        if mqtt_topic in mqtt_gpio_write_mapping:
            led = mqtt_gpio_write_mapping[mqtt_topic]
            payload = str(payload).upper()
            led.on() if (payload == "ON" or payload == "1") else led.off()

    def __raise_event(self, mqtt_topic, new_val):
        for subscriber in subscribers:
            subscriber.notify(mqtt_topic, new_val)

    def __read_gpios(self):
        for mqtt_topic, button in mqtt_gpio_read_mapping.items():
            previous_val = gpio_state[button.pin]
            new_val = button.value
            if previous_val != new_val:
                self.__raise_event(mqtt_topic, new_val)
        sleep(0.1)
