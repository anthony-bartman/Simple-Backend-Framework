#!/usr/bin/env python3

# import packages
import gpiozero
import time

#Hacking Rock
from gpiozero import LED
from gpiozero.pins.pigpio import PiGPIOFactory
from gpiozero.tools import all_values

led1 = gpiozero.PWMLED(6, active_high=False)
led2 = gpiozero.PWMLED(13, active_high=False)
led3 = gpiozero.PWMLED(12, active_high=False)
led4 = gpiozero.PWMLED(26, active_high=False)
led5 = gpiozero.PWMLED(20, active_high=False)
led6 = gpiozero.PWMLED(21, active_high=False)
button = gpiozero.Button(16)

# POWERING UP
def turn_on(repeat):
    for loop in range(0, repeat):
        for b in range(10):
            led1.value = b*0.1
            time.sleep(.025)
            while button.is_pressed:
                pass
        for b in range(10):
            led2.value = b*0.1
            time.sleep(.025)
            while button.is_pressed:
                pass
        for b in range(10):
            led3.value = b*0.1
            time.sleep(.025)
            while button.is_pressed:
                pass
        for b in range(10):
            led4.value = b*0.1
            time.sleep(.025)
            while button.is_pressed:
                pass
        for b in range(10):
            led5.value = b*0.1
            time.sleep(.025)
            while button.is_pressed:
                pass
        for b in range(10):
            led6.value = b*0.1
            time.sleep(.025)
            while button.is_pressed:
                pass
# POWERING BACK DOWN
        for b in range(10, 0, -1):
            led6.value = 0
            time.sleep(.005)
            while button.is_pressed:
                pass
        for b in range(10, 0, -1):
            led5.value = 0
            time.sleep(.005)
            while button.is_pressed:
                pass
        for b in range(10, 0, -1):
            led4.value = 0
            time.sleep(.005)
            while button.is_pressed:
                pass
        for b in range(10, 0, -1):
            led3.value = 0
            time.sleep(.005)
            while button.is_pressed:
                pass
        for b in range(10, 0, -1):
            led2.value = 0
            time.sleep(.005)
            while button.is_pressed:
                pass
        for b in range(10, 0, -1):
            led1.value = 0
            time.sleep(.005)
            while button.is_pressed:
                pass

def swap(repeat):
    for loop in range(0, repeat):
        # ON
        led1.value = 1
        led3.value = 1
        led5.value = 1
        # Off
        led2.value = 0
        led4.value = 0
        led6.value = 0
        time.sleep(.5)
        # OFF
        led2.value = 1
        led4.value = 1
        led6.value = 1
        # ON
        led1.value = 0
        led3.value = 0
        led5.value = 0
        time.sleep(.5)
    #Turn all Off when done    
    led1.value = 0
    led3.value = 0
    led5.value = 0
    led2.value = 0
    led4.value = 0
    led6.value = 0
def hack(repeat, hacked_ip):
    # pi = pigpio.pi(hacked_ip,8888)
    # pigpio.pi('soft', 8888)
    factory = PiGPIOFactory(host=hacked_ip)
    led7 = gpiozero.PWMLED(4, active_high=False,pin_factory=factory)
    
    for loop in range(0, repeat):
        while not button.is_pressed:
            led7.value = 1
            time.sleep(.5)
            led7.value = 0
            time.sleep(.5)