# Guiding Eyes for the Blind Project
Repository for the Guiding Eyes Project

Welcome to the Guiding Eyes Project

This document describes how to set up the wearable navigation unit used in the Guiding Eyes Project

TODO: Write-up of how to install OS step-by-step

TODO: Write-up of how to run/usage side

### Materials Used:

TODO: List materials used here 

## Installation Instructions
The following instructions are used to install packages and libraries needed for this project. Steps 1-4 follow this [tutorial](https://github.com/IntelRealSense/librealsense/blob/master/doc/RaspberryPi3.md) with notes on things to avoid or be aware of
### 1. Install Ubuntu MATE on your Raspberry Pi 3

> **Note:** Ubuntu MATE on Raspberry Pi may present a `Kernel not started` error on startup. This can be ignored as it doesn't affect functionality<br/>

Download the file from the Ubuntu Mate website
For Windows, use Win32 Disk Imager or a similar disk imager to write the file to your sd card
Boot the pi up connected to a hdmi monitor and complete setup/system configuration


### 2. Clone and compile Intel Realsense SDK
> **Note:** Camera capabilities are severely reduced when connected to USB2 port due to lack of bandwidth. <br /> However, this should not be a problem with this project as firmware updates have added more functionality. 

> **Note:** Make sure the camera's firmware is up-to-date. This can be done using the [Device Firmware Update](https://www.intel.com/content/www/us/en/support/articles/000028171/emerging-technologies/intel-realsense-technology.html) tool. <br/>



### 3. Activate the OpenGL driver
Activate [OpenGL](https://ubuntu-mate.community/t/tutorial-activate-opengl-driver-for-ubuntu-mate-16-04/7094/33)

`sudo raspi-config` 
Then in Advanced options -> Activate opengl driver
`sudo reboot`
### 4. Install wiringPi
Install [wiringPi](http://wiringpi.com/download-and-install/) 
### 5. Install OpenCV 
Install [OpenCV](https://docs.opencv.org/2.4/opencv_tutorials.pdf)
### 6. Setting up Pi to run on startup
[Run on boot](https://www.dexterindustries.com/howto/run-a-program-on-your-raspberry-pi-at-startup/)

### 7. Clone this repo and pull the latest changes

git clone command

### 8. Build the project

make a build directory
enter the build directory
cmake ../
make

## Usage

### 1. Detect objects within walking path

Describe how to run the associated programs to run this code

