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

    //TODO: Determine what is needed here--just some GUI object initializations from rs-align
    //window app(1280, 720, "CPP - Align Example"); // Simple window handling
    //ImGui_ImplGlfw_Init(app, false); // ImGui library initialization
    //rs2::colorizer c; // Helper to colorize depth images
    //texture renderer; // Helper for rendering images

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    // Start streaming with default recommended configuration
    rs2::pipeline_profile profile = pipe.start();
    float depth_scale = get_depth_scale(profile.get_device());
 
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
    
    int frame_count = 0;
    float app_filler = 1.f;
    while(app_filler) // Application still alive?
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

        bool b = false;

        /*
        for (int x = top_left_x; x < bot_right_x; x++)
        {
            for (int y = top_left_y; y < bot_right_y; y++)
            {
                //std::cout << "Looking at the following points" << std::endl;
                //std::cout << x << std::endl;
                //std::cout << y << std::endl;
                dist = depth.get_distance(x, y) * depth_scale;
                if (dist < depth_clipping_distance)
                {
                    //std::cout << "The distance that is within clipping distance is" << std::endl;
                    //std::cout << dist << std::endl;
                    b = true;
                    //break;
                }
                if (dist < min_dist)
                {
                    min_dist = dist;
                }
                if (dist > max_dist)
                {
                    max_dist = dist;
                }
            }
        }
        */

        // If there are pixels below a certain threshold, make motor vibrate with inverse relation to that distance
        int power = 0;
        // If the boolean is true, then activate the vibration motor
        if (b_1 > 0.0) 
        {
            // Set power to be a value from 50-100 depending on the distance away
            // TODO: Set the frequency of the vibration to be different as well
            // TODO: Extract this to a helper method
            bool vib_flag = true;
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
           
            // Set power of vibration to be a function of distance of closest object
            if (vib_flag) {
                power = static_cast<int>(30+70.0*(depth_clipping_distance-b_1)/depth_clipping_distance);
            }
            std::cout <<"vib_flag: " << vib_flag << ", power: " <<  power << ", dist: " << b_1 << ", frame: " << frame_count << std::endl;
            softPwmWrite(13, power);
        } else
        {
            softPwmWrite(13, power);
            std::cout << "Not vibrating, no objects detected! " <<  "frame: " << frame_count << std::endl;
        }
        
        // Update frame count (used for frequency of vibrations)
        if (frame_count > 89)
        {
            frame_count = 0;
        } else
        {
            frame_count = frame_count + 1;
        }
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
