# Guiding Eyes for the Blind Project
Code repository for the Guiding Eyes Project

This document describes how to set up the wearable navigation unit used in the Guiding Eyes Project, including the 

### Materials Used:

* Raspberry Pi Model 3B+
* Intel RealSense D435 Camera
* USB hub with at least two ports
* Miuzei Raspberry Pi 3B+ Battery Pack Expansion (or similar, the lithium-ion battery is what's relevant)
* Vibrating Mini Motor Disc
* Electronics-Salon Prototype Breakout PCB Shield Board Kit for Raspberry Pi 3 (or similar)
* DRV2605L Haptic Motor Controller (to drive the vibration motor)
* microSD card (for the Raspberry Pi)

## Installation Instructions

The following instructions are used to install packages and libraries needed for this project. Steps 1-4 follow this [tutorial](https://github.com/IntelRealSense/librealsense/blob/master/doc/RaspberryPi3.md) with notes on things to avoid or be aware of

### 1. Install Ubuntu MATE on your Raspberry Pi 3

> **Note:** Ubuntu MATE on Raspberry Pi may present a `Kernel not started` error on startup. This can be ignored as it doesn't affect functionality<br/>

To begin with the download of Ubuntu Mate, navigate to the [Ubuntu Mate](https://ubuntu-mate.org/raspberry-pi/) website and find the download button. You will want the version for Raspberry Pi, and the latest version of the Bionic OS.

> **Note:** Ubuntu MATE will only support Bionic until April 2021. If downloading Ubuntu Mate after that date, a new version other than Bionic may be the only one supported. Any version should be fine. 

Once the file is downloaded, you may use Win32 Disk Imager (or a similar disk imager) to write the file to your microSD card.

> **Note:** The Intel RealSense SDK has not been tested with Raspbian, nor does it have documentation of how to use the camera with it. The SDK may have unexpected behavior with Raspbian if you choose to use that

Once the microSD card has Ubuntu Mate on it, insert it into the microSD slot on the Raspberry Pi and turn the Raspberry Pi on (this can be done by plugging in a microUSB cable). Connect the Raspberry Pi to an HDMI monitor and follow the steps on the screen to complete the system setup and configuration. This will involve creating settings on the device such as time zone and login credentials.

### 2. Clone and compile Intel Realsense SDK

> **Note:** Camera capabilities are severely reduced when connected to USB2 port due to lack of bandwidth. <br /> However, this should not be a problem with this project as firmware updates have added more functionality. 

> **Note:** Make sure the camera's firmware is up-to-date. This can be done using the [Device Firmware Update](https://www.intel.com/content/www/us/en/support/articles/000028171/emerging-technologies/intel-realsense-technology.html) tool. <br/>

> **Note:** In some cases the RAM capacity is not sufficient to compile the SDK, so if the compilation process crashes or exits with an error-code try to create a [swap file](https://www.howtoforge.com/ubuntu-swap-file) and then recompile the SDK. This should only be relevant when building the SDK. 

Next step is to clone and compile the latest [Intel RealSense SDK 2.0](https://github.com/IntelRealSense/librealsense/releases/tag/v2.23.0) by following the instructions in the Prerequisites section under [Linux Installation](https://github.com/IntelRealSense/librealsense/blob/development/doc/installation.md) until the step asking to build and apply patched kernel modules. Then skip ahead to the steps on Building librealsense2 SDK and do those steps as well.

### 3. Activate the OpenGL driver

We will need to activate the OpenGL driver because the librealsense SDK uses OpenGL to drive its visualizations of depth data from the camera. [Activating OpenGL](https://ubuntu-mate.community/t/tutorial-activate-opengl-driver-for-ubuntu-mate-16-04/7094/33) is fairly simple and can actually be done with the following steps: 

1. Boot up the Raspberry Pi and open up a terminal/command line to run the following command: `sudo raspi-config`
1. Enter into Advanced options -> Activate opengl driver
1. Exit that menu and run the following command from a terminal: `sudo reboot`

This should reboot the Raspberry Pi with the OpenGL driver activated.

### 4. Install wiringPi

We'll need to install [wiringPi](http://wiringpi.com/download-and-install/) in order to have access to commands that can control the vibration motor. Follow the steps in that link in order to download the wiringPi library.

To check to see if the installation of wiringPi worked, you can run the following:
* `gpio -v`
* `gpio readall`

If those commands run without throwing errors, then that should give confidence that wiringPi is working okay.

### 5. Install OpenCV 

Installing [OpenCV](https://docs.opencv.org/2.4/opencv_tutorials.pdf) may be useful for further visualizing the depth data, and can help with transformations of that visual data later on. In the link to the installation guide, navigate to the "Installation in Linux" section. Some required packages may need to be downloaded, and then the OpenCV code will need to be downloaded/cloned and then built using CMake.

### 6. Clone this repo and pull the latest changes

Now that all relevant libraries have been downloaded, the programs contained in this repo can now run correctly. Now all that's left is to clone the repo and build the project. [Cloning the repo](https://help.github.com/en/articles/cloning-a-repository) should only be one line of code. 

### 7. Build the project

Within the newly cloned guiding-eyes folder, we will need to make a "build" directory so that we can compile our project using CMake. To do this, within the librealsense folder within the guiding-eyes repository (which can be navigated to from the guiding-eyes home directory) you will want to run the following, one after the other:

* `mkdir build`
* `cd build`
* `cmake ../`
* `make`

If you want to create multiple builds, you can also run the same commands from the librealsense folder again, but with "build" replaced with another name for the folder.

## Usage

### 1. Detect objects within walking path

Describe how to run the associated programs to run this code
Once the project has been built, we can navigate within the build folder where there should be an executable file called "camera-motor". That executable can now be run using the command `./camera-motor`. This program is the main functionality of the device, detecting objects and activating the vibration motor if it senses an object.

### 2. Setting up Pi to run on startup

This step is not completely necessary, but if you'd like the device to [run the program on boot](https://www.dexterindustries.com/howto/run-a-program-on-your-raspberry-pi-at-startup/) then add the camera-motor program to be run on bootup of the Raspberry Pi.

