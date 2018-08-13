/****************************************************************************\
 * Copyright (C) 2017 Infineon Technologies & pmdtechnologies ag
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 \****************************************************************************/

#include <royale.hpp>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

#include <sample_utils/PlatformResources.hpp>

using namespace sample_utils;
using namespace std;

namespace
{
    class MasterListener : public royale::IDepthDataListener
    {
        void onNewData (const royale::DepthData *data)
        {
            cout << "Received data for the master ..." << endl;

            // Do something with the data
        }
    };

    class SlaveListener : public royale::IDepthDataListener
    {
        void onNewData (const royale::DepthData *data)
        {
            cout << "Received data for the slave ..." << endl;

            // Do something with the data
        }
    };

    /**
     * Convenience method to list the connected cameras' IDs. For this sample, it's assumed that two
     * cameras of the same type are connected, therefore adding code to retrieve the camera names
     * would not provide useful information.
     */
    void printCamerasToStderr (const royale::Vector<royale::String> &camlist)
    {
        for (const auto &id : camlist)
        {
            cerr << "    " << id << endl;
        }
    }
}

/**
 * This example shows how to handle master/slave connections.  It tries to open two cameras and
 * define one as master and the other as slave. For this you should use two modules of the same
 * kind.
 *
 * This takes one optional argument, the identifier for the master device. If this is not given, the
 * sample chooses one in a deterministic way (given a pair of cameras, it will always choose the
 * same one as the master).
 */
int main (int argc, char **argv)
{
    // Windows requires that the application allocate these, not the DLL.
    PlatformResources resources;

    // These are the data listeners which will receive callbacks.
    SlaveListener slaveListener;
    MasterListener masterListener;

    // these represent the main camera device objects
    unique_ptr<royale::ICameraDevice> slaveCameraDevice;
    unique_ptr<royale::ICameraDevice> masterCameraDevice;

    // if non-empty, choose this camera as the master
    royale::String masterId;

    // if true, just lists the cameras available
    bool justListCameras = false;

    if (argc > 1)
    {
        auto arg = royale::String (argv[1]);
        if (arg == "--help" || arg == "-h" || arg == "/?" || argc > 2)
        {
            cout << argv[0] << ":" << endl;
            cout << "--help, -h, /? : this help" << endl;
            cout << "--list         : prints the list of cameras" << endl;
            cout << endl;
            cout << "With one command line argument: chooses the camera with that ID as the master." << endl;
            return 0;
        }
        else if (arg == "--list")
        {
            justListCameras = true;
        }
        else
        {
            masterId = std::move (arg);
        }
    }


    // the camera manager will query for a connected camera
    {
        royale::CameraManager manager;

        auto camlist = manager.getConnectedCameraList();
        cout << "Detected " << camlist.size() << " camera(s)." << endl;
        if (justListCameras)
        {
            printCamerasToStderr (camlist);
            return 0;
        }

        if (camlist.size() < 2)
        {
            cerr << "The master/slave example needs at least 2 cameras!" << endl;
            return 1;
        }

        if (masterId.empty())
        {
            masterId = *std::min_element (camlist.begin(), camlist.end());
        }
        else if (std::find (camlist.begin(), camlist.end(), masterId) == camlist.end())
        {
            cerr << "No camera has id \"" << masterId << "\". Available cameras are:" << endl;
            printCamerasToStderr (camlist);
            return 1;
        }

        royale::String slaveId;
        if (masterId == camlist.at (0))
        {
            slaveId = camlist.at (1);
        }
        else
        {
            slaveId = camlist.at (0);
        }

        cout << "CamID for master : " << masterId << endl;
        masterCameraDevice = manager.createCamera (masterId, royale::TriggerMode::MASTER);

        cout << "CamID for slave : " << slaveId << endl;
        slaveCameraDevice = manager.createCamera (slaveId, royale::TriggerMode::SLAVE);
    }

    // the camera devices should now be available and the CameraManager can be deallocated here

    if (masterCameraDevice == nullptr ||
            slaveCameraDevice == nullptr)
    {
        cerr << "Cannot create the camera devices" << endl;
        return 1;
    }

    // IMPORTANT: call the initialize method of the master before initializing the slave!
    if (masterCameraDevice->initialize() != royale::CameraStatus::SUCCESS)
    {
        cerr << "Cannot initialize the master" << endl;
        return 1;
    }
    if (slaveCameraDevice->initialize() != royale::CameraStatus::SUCCESS)
    {
        cerr << "Cannot initialize the slave" << endl;
        return 1;
    }

    // register the data listeners
    if (masterCameraDevice->registerDataListener (&masterListener) != royale::CameraStatus::SUCCESS)
    {
        cerr << "Error registering data listener for the master" << endl;
        return 1;
    }
    if (slaveCameraDevice->registerDataListener (&slaveListener) != royale::CameraStatus::SUCCESS)
    {
        cerr << "Error registering data listener for the slave" << endl;
        return 1;
    }

    // retrieve available use cases
    royale::Vector<royale::String> useCasesMaster;
    if (masterCameraDevice->getUseCases (useCasesMaster) != royale::CameraStatus::SUCCESS)
    {
        cerr << "Error retrieving use cases for the master" << endl;
        return 1;
    }

    royale::Vector<royale::String> useCasesSlave;
    if (slaveCameraDevice->getUseCases (useCasesSlave) != royale::CameraStatus::SUCCESS)
    {
        cerr << "Error retrieving use cases for the slave" << endl;
        return 1;
    }

    // choose a use case without mixed mode
    auto selectedUseCaseIdx = 0u;
    auto useCaseFound = false;
    for (auto i = 0u; i < useCasesMaster.size(); ++i)
    {
        uint32_t streamCount = 0;
        if (masterCameraDevice->getNumberOfStreams (useCasesMaster[i], streamCount) != royale::CameraStatus::SUCCESS)
        {
            cerr << "Error retrieving the number of streams for use case " << useCasesMaster[i] << endl;
            return 1;
        }

        if (streamCount == 1)
        {
            // we found a use case with only one stream
            selectedUseCaseIdx = i;
            useCaseFound = true;
            break;
        }
    }

    // check if we found a suitable use case
    if (!useCaseFound)
    {
        cerr << "Error : There are only mixed modes available" << endl;
        return 1;
    }

    // We expect both devices to be the same kind, and therefore have the same list of
    // available use cases. Use the name of the master's use case for the slave, if it isn't
    // an available use case on the slave then something is wrong.
    //
    // The use case name includes the exposure time, so this prevents having a master
    // driving a slave faster than the slave's use case is expected to run.
    const auto masterUseCase = useCasesMaster.at (selectedUseCaseIdx);

    // set an appropriate use case (in this case we take the first one)
    if (masterCameraDevice->setUseCase (masterUseCase) != royale::CameraStatus::SUCCESS)
    {
        cerr << "Error setting use case '" << masterUseCase << "' for the master" << endl;
        return 1;
    }
    if (slaveCameraDevice->setUseCase (masterUseCase) != royale::CameraStatus::SUCCESS)
    {
        cerr << "Error setting use case '" << masterUseCase << "' for the slave" << endl;
        return 1;
    }

    // IMPORTANT: at this point it is important to first start capturing for all the slave cameras,
    // as they will be triggered by the master!
    if (slaveCameraDevice->startCapture() != royale::CameraStatus::SUCCESS)
    {
        cerr << "Error starting the capturing for the slave" << endl;
        return 1;
    }
    if (masterCameraDevice->startCapture() != royale::CameraStatus::SUCCESS)
    {
        cerr << "Error starting the capturing for the master" << endl;
        return 1;
    }


    // let the cameras capture for some time
    this_thread::sleep_for (chrono::seconds (5));


    // IMPORTANT: stop capturing for the master before stopping the capturing for the slaves, otherwise
    // the slave might be triggered after the slaveCameraDevice is already stopped!
    if (masterCameraDevice->stopCapture() != royale::CameraStatus::SUCCESS)
    {
        cerr << "Error stopping the capturing for the master" << endl;
        return 1;
    }
    if (slaveCameraDevice->stopCapture() != royale::CameraStatus::SUCCESS)
    {
        cerr << "Error stopping the capturing for the slave" << endl;
        return 1;
    }

    return 0;
}
