// Test code to read in data from an IMU

#include <wiringPi.h>
#include <wiringSerial.h>

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main()
{
    int fd ; 
    if ((fd = serialOpen ("/dev/serial0", 115200)) < 0)
    {
        fprintf(stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
        return 1;
    } else
    {
        std::cout << "nice, serialOpen is a success" << std::endl;
    }

    // TODO: Initialize the BNO055 and stop if something went wrong
    // This is done in python with bno.begin() where bno is member of BNO055 class 


    // TODO: Print system status and self test result
    // This is done with bno.get_system_status()
    

    // TODO: Print BNO055 software revision and other diagnostic data
    // This is done with bno.get_revision()

    // TODO: Read the euler angles for heading, roll, pitch (all in degrees)
    // This is done with bno.read_euler()
    
    // TODO: Read the calibration status, 0=uncalibrated and 3=fully calibrated
    // bno.get_calibration_status()
  
    // TODO: Print everything out 

    // TODO: Determine whether the register addresses are needed
}

