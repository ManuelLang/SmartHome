# https://www.raspberrypi.org/documentation/usage/gpio/python/README.md

from gpiozero import LED, Button
from time import sleep

led = LED(17)
button = Button(2)

while True:
    button.when_pressed = led.on
    button.when_released = led.off

#    if button.is_pressed:
#        print("Pressed")
#        led.on()
#    else:
#        print("Released")
#        led.off()
#    sleep(1)
