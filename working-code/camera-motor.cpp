// Main driver for camera to motor control

// This method will need to do the following:
// 1. Take in the depth data in whatever format it comes in
// 2. Narrows the view to just the relevant pixels
// 3. Check if there is an object within those pixels that is within a threshold
// 4. Activate motor as an inverse relation to the distance that the object is away from us

#include <wiringPi.h>
#include <softPwm.h>
#include <iostream>
#include <librealsense2/rs.hpp>   
#include <signal.h>
#include <stdio.h>
#include <cstdlib>

#include <algorithm>

#include <opencv2/opencv.hpp>

#include "../example.hpp"
#include <imgui.h>
#include "imgui_impl_glfw.h"

using namespace cv;

float get_depth_scale(rs2::device dev);
float object_within_depth(const rs2::depth_frame& depth_frame, int x_bot, int x_top, int y_bot, int y_top, float depth_scale, float clipping_dist);
bool profile_changed(const std::vector<rs2::stream_profile>& current, const std::vector<rs2::stream_profile>& prev);
rs2_stream find_stream_to_align(const std::vector<rs2::stream_profile>& streams);
int get_vibration_power(const float& distance_of_object, const float& clipping_dist, const int& frame_count);
void remove_background(rs2::video_frame& other, const rs2::depth_frame& depth_frame, float depth_scale, float clipping_dist, int y_bot, int y_top, int x_bot, int x_top);

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

    //TODO: Determine what is needed here--just some GUI object initializations from rs-align
    //window app(1280, 720, "CPP - Align Example"); // Simple window handling
    //ImGui_ImplGlfw_Init(app, false); // ImGui library initialization
    //rs2::colorizer c; // Helper to colorize depth images
    //texture renderer; // Helper for rendering images
    window app(1280, 720, "Guiding Eyes Code Test"); // Simple window handling
    ImGui_ImplGlfw_Init(app, false); // ImGui library initialization
    rs2::colorizer c;  // Helper to colorize depth images
    texture renderer;  // Helper for rendering images

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    // Start streaming with default recommended configuration
    rs2::pipeline_profile profile = pipe.start();
    float depth_scale = get_depth_scale(profile.get_device());

    // TODO: Figure out if this line is needed or not
    // Pipeline could choose a device that does not have a color stream
    // If there is no color stream, choose to align depth to another stream
    rs2_stream align_to = find_stream_to_align(profile.get_streams());

    // TODO: Figure out if this line is needed or not
    // rs2::align allows us to perform alignment of depth frames to others frames
    // The "align_to" is the stream type to which we plan to align depth frames
    rs2::align align(align_to);
    
 
    // Initialize wiringPi setup
    wiringPiSetupGpio();
    // Initialize softPwm on GPIO pin 13 with a range of 0 (off) to 100 (fully on) 
    softPwmCreate(13, 0, 100);
    
    // Define a variable for controlling the distance to clip
    // TODO: Rename these to something else
    float depth_clipping_distance = 2.f;
    float three_depth_clipping_distance = 0.75 * depth_clipping_distance;
    float two_depth_clipping_distance = 0.50 * depth_clipping_distance;
    float one_depth_clipping_distance = 0.25 * depth_clipping_distance;
    
    int frame_count = 0; // Variable used to keep track of what number frame 
    while(app) // Application still alive?
    {
        rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
        rs2::depth_frame depth = data.get_depth_frame();
        
        //Check to see if the device has been changed
        if(profile_changed(pipe.get_active_profile().get_streams(), profile.get_streams()))
        {
            //If the profile was changed, get the new device and depth_scale
            profile = pipe.get_active_profile();
            depth_scale = get_depth_scale(profile.get_device());
        }

        // Get processed aligned frame
        auto processed = align.process(data);
        
        // Trying to get both other and aligned depth frames
        rs2::video_frame other_frame = processed.first(align_to);
        rs2::depth_frame aligned_depth_frame = processed.get_depth_frame();

        // TODO: Determine whether this is needed or not, currently conflicts with 
        // frame count iteration
        // If one of them is unavailable, continue iteration
        /*if (!aligned_depth_frame || !other_frame)
        {
            continue;
        }*/


        // Narrowing of the frame: pick a top left corner and bottom right corner to look at
        // Will come into play later when we implement geometry algorithm better
        int top_left_x = 0;
        int top_left_y = 0;
        int bot_right_x = 100;
        int bot_right_y = 100;

        // Check to see if there are pixels below a certain threshold, store in a boolean
        // Get the width and height of the depth frame
        // Narrow the frame based on some proportion of the frame
        int h = depth.get_height();
        int w = depth.get_width();
        int y_bot = static_cast<int>(h/3.0);
        int y_top = static_cast<int>(2.0*h/3.0);
        int x_bot = static_cast<int>(w/3.0);
        int x_top = static_cast<int>(2.0*w/3.0);
        float b_1 = object_within_depth(depth, x_bot, x_top, y_bot, y_top, depth_scale, depth_clipping_distance);

        // If there are pixels below a certain threshold, make motor vibrate with inverse relation to that distance
        //int power = 0; 
        // Power is an int between 50-100 that is calculated as an inverse relation to that distance
        int power = get_vibration_power(b_1, depth_clipping_distance, frame_count);

        if (power > 0) {
            std::cout <<"vib_flag: True, power: " <<  power << ", dist: " << b_1 << ", frame: " << frame_count << std::endl;
            softPwmWrite(13, power);
        } else
        {
            softPwmWrite(13, power);
            std::cout << "Not vibrating, no objects detected! Distance: " << b_1 << " frame: " << frame_count << std::endl;
        }
        
        // Update frame count (used for frequency of vibrations)
        if (frame_count > 79)
        {
            frame_count = 0;
        } else
        {
            frame_count = frame_count + 1;
        }
       
        // Passing both frames to remove_background so it will "strip" the background 
        // NOTE: in this example, we alter the buffer of the other frame, instead of copying it and altering the copy
        // This behavior is not recommended in real application since the other frame could be used elsewhere
        // TODO: implement a comparable "remove background" method
        remove_background(other_frame, aligned_depth_frame, depth_scale, depth_clipping_distance, y_bot, y_top, x_bot, x_top);

        //Taking dimensions of the window for rendering purposes
        float w2 = static_cast<float>(app.width());
        float h2 = static_cast<float>(app.height());

        // At this point, "other_frame" may be altered by the "remove_background" section mentioned above
        // Calculating the position to place the frame in the window
        rect altered_other_frame_rect{0, 0, w2, h2};
        altered_other_frame_rect.adjust_ratio({ static_cast<float>(other_frame.get_width()), static_cast<float>(other_frame.get_height()) });
        
        //Render aligned image 
        renderer.render(other_frame, altered_other_frame_rect);
        
        //The example also renders the depth frame, as a picture-in-picture
        //Calculating the position to place the depth frame in the window
        rect pip_stream{0, 0, w2/5, h2/5};
        pip_stream = pip_stream.adjust_ratio({ static_cast<float>(aligned_depth_frame.get_width()), static_cast<float>(aligned_depth_frame.get_height()) });
        pip_stream.x = altered_other_frame_rect.x + altered_other_frame_rect.w - pip_stream.w - (std::max(w2, h2) / 25);
        pip_stream.y = altered_other_frame_rect.y + altered_other_frame_rect.h - pip_stream.h - (std::max(w2, h2) / 25);

        // Render depth (as picture in picture)
        renderer.upload(c.process(aligned_depth_frame)); 
        renderer.show(pip_stream);
        
    }
    return EXIT_SUCCESS;
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

bool profile_changed(const std::vector<rs2::stream_profile>& current, const std::vector<rs2::stream_profile>& prev)
{
    for (auto&& sp : prev)
    {
        //If previous profile is in current (maybe just added another)
        auto itr = std::find_if(std::begin(current), std::end(current), [&sp](const rs2::stream_profile& current_sp) { return sp.unique_id() == current_sp.unique_id(); });
        if (itr == std::end(current)) // If previous stream wasn't found in current stream
        {
            return true;
        }
    }
    return false;
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
