// Method for controlling motor for guiding eyes project

// Before anything else make sure wiring pi is downloaded

// This method will need to do the following:
// 1. Take in the depth data in whatever format it comes in
// 2. Narrows the view to just the relevant pixels
// 3. Check if there is an object within those pixels that is within a threshold
// 4. Activate motor as an inverse relation to the distance that the object is away from us
#include <wiringPi.h>
#include <softPwm.h>
#include <iostream>
int main (void)
{
    wiringPiSetupGpio () ;
    softPwmCreate (13, 0, 100) ;
    for (;;) 
    {
        softPwmWrite (13, 100) ;
        std::cout << "On" << std::endl;
        delay (4000) ;
        softPwmWrite (13, 0) ;
        std::cout << "Off" << std::endl;
        delay (4000) ;
    }
    return 0;
}
