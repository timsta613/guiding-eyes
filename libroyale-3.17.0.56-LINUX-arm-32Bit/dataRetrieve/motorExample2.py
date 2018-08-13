import RPi.GPIO as GPIO
from time import sleep

GPIO.setmode(GPIO.BOARD)

Vib_pin = 13

GPIO.setup(Vib_pin, GPIO.OUT)

print("Turning motor on")
GPIO.output(Vib_pin, GPIO.HIGH)

sleep(2)

print("Stopping motor")

GPIO.output(Vib_pin, GPIO.LOW)

GPIO.cleanup()
