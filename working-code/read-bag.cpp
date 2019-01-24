// Code that simply reads a bag named 'a.bag' and repeats it

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

int main(int argc, char * argv[]) try
{
    // Create a simple OpenGL window for rendering:
    window app(1280, 720, "Guiding Eyes Read Bag");
    ImGui_ImplGlfw_Init(app, false);
    
    // Declare a texture for the depth image on the GPU
    texture depth_image;

    // Declare frameset and frames which will hold the data from the camera
    rs2::frameset frames;
    rs2::frame depth;

    // Declare depth colorizer for pretty visualization of depth data
    rs2::colorizer color_map;
    
    // TODO: add in lines before while loop
    // Created a shared pointer to a pipeline
    auto pipe = std::make_shared<rs2::pipeline>();    

    // Start streaming with default configuration
    pipe->start();

    // Initialize a shared pointer to a device with the current device on the pipeline 
    rs2::device device = pipe->get_active_profile().get_device();

    // Create a variable to control the seek bar
    int seek_pos;

    // While application is running
    while(app) {
        ImGui_ImplGlfw_NewFrame(1);
        ImGui::SetNextWindowSize({ app.width(), app.height() });

        if (!device.as<rs2::playback>())
        {
            pipe -> stop(); // Stop stream with default configuration
            pipe = std::make_shared<rs2::pipeline>();
            rs2::config cfg;
            cfg.enable_device_from_file("a.bag");
            pipe->start(cfg); // File will be opened in read mode at this point
 
            device = pipe->get_active_profile().get_device();
        } else {
            device.as<rs2::playback>().resume();
        }
        //ImGui::PopStyleColor(4);
        //ImGui::PopStyleVar();

        rs2::playback playback = device.as<rs2::playback>();
        if (pipe->poll_for_frames(&frames))
        {
            depth = color_map.process(frames.get_depth_frame()); // Find and colorize the depth data for rendering
        }
        
        //ImGui::End();
        //ImGui::Render();

        // Render depth frames from the playback
        depth_image.render(depth, {app.width() / 4, 0, 3*app.width() / 5, 3 * app.height() / 5 + 50 }) ;
    }
    return EXIT_SUCCESS;
} 
catch (const rs2::error & e)
{
    std::cout << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}

