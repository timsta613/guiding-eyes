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


rs2_stream find_stream_to_align(const std::vector<rs2::stream_profile>& streams);
float get_depth_scale(rs2::device dev);
float object_within_depth(const rs2::depth_frame& depth_frame, int x_bot, int x_top, int y_bot, int y_top, float depth_scale, float clipping_dist);
int get_vibration_power(const float& distance_of_object, const float& clipping_dist, const int& frame_count);
void remove_background(rs2::video_frame& other, const rs2::depth_frame& depth_frame, float depth_scale, float clipping_dist, int y_bot, int y_top, int x_bot, int x_top);


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
    rs2::frame other_frame;
    rs2::frame aligned_depth_frame;
    //rs2::frame d;

    // Declare depth colorizer for pretty visualization of depth data
    rs2::colorizer color_map;
    
    // TODO: add in lines before while loop
    // Created a shared pointer to a pipeline
    auto pipe = std::make_shared<rs2::pipeline>();    

    // Start streaming with default configuration
    pipe->start();

    // Initialize a shared pointer to a device with the current device on the pipeline 
    rs2::device device = pipe->get_active_profile().get_device();
    float depth_scale = get_depth_scale(device);

    // Create a variable to control the seek bar
    int seek_pos;

    /*pipe -> stop(); // Stop stream with default configuration
    pipe = std::make_shared<rs2::pipeline>();
    rs2::config cfg;
    cfg.enable_device_from_file("a.bag");
    pipe->start(cfg); // File will be opened in read mode at this point
 
    device = pipe->get_active_profile().get_device();

            
    // Pipeline could choose a device that does not have a color stream
    // If there is no color stream, choose to align depth to another stream
    rs2_stream align_to = find_stream_to_align(pipe->get_active_profile().get_streams());
            
    // rs2::align allows us to perform alignment of depth frames to others frames
    // The "align_to" is the stream type to which we plan to align depth frames
    rs2::align align(align_to);
    
    device.as<rs2::playback>().resume();*/
    

    if (!device.as<rs2::playback>())
    {
        pipe -> stop(); // Stop stream with default configuration
        pipe = std::make_shared<rs2::pipeline>();
        rs2::config cfg;
        cfg.enable_device_from_file("a.bag");
        pipe->start(cfg); // File will be opened in read mode at this point
 
        device = pipe->get_active_profile().get_device();
        depth_scale = get_depth_scale(device);    
        // Pipeline could choose a device that does not have a color stream
        // If there is no color stream, choose to align depth to another stream
        //rs2_stream align_to = find_stream_to_align(pipe->get_active_profile().get_streams());
            
        // rs2::align allows us to perform alignment of depth frames to others frames
        // The "align_to" is the stream type to which we plan to align depth frames
        //rs2::align align(align_to);
    } else {
        device.as<rs2::playback>().resume();
    }

    // Pipeline could choose a device that does not have a color stream
    // If there is no color stream, choose to align depth to another stream
    rs2_stream align_to = find_stream_to_align(pipe->get_active_profile().get_streams());
            
    // rs2::align allows us to perform alignment of depth frames to others frames
    // The "align_to" is the stream type to which we plan to align depth frames
    rs2::align align(align_to);

    // Define a variable for controlling the distance to clip
    float depth_clipping_distance = 2.f;

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
            depth_scale = get_depth_scale(device);    

            // Pipeline could choose a device that does not have a color stream
            // If there is no color stream, choose to align depth to another stream
            rs2_stream align_to = find_stream_to_align(pipe->get_active_profile().get_streams());
            
            // rs2::align allows us to perform alignment of depth frames to others frames
            // The "align_to" is the stream type to which we plan to align depth frames
            rs2::align align(align_to);
        } else {
            device.as<rs2::playback>().resume();
        }
        //ImGui::PopStyleColor(4);
        //ImGui::PopStyleVar();
        /*if(device.as<rs2::playback>())
        {
            device.as<rs2::playback>().resume();
        }*/

        rs2::playback playback = device.as<rs2::playback>();
        if (pipe->poll_for_frames(&frames))
        {
            depth = color_map.process(frames.get_depth_frame()); // Find and colorize the depth data for rendering
            rs2::depth_frame d_1 = frames.get_depth_frame();
            //TODO: Grab the color frame here, display it
            // Get processed aligned frame
            auto processed = align.process(frames);

            // Trying to get both other and aligned depth frames
            other_frame = processed.first(align_to);
            aligned_depth_frame = processed.get_depth_frame();

            //TODO: Run some processing on the frames here
            int h = d_1.get_height();
            int w = d_1.get_width(); 
            int y_bot = static_cast<int>(h/3.0);
            int y_top = static_cast<int>(2.0*h/3.0);
            int x_bot = static_cast<int>(w/3.0);
            int x_top = static_cast<int>(2.0*w/3.0);
            float b_1 = object_within_depth(d_1, x_bot, x_top, y_bot, y_top, depth_scale, depth_clipping_distance);             

            int power = get_vibration_power(b_1, depth_clipping_distance, 0);

             
            if (power > 0) {
                std::cout <<"vib_flag: True, power: " <<  power << ", dist: " << b_1 << std::endl;
                //softPwmWrite(13, power);
            } else
            {
                //softPwmWrite(13, power);
                std::cout << "Not vibrating, no objects detected! Distance: " << b_1 << std::endl;
            }
        }
        
        //ImGui::End();
        //ImGui::Render();

        // Render depth frames from the playback
        depth_image.render(other_frame, {0, 0, 1*app.width() / 2, 1*app.height() / 2}) ;
        depth_image.render(depth, {1*app.width()/2, 1*app.height()/2, 1*app.width()/2, 1*app.height()/2});
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

rs2_stream find_stream_to_align(const std::vector<rs2::stream_profile>& streams)
{
    // Given a vector of streams, we try to find a depth stream and another stream to align depth with
    // We prioritize color streams to make the view look better
    // If color is not available, we take another stream (other than depth) 
    rs2_stream align_to = RS2_STREAM_ANY;
    bool depth_stream_found = false;
    bool color_stream_found = false;
    for (rs2::stream_profile sp : streams)
    {
        rs2_stream profile_stream = sp.stream_type();
        if (profile_stream != RS2_STREAM_DEPTH)
        {
            if (!color_stream_found)  // Prefer color
            {
                align_to = profile_stream;
            }
            if (profile_stream == RS2_STREAM_COLOR)
            {
                color_stream_found = true;
            }
        } else {
            depth_stream_found = true;
        }
    }

    if(!depth_stream_found) {
        throw std::runtime_error("No Depth stream available");
    }

    if (align_to == RS2_STREAM_ANY) {
        throw std::runtime_error("No stream found to align with Depth");
    }

    return align_to;
}

float get_depth_scale(rs2::device dev) 
{
    // Go over the device's sensors
    for (rs2::sensor& sensor : dev.query_sensors())
    {
        // Check if the sensor is a depth sensor
        if (rs2::depth_sensor dpt = sensor.as<rs2::depth_sensor>())
        {
            return dpt.get_depth_scale();
        }
    }
    throw std::runtime_error("Device does not have a depth sensor");
}

float object_within_depth(const rs2::depth_frame& depth_frame, int x_bot, int x_top, int y_bot, int y_top, float depth_scale, float clipping_dist)
{
    const uint16_t* p_depth_frame = reinterpret_cast<const uint16_t*>(depth_frame.get_data());

    int width = depth_frame.get_width();
    int height = depth_frame.get_height();
    
    int total_pixels = (x_top-x_bot) * (y_top-y_bot);
    int pixel_count_threshold = static_cast<int>(total_pixels/4.0);
    int b_count = 0;
    float min_dist = clipping_dist;
    #pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelize the loop
    for (int y = y_bot; y < y_top; y++)
    {
        auto depth_pixel_index = y*width;
        for (int x = x_bot; x<x_top; x++, ++depth_pixel_index)
        {
            // Get the depth value of the current pixel
            auto pixels_distance = depth_scale * p_depth_frame[depth_pixel_index];

            // Check if the depth value is less than the clipping threshold
            if ((pixels_distance < clipping_dist) && (pixels_distance > 0.f))
            {
                b_count = b_count+1;
                if (pixels_distance < min_dist)
                {
                    min_dist = pixels_distance;
                }
            }
        }
    }
    if (b_count > pixel_count_threshold)
    {
        return min_dist;
    }
    return 0.0;
}


int get_vibration_power(const float& distance_of_object, const float& clipping_dist, const int& frame_count)  {
    //Method that retrieves the power of vibration
    // Argument distance_of_object will be >0 if there's an object within threshold
    // we have defined above. Otherwise it will be 0.0.
    int power = 0;
    bool vib_flag = false;
    // If the boolean is true, then activate the vibration motor
    if (distance_of_object > 0.0) 
    {
        // TODO: Set the frequency of the vibration to be different as well
        vib_flag = true;
        /*
        if (( b_1 > three_depth_clipping_distance) && (frame_count%8 == 0))
        {
            vib_flag = true;
        } else if ((b_1 > two_depth_clipping_distance) && (frame_count%4 == 0)) 
        {
            vib_flag = true;
        } else if ((b_1 > one_depth_clipping_distance) && (frame_count%2 == 0))
        {
            vib_flag = true;
        } else if (b_1 <= one_depth_clipping_distance)
        {
            vib_flag = true;
        }
        */ 
    }
    //std::cout << "distance_of_object: " << distance_of_object << " vib_flag: " << vib_flag;
    // Set power to be a value from 50-100 as a function of
    //  distance of closest object
    // This function can be changed to whatever else is deemed to fit
    if (vib_flag) {
        power = static_cast<int>(30+70.0*(clipping_dist-distance_of_object)/clipping_dist);
    }
    return power;
}

void remove_background(rs2::video_frame& other_frame, const rs2::depth_frame& depth_frame, float depth_scale, float clipping_dist, int y_bot, int y_top, int x_bot, int x_top) 
{
    const uint16_t* p_depth_frame = reinterpret_cast<const uint16_t*>(depth_frame.get_data());
    uint8_t* p_other_frame = reinterpret_cast<uint8_t*>(const_cast<void*>(other_frame.get_data()));

    int width = other_frame.get_width();
    int height = other_frame.get_height();
    int other_bpp = other_frame.get_bytes_per_pixel();

    #pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelise the loop
    for (int y = 0; y < height; y++) 
    {
        auto depth_pixel_index = y * width;
        for (int x = 0; x < width; x++, ++depth_pixel_index)
        {
            // Get the depth value of the current pixel
            auto pixels_distance = depth_scale * p_depth_frame[depth_pixel_index];

            // Check if the depth value is invalid (<=0) or greater than the threshold 
            if (x <= x_bot || x >= x_top || y <= y_bot || y >= y_top || pixels_distance <= 0.f || pixels_distance > clipping_dist)
            {
                // Calculate the offset in other frame's buffer to current pixel
                auto offset = depth_pixel_index * other_bpp;
                
                // Set pixel to "background" color (0x999999)
                std::memset(&p_other_frame[offset], 0x99, other_bpp);
            }
        }
    }
}
