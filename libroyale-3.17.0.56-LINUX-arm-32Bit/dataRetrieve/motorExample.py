import RPi.GPIO as GPIO
from time import sleep

servo_pin = 13
GPIO.setmode(GPIO.BCM)
GPIO.setup(servo_pin, GPIO.OUT)

p = GPIO.PWM(servo_pin, 50)
p.start(2.5)

try:
    while 1:
        p.ChangeDutyCycle(2.5)
        sleep(1)
        p.ChangeDutyCycle(7.5)
        sleep(1)
        p.ChangeDutyCycle(12.5)
        sleep(1)

except KeyboardInterrupt:
    pass

GPIO.cleanup()
