/Users/tzhong/Desktop/Screen\ Shot\ 2019-07-25\ at\ 7.46.36\ PM.png # Guiding Eyes for the Blind Project
Repository for the Guiding Eyes Project

This document describes how to set up the wearable navigation unit used in the Guiding Eyes Project, including the 

TODO: Write-up of how to install OS step-by-step

TODO: Write-up of how to run/usage side

### Materials Used:

Raspberry Pi Model 3B+
Intel RealSense D435 Camera
USB hub with at least two ports
Miuzei Raspberry Pi 3B+ Battery Pack Expansion (or similar, the lithium-ion battery is what's relevant)
Vibrating Mini Motor Disc
Electronics-Salon Prototype Breakout PCB Shield Board Kit for Raspberry Pi 3 (or similar)
DRV2605L Haptic Motor Controller (to drive the vibration motor)
microSD card (for the Raspberry Pi)

## Installation Instructions
The following instructions are used to install packages and libraries needed for this project. Steps 1-4 follow this [tutorial](https://github.com/IntelRealSense/librealsense/blob/master/doc/RaspberryPi3.md) with notes on things to avoid or be aware of
### 1. Install Ubuntu MATE on your Raspberry Pi 3

> **Note:** Ubuntu MATE on Raspberry Pi may present a `Kernel not started` error on startup. This can be ignored as it doesn't affect functionality<br/>

To begin with the download of Ubuntu Mate, navigate to the [Ubuntu Mate](https://ubuntu-mate.org/raspberry-pi/) website and find the download button. You will want the version for Raspberry Pi, and the latest version of the Bionic OS.

> **Note:** Ubuntu MATE will only support Bionic until April 2021. If downloading Ubuntu Mate after that date, a new version other than Bionic may be the only one supported. Any version should be fine. 

Once the file is downloaded, you may use Win32 Disk Imager (or a similar disk imager) to write the file to your microSD card.

> **Note:** The Intel RealSense SDK has not been tested with Raspbian, nor does it have documentation of how to use the camera with it.

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

