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
#include <unistd.h>
#include <cmath>

#include <map>
#include <queue>
#include <list>

#include <algorithm>

#include <opencv2/opencv.hpp>

using namespace cv;

float get_depth_scale(rs2::device dev);
float object_within_depth(const rs2::depth_frame& depth_frame, int x_bot, int x_top, int y_bot, int y_top, float depth_scale, float clipping_dist);
float cluster_within_depth(const rs2::depth_frame& depth_frame, int x_bot, int x_top, int y_bot, int y_top, float depth_scale, float clipping_dist);
std::list<std::pair<int, int>> find_neighbors(const int& x, const int& y);
std::list<std::pair<int, int>> find_neighbors_in_frame(const int& x, const int& y, const int& x_bot, const int& x_top, const int& y_bot, const int& y_top);
int get_vibration_power(const float& distance_of_object, const float& clipping_dist, const int& frame_count);
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

    //window app(1280, 720, "CPP - Align Example"); // Simple window handling
    //ImGui_ImplGlfw_Init(app, false); // ImGui library initialization
    //rs2::colorizer c; // Helper to colorize depth images
    //texture renderer; // Helper for rendering images

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    // Start streaming with default recommended configuration
    rs2::pipeline_profile profile = pipe.start();
    float depth_scale = get_depth_scale(profile.get_device());
 
    // Define decimation filter (reduces depth frame density)
    rs2::decimation_filter dec;

    // Define spatial filter (edge-preserving)
    rs2::spatial_filter spat;

    // Configure filter parameters
    dec.set_option(RS2_OPTION_FILTER_MAGNITUDE, 3);
    
    // Enable hole-filling
    // Hole filling is an aggressive heuristic and it gets the depth wrong many times
    // (the shortest-path will always prefer to "cut" through the holes since they have 0 3D dist

    spat.set_option(RS2_OPTION_HOLES_FILL, 2); // 5 = fill all the zero pixels
    // Initialize wiringPi setup
    wiringPiSetupGpio();
    // Initialize softPwm on GPIO pin 13 with a range of 0 (off) to 100 (fully on) 
    softPwmCreate(13, 0, 100);
    
    // Define a variable for controlling the distance to clip
    float depth_clipping_distance = 2.5f;
    float three_depth_clipping_distance = 0.75 * depth_clipping_distance;
    float two_depth_clipping_distance = 0.50 * depth_clipping_distance;
    float one_depth_clipping_distance = 0.25 * depth_clipping_distance;

    //Code vibrations to indicate the start of program
    softPwmWrite(13, 100);
    usleep(500000);
    softPwmWrite(13, 0);
    usleep(500000);

    softPwmWrite(13, 100);
    usleep(500000);
    softPwmWrite(13, 0);
    usleep(500000);
    
    int frame_count = 0;
    float app_filler = 1.f;
    while(app_filler) // Application still alive?
    {
        rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
        rs2::depth_frame depth = data.get_depth_frame();
        
        data = data.apply_filter(dec);
        data = data.apply_filter(spat);

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
        int y_bot = static_cast<int>(4.5*h/9.0); // left
        int y_top = static_cast<int>(8.0*h/9.0); // right
        int x_bot = static_cast<int>(4.5*w/9.0); // down
        int x_top = static_cast<int>(9.0*w/9.0); // up
        float b_1 = object_within_depth(depth, x_bot, x_top, y_bot, y_top, depth_scale, depth_clipping_distance);
        float b_2 = cluster_within_depth(depth, x_bot, x_top, y_bot, y_top, depth_scale, depth_clipping_distance); 
        
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
        if (b_2 > 0.0) 
        {
            // Set power to be a value from 50-100 depending on the distance away
            //  Set the frequency of the vibration to be different as well
            //  Extract this to a helper method
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
                //power = static_cast<int>(30+70.0*(depth_clipping_distance-b_1)/depth_clipping_distance);
                power = 100;
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
    int pixel_count_threshold = static_cast<int>(total_pixels/8.0);
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

typedef std::pair<int, int> pair;

float cluster_within_depth(const rs2::depth_frame& depth_frame, int x_bot, int x_top, int y_bot, int y_top, float depth_scale, float clipping_dist) { 
    const uint16_t* p_depth_frame = reinterpret_cast<const uint16_t*>(depth_frame.get_data());

    int width = depth_frame.get_width();
    int height = depth_frame.get_height();
    
    //std::cout << "depth_frame width for cluster finding: " << width << std::endl;
    //std::cout << "depth_frame height for cluster finding: " << height << std::endl;

    int total_pixels = (x_top-x_bot) * (y_top-y_bot);
    int pixel_count_threshold = static_cast<int>(total_pixels/4.0);

    int b_count = 0;
    float min_dist = clipping_dist;
    
    //collect all pixels within a clipping distance by their indices
    // put em in a map maybe? something with constant time access
    // then as we iterate through the collection of pixels within 
    // a certain distance, we start each iteration assuming the single 
    // pixel we are looking at is within a "cluster" then we look at each
    // of that pixel's neighbors to see if any are also within the distance
    // and so on. The iteration continues if we find all the pixels in that 
    // "cluster" and it doesn't exceed the threshold
    // if it does exceed threshold, then keep track of min distance of pixel
    // Determine whether we want the algorithm to go through all clusters

    #pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelize the loop
    //  Add all pixels within clipping distance to a map

    // Map for y_bot => left
    //  y_top => right
    //  x_bot => down
    //  x_top => up

    // y is height
    // x is width

    std::map<pair, int> coordmap;
    for (int y = y_bot; y < y_top; y++)
    {
        auto depth_pixel_index = y*width;
        for (int x = x_bot; x<x_top; x++)
        {
            // Get the depth value of the current pixel
            auto pixels_distance = depth_scale * p_depth_frame[(depth_pixel_index+x)];

            
            // Check if the depth value is less than the clipping threshold
            if ((pixels_distance < clipping_dist) && (pixels_distance > 0.f))
            {
                // Hypoteneuse * sine((y-(height/2.0))/(height/2.0)) 
                float right_offset_estimate = pixels_distance * sin((y-(height/2.0))/(height/2.0));
                if (right_offset_estimate < 0.85) 
                {
                    // Add all pixels here to a map
                    b_count = b_count+1;
                    if (pixels_distance < min_dist)
                    {
                        min_dist = pixels_distance;
                    }
                    coordmap.insert(std::make_pair(std::make_pair(x, y),1));
                    if (b_count > 12000) {
                        return 1.0;
                    }
                }
            }
        }
    }
    
    // Loop through them in a BFS way to find clusters, while tracking average distance
    // For this, keep a map of visited coordinates
    // While there are coordinates in the map of pixels within a certain distance
    // while 
    int max_clust = 0;
    //std::cout << "max possible coordmap.size() is " << (y_top-y_bot)*(x_top-x_bot) << std::endl;
    std::cout << "coordmap.size() is " << coordmap.size() << std::endl;

    float upper_clust_size_thresh = 0.1f;
    int pixels_in_frame = (y_top-y_bot)*(x_top-x_bot); //About 100000 currently 
    //std::cout << "here's threshold: " << upper_clust_size_thresh * pixels_in_frame << std::endl;
    int clust_threshold = 250;
    std::map<pair, int> visited;

    //std::cout << "here's x_bot, x_top in cluster within "  << x_bot << ", " << x_top << std::endl;
    //std::cout << "here's y_bot, y_top in cluster within "  << y_bot << ", " << y_top << std::endl;

    for ( const auto& myPair : coordmap) {
        if (visited.find(myPair.first) == visited.end() ) {
            // not found, check neighbors and add to queue, keeping track of a list of these coords. compare size of list to max_clust at end   
            auto key = myPair.first;
            // do bfs here on neighbors 
              
            // initialize queue here
            std::queue<std::pair<int, int>> q;
            // while queue not empty 
            q.push(key);
            int clust_size = 0;
            while(q.size() > 0) {
                // find way to obtain the neighbors
                std::list<std::pair<int, int>> neighbors = find_neighbors_in_frame(q.front().first, q.front().second, x_bot, x_top, y_bot, y_top);  
                // check if neighbors are in coordmap keys
                for (const auto& neighCoords : neighbors) {
                    
                    // if so, add them to queue, increment cluster size by 1, add to visited
                    if((coordmap.find(neighCoords) != coordmap.end()) && (visited.find(neighCoords) == visited.end())) {
                        q.push(neighCoords);
                        clust_size++;
                        // Should replace clust_threshold if a new thresh is needed
                        if (clust_size > clust_threshold) {
                            //std::cout << clust_size << " is greater than " << upper_clust_size_thresh*pixels_in_frame << " with boolean " << (clust_size > upper_clust_size_thresh*pixels_in_frame) <<std::endl;
                            std::cout << "max_clust is reached " << clust_size << std::endl;
                            return 1.0;
                        }
                        visited.insert(std::make_pair(neighCoords, 1));
                    }
                }
                // pop off queue
                q.pop();
            }
            // compare cluster size to max_clust
            if (clust_size > max_clust) {
                max_clust = clust_size; 
            }
            // update visited
            visited.insert(std::make_pair(key, 1));
        } else {
            // found, continue
            continue;
        }
    }     
 
    // Check if the largest cluster is above a certain pixel threshold (for now, just print the number of pixels in the largest cluster)
 
    std::cout << "max_clust is " << max_clust << std::endl;
    //  If it is, return the average distance of pixels in that cluster
    if (max_clust > clust_threshold)
    {
        return 1.0;
    }
    return 0.0;
}

std::list<std::pair<int, int>> find_neighbors(const int& x, const int& y) {
    // Helper method for BFS to find neighbors
    std::list<std::pair<int, int>> l;
    for (int i = x-1; i <= x+1; i++) {
        for (int j = y-1; j <= y+1; j++) {
            if ((i == x) && (j==y)) {
                continue;
            } else {
                l.push_back(std::make_pair(i, j));
            }
        }
    }
    return l;
}

std::list<std::pair<int, int>> find_neighbors_in_frame(const int& x, const int& y, const int& x_bot, const int& x_top, const int& y_bot, const int& y_top) {
    // Helper method for BFS to find neighbors within a box
    std::list<std::pair<int, int>> l;
    for (int i = x-1; i <= x+1; i++) {
        for (int j = y-1; j <= y+1; j++) {
            if (((i == x) && (j==y)) || (i < x_bot) || (i > x_top) || (j < y_bot) || (j > y_top)) {
                continue;
            } else {
                l.push_back(std::make_pair(i, j));
            }
        }
    }
    return l;
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
        // Set the frequency of the vibration to be different as well
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
        //power = static_cast<int>(30+70.0*(clipping_dist-distance_of_object)/clipping_dist);
        power = 100;
    }
    return power;
}
