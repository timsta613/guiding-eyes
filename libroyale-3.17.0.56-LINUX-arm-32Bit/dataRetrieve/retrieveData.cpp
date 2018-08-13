/*Code that should take in the image streams from both cameras and run some image processing:
    1. determine a "hit box" to describe the expected path of the blind person
    2. determine whether there is an object that is close enough in that "hit box"
    3. send a command to a vibrating motor, if there is indeed an object
*/

#include <royale.hpp>
#include <iostream>
#include <mutex>
#include <thread>
#include <iomanip>
#include <chrono>
#include <algorithm>

#include <sample_utils/PlatformResources.hpp>

using namespace royale;
using namespace sample_utils;
using namespace std;


/**
 * A listener which receives a callback to onNewData when each depth frame is captured
 *
 */

class MyListener : public royale::IDepthDataListener
{
    static const size_t MAX_HEIGHT = 40;
    static const size_t MAX_WIDTH = 76; 

    /**
     * Data that has been received in onNewData, and will be printed in the paint() method
     */
    struct MyFrameData
    {

    };

public:
    /**
     * This callback is called for each depth frame that is captured. In a mixed-mode use case
     * (a use case with multiple streams), each callback refers to data from a single stream.
     */
    void onNewData (const royale::DepthData *data) override
    {
    
        //retrieves exposure times for the current stream
        auto exposureTimes = data->exposureTimes;
        
        // The data pointer will become invalid after onNewData returns. When processing the data,
        // it's necessary to either:
        // 1. Do all the processing before this method returns, or
        // 2. Copy the data (not just the pointer) for later processing, or
        // 3. Do processing that needs the full data here, and generate or copy only the data 
        //    required for later processing
        //
        // The Royale library's depth-processing thread may block while waiting for this function to 
        // return; if this function is slow then there may be some lag between capture and onNewData 
        // for the next frame. If it's very slow then Royale may drop frames to catch up.
        //
        // This sample code assumes that the UI toolkit will provide a callback to the paint()
        // method as needed, but does the initial processing of the data in the current thread.
        //
        // This sample code uses option 3 above, the conversion from DepthData to MyFrameData is 
        // done in this method, and MyFrameData provides all the data required for the paint()
        // method, without needing any pointers to the memory owned by the DepthData instance.
        std::size_t scale = std::max({size_t (1u), data->width / m_widthPerStream, data->height / MAX_HEIGHT});
        std::size_t height = data->height / scale;

        // To reduce the depth data to ascii art, this sample discards most of the information in
        // the data. However, even if we had a full GUI, treating the depth data as an array of 
        // (data->width * data->height) z coordinates would lose the accuracy. The 3D depth
        // points are not arranged in a rectilinear projection, and the discarded x and y 
        // coordinates form the depth points account for the optical projection (or optical
        // distortion) of the camera.  
        std::vector<std::string> asciiFrame;
        for (auto y = 0u; y < height; y++)
        {
            std::string asciiLine;
            asciiLine.reserve (m_widthPerStream);
            for (auto x = 0u; x < m_widthPerStream; x++)
            {
                // There is a bounds-check in the asciiPoint function, it returns a space character
                // if x or y is out-of-bounds
                asciiFrame.push_back (asciiPoint (data, x * scale, y * scale));
            }
            asciiFrame.push_back (std::move (asciiLine));
        } 

        // Scope for a lock while updating the internal model
        {
            std::unique_lock<std::mutex> lock (m_lockForReceivedData);
            auto
