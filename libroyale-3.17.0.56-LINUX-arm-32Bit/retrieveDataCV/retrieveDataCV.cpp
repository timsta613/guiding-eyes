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

#include <opencv2/opencv.hpp>

#include <sample_utils/PlatformResources.hpp>

using namespace royale;
using namespace sample_utils;
using namespace std;

class MyListener : public IDepthDataListener
{

public :

    MyListener() :
        undistortImage (false)
    {
    }

    void onNewData (const DepthData *data)
    {
        // this callback function will be called for every new 
        // depth frame

        std::lock_guard<std::mutex> lock (flagMutex);

        // create two images which will be filled afterwards
        // each image constraining one 32Bit channel
        zImage.create (Size (data->width, data->height), CV_32FC1);
        grayImage.create (Size (data->width, data->height), CV_32FC1);

        // set the image to zero
        zImage = Scalar::all (0);
        grayImage = Scalar::all (0);

        int k = 0;
        for (int y = 0; y < zImage.rows; y++)
        {
