// Code that helps with recording datasets
// Make it so that the code vibrates/waits for like ten seconds before recording
// See if I can get it so that recording happens without the IMGUI code so that I can record at a higher fps
// Steps:
// 1. Get it working with code from record-playback
// 2. Get it working without showing the ImGui stuff
// 3. See if I can record a better dataset


#include <wiringPi.h>
#include <softPwm.h>
#include <iostream>
#include <librealsense2/rs.hpp>   
#include <signal.h>
#include <stdio.h>
#include <cstdlib>

#include <algorithm>

#include <opencv2/opencv.hpp>

#include "example.hpp"
#include <chrono>
#include <imgui.h>
#include "imgui_impl_glfw.h"

using namespace cv;

float get_depth_scale(rs2::device dev);
float object_within_depth(const rs2::depth_frame& depth_frame, int x_bot, int x_top, int y_bot, int y_top, float depth_scale, float clipping_dist);
bool profile_changed(const std::vector<rs2::stream_profile>& current, const std::vector<rs2::stream_profile>& prev);


void my_handler(int s) {
    printf("Caught signal %d\n", s);
    softPwmWrite(13, 0);
    delay(100);
    exit(1);
}

int main(int argc, char * argv[]) try
{
    rs2::log_to_console(RS2_LOG_SEVERITY_ERROR);
    
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);


    // Initialize wiringPi setup
    wiringPiSetupGpio();
    // Initialize softPwm on GPIO pin 13 with a range of 0 (off) to 100 (fully on) 
    softPwmCreate(13, 0, 100);
    
    // Create a simple OpenGL window for rendering:
    window app(1280, 720, "Guiding Eyes Create Bag");
    ImGui_ImplGlfw_Init(app, false);
    
    // Declare a texture for the depth image on the GPU
    tecture depth_image; 

    // Declare frameset and frames which will hold the data from the camera
    rs2::frameset frames;
    rs2::frame depth; 

    // Declare depth colorizer for pretty visualization of depth data
    rs2::colorizer color_map;
   
    // Created a shared pointer to a pipeline
    auto pipe = std::make_shared<rs2::pipeline>();

    // Start streaming with default configuration
    pipe->start(); 

    // Initialize a shared pointer to a device with the current device  on the pipeline
    rs2::device device = pipe->get_active_profile().get_device();

    // TODO: Figure out how to code the different buttons

    // While application is running
    while(app) {
        ImGui_ImplGlfw_NewFrame(1);
        ImGui::SetNextWindowSize({app.width(), app.height() });
        ImGui::Begin("app", nullptr, flags);

        if (!device.as<rs2::playback>()) {
            ImGui::Text("Click 'record' to start recording");
            if (ImGui::Button("record", {50, 50})) {
                // If it is the start of a new recording (device is not a recorder yet)
                if (!device.as<rs2::recorder>()) {
                    pipe->stop(); // Stop the pipeline with the default configuration
                    pipe = std::make_shared<rs2::pipeline>();
                    rs2::config cfg; // Declare a new configuration
                    cfg.enable_record_to_file("a.bag");
                    pipe->start(cfg); //File will be opened at this point
                    device = pipe->get_active_profile().get_device();
                }
            }                    
 
            if (device.as<rs2::recorder>()) {
                // TODO: write this portion to stop recording/program that button
            }
        }

        // TODO: Render depth frames from the playback
        depth_image.render(depth, {app.width() / 4, 0, 3 * app.width() / 5, 3 * app.height() / 5 + 50 });
    }
}
catch (const rs2::error & e)
{
    std::cerr << "RealSense error callin " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}
