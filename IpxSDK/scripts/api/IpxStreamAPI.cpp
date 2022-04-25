/**
 * @file IpxStreamAPI.cpp
 * @author Anthony Kung (hi@anth.dev)
 * @name IpxStreamAPI
 * @brief IpxStreamAPI is a wrapper for the Ipx SDK which provides a simple interface to perform regular camera operations.
 * @version 0.0.1
 * @date 2022-02-26
 * 
 * Modified From Imperx SDK Samples
 * 
 * @copyright Copyright (c) 2022 Anthony Kung <hi@anth.dev>
 * @license Apache-2.0
 * 
 */

#include <vector>
#include <string>
#include <atomic>
#include <thread>
#include <limits>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cinttypes>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "IpxCameraApi.h"

// Function Prototypes
IpxCam::Interface *SelectInterface( IpxCam::System *system );
IpxCam::DeviceInfo *SelectDevice( IpxCam::Interface *iface, int deviceIndex );
void GetSetParams( IpxCam::Device *device );
std::string AcquireImages( IpxCam::Device *device, IpxCam::Stream *stream );
void FreeDataStreamBuffers( IpxCam::Stream *stream, std::vector<IpxCam::Buffer *> *buffers );
const char* GetAccessStatusStr( int32_t status );
std::vector<std::string> split(const std::string& s, char delimiter);
bool SetIpAddress( IpxCam::DeviceInfo* iface );
uint32_t ValidateIpAddress( const std::string &ipAddress );
int GetConnectedDevices(IpxCam::Interface *iface);

// sync values
std::atomic_bool g_isStop(false);
std::string g_result = "";

#define SHM_KEY_GIGE 0x4443
#define STR_SIZE 1000
#define DEVICES 5

struct shm_gige {
    int busy;
    int request;
    int start;
    int started;
    int exit;
    int num_devices;
    char identifier[DEVICES][STR_SIZE];
    char path[STR_SIZE];
};

struct shm_gige *shmmsg_gige;

// Main function
int main(int argc, char *argv[]) {

    //****************************************

    // Interprocess Communication

    // ANTH = 2684
    int shmid_gige = -1;
    while (shmid_gige < 0) {
        shmid_gige = shmget(SHM_KEY_GIGE, sizeof(struct shm_gige), IPC_CREAT | 0644);
        if (shmid_gige < 0) {
            std::cout << "IPC Init Failed\n" << std::endl;
            return shmid_gige;
        }
    }
    
    
    shmmsg_gige = (struct shm_gige *) shmat(shmid_gige, NULL, 0);
    if (shmmsg_gige == (void *) -1) {
        std::cout << "IPC Attach Failed\n" << std::endl;
        return -1;
    }

    shmmsg_gige->busy = 0;
    shmmsg_gige->exit = 0;
    shmmsg_gige->start = 0;
    shmmsg_gige->started == 0;

  //****************************************
    
    // while (shmmsg_gige->exit == 0) {
        // std::cout << "Waiting for start signal" << std::endl;
        // If request check request
        // if (shmmsg_gige->request) {
            shmmsg_gige->request = 0;
            // Get System
            auto system = IpxCam::IpxCam_GetSystem();
            if (system) {
                while (true) {
                    shmmsg_gige->started = 1;
                    // std::cout << "Started!" << std::endl;
                    // Select desired interface
                    auto iface = SelectInterface(system);
                    if (iface) {
                        // Show Number of Connected Devices
                        auto numDevices = GetConnectedDevices(iface);

                        for (int i = 0; i < numDevices; i++) {
                            // Select desired device
                            auto deviceInfo = SelectDevice(iface, i);
                            if (deviceInfo) {
                                auto deviceName = deviceInfo->GetDisplayName();
                                std::cout << "\nConnecting to " << deviceName << " ....\n";

                                // check if Ip Address can be adjusted
                                if (deviceInfo->GetAccessStatus() == IpxCam::DeviceInfo::IpSubnetMismatch) {
                                    std::cout << "Cannot connect due to Ip Subnet Mismatch error\n";
                                    SetIpAddress(deviceInfo);
                                }

                                if (deviceInfo->GetAccessStatus() == IpxCam::DeviceInfo::AccessStatusReadWrite) {
                                    auto device = IpxCam_CreateDevice(deviceInfo);
                                    if (device) {
                                        std::cout << "Connecting to " << deviceName << " DONE\n\n";
                                        std::cout << std::endl;

                                        IpxCam::Stream *stream = nullptr;
                                        if (device->GetNumStreams() >= 1 && (stream = device->GetStreamByIndex(0))) {
                                            std::vector<IpxCam::Buffer*> bufferList;
                                            auto bufSize = stream->GetBufferSize();
                                            auto minNumBuffers = stream->GetMinNumBuffers();
                                            for (size_t i = 0; i < minNumBuffers; ++i)
                                                bufferList.push_back(stream->CreateBuffer(bufSize, nullptr, nullptr));

                                            // set and start separate acquisition thread
                                            g_isStop = false;
                                            std::thread thread(AcquireImages, device, stream);
                                            std::cin.get();
                                            g_isStop = true;

                                            // cancel I/O for current buffer just in case acquisition thread is waiting it
                                            stream->CancelBuffer();

                                            thread.join();
                                            if (!g_result.compare(""))
                                                std::cout << "ERROR: AcquireImages failed" << std::endl;
                                            else
                                                std::cout << "\tImage Location: " << g_result << std::endl;

                                            stream->FlushBuffers(IpxCam::Flush_AllDiscard);
                                            FreeDataStreamBuffers(stream, &bufferList);

                                            // Close the stream
                                            std::cout << "Closing stream    ...." << std::endl;
                                            stream->Release();
                                            std::cout << "Closing stream    DONE" << std::endl;

                                            // Disconnect the device
                                            std::cout << "Disconnecting device(" << deviceName << ")    ...." << std::endl;
                                            device->Release();
                                            std::cout << "Disconnecting device(" << deviceName << ")    DONE" << std::endl;
                                        }
                                        else
                                            std::cout << "Unable to create a stream on" << deviceName << std::endl;
                                    }
                                    else
                                        std::cout << "Unable to connect to" << deviceName << std::endl;
                                }
                                else
                                {
                                    std::cout << "Cannot connect due to AccessStatus(" << GetAccessStatusStr(deviceInfo->GetAccessStatus()) << ")";
                                    std::cout << "\nConnecting to " << deviceName << " FAIL\n";
                                }
                            }
                        }
                    }
                    else
                        break;
                }

                std::cout << "Releasing system" << std::endl;
                system->Release();
            }
            else
                std::cout << "Unable to create system!" << std::endl;
        // }
    // }

    return 0;
}

IpxCam::Interface *SelectInterface( IpxCam::System *system )
{
    std::vector<IpxCam::Interface*> ifaces;

    // list has to be released, do this by using of unique pointer
    auto del = [](IpxCam::InterfaceList *l) { l->Release(); };
    std::unique_ptr<IpxCam::InterfaceList, decltype(del)> list(system->GetInterfaceList(), del);
    for (auto iface = list->GetFirst(); iface; iface = list->GetNext())
    {
        ifaces.push_back(iface);
    }

    uint32_t numIfaces = static_cast<uint32_t>(ifaces.size());
    if (numIfaces == 0)
        std::cout << "No interfaces available!\n";

    // Instead of selecting interface manually we are using GigE interface

    // Select interface
    auto iface = ifaces[1];
    std::cout << "Interface Selected: " << iface->GetDescription() << std::endl;
    return iface;
}

int GetConnectedDevices(IpxCam::Interface *iface)
{
    std::vector<IpxCam::DeviceInfo*> devices;
    int numDevices = 0;
    auto del = [](IpxCam::DeviceInfoList *l) { l->Release(); };
    std::unique_ptr<IpxCam::DeviceInfoList, decltype(del)> list(iface->GetDeviceInfoList(), del);
    for (auto device = list->GetFirst(); device; device = list->GetNext())
    {
        std::cout << "\tDevice Name: " << device->GetDisplayName() << "\tIdentified By: [" << (devices.size() - 1) << "]" << std::endl;
        char identity[STR_SIZE];
        snprintf(identity, STR_SIZE - 1, "%d", (devices.size() - 1));
        strcpy(shmmsg_gige->identifier[numDevices], identity);
        numDevices++;
    }
    shmmsg_gige->num_devices = numDevices;
    return numDevices;
}

IpxCam::DeviceInfo *SelectDevice( IpxCam::Interface *iface, int deviceIndex )
{
    std::vector<IpxCam::DeviceInfo*> devices;
    const char *indent = "    ";
	
    // re-enumerate just in case new devices were connected
	iface->ReEnumerateDevices(nullptr, 200);

    std::cout << indent << "Devices Available:" << std::endl;

    // list has to be released, do this by using of unique pointer
    auto del = [](IpxCam::DeviceInfoList *l) { l->Release(); };
    std::unique_ptr<IpxCam::DeviceInfoList, decltype(del)> list(iface->GetDeviceInfoList(), del);
    for (auto device = list->GetFirst(); device; device = list->GetNext())
    {
        devices.push_back(device);
        std::cout << indent << "[" << (devices.size() - 1) << "]" << "\t" << device->GetDisplayName() << std::endl;
    }

    uint32_t numDevices = static_cast<uint32_t>(devices.size());
    if (numDevices == 0)
        std::cout << indent << "No devices available!\n";

    std::cout << indent << "[" << devices.size() << "]" << "\t" << "To search for interface\n";

    std::cout << indent << "Enter Device Number (default [0]): ";
    uint32_t sel = deviceIndex;

    if (sel >= numDevices)
        return nullptr;

    // Select interface
    auto device = devices[sel];
    std::cout << indent << "Device Chosen: " << device->GetDisplayName() << std::endl;
    return device;
}

void GetSetParams( IpxCam::Device *device )
{
    const char *indent = "    ";
    auto genParams = device->GetCameraParameters();
    while (genParams)
    {
        try
        {
            std::cout << indent << "Set[1], get[2] a parameter, go on[0] (default [0]): ";
            uint32_t sel = 0;

            std::string str;
            std::getline(std::cin, str, '\n');
            if (!str.empty())
                sel = (uint32_t)std::atoi(str.c_str());

            if (!sel)
                break;

            bool isGet = sel > 1;

            std::string paramName, paramValue;

            std::cout << indent << "Enter parameter name: ";
            std::getline(std::cin, paramName);

            IpxGenParam::Param *param = nullptr;
            if (!(param = genParams->GetParam(paramName.c_str(), nullptr)))
            {
                std::cout << indent << "Cannot find '" << paramName << "' parameter!\n";
                continue;
            }

            // check if parameter Available, Readable, Writable
            if (!param->IsAvailable())
            {
                std::cout << indent << "'" << paramName << "' is not available!\n";
                continue;
            }
            else if (isGet)
            {
                if (!param->IsReadable())
                {
                    std::cout << indent << "'" << paramName << "' is not readable!\n";
                    continue;
                }
            }
            else if (!param->IsWritable()) // set
            {
                std::cout << indent << "'" << paramName << "' is not writable!\n";
                continue;
            }

            // convert to genuine type
            switch (param->GetType())
            {
                case IpxGenParam::ParamInt:
                {
                    IpxGenParam::Int *paramInt = dynamic_cast<IpxGenParam::Int*>(param);
                    if (paramInt)
                    {
                        // print limits
                        std::cout << indent << "Range of '" << paramName << "': " << paramInt->GetMin();
                        std::cout << ", " << paramInt->GetMax() << " inc: " << paramInt->GetIncrement() << std::endl;

                        if (isGet)
                            std::cout << indent << "'" << paramName << "' -> " << paramInt->GetValue() << std::endl;
                        else // set
                        {
                            std::cout << indent << "Enter parameter value: ";
                            std::getline(std::cin, paramValue);
                            auto value = std::stoll(paramValue);
                            std::cout << indent << value << " -> '" << paramName << "' errcode = ";
                            std::cout << (int)paramInt->SetValue(value) << std::endl;
                        }
                        continue;
                    }
                    break;
                }
                case IpxGenParam::ParamFloat:
                {
                    IpxGenParam::Float *paramFloat = dynamic_cast<IpxGenParam::Float*>(param);
                    if (paramFloat)
                    {
                        // print limits
                        std::cout << indent << "Range of '" << paramName << "': " << paramFloat->GetMin();
                        std::cout << ", " << paramFloat->GetMax() << std::endl;

                        if (isGet)
                            std::cout << indent << "'" << paramName << "' -> " << paramFloat->GetValue() << std::endl;
                        else // set
                        {
                            std::cout << indent << "Enter parameter value: ";
                            std::getline(std::cin, paramValue);
                            auto value = std::stod(paramValue);
                            std::cout << indent << value << " -> '" << paramName << "' errcode = ";
                            std::cout << (int)paramFloat->SetValue(value) << std::endl;
                        }
                        continue;
                    }
                    break;
                }
                case IpxGenParam::ParamString:
                {
                    IpxGenParam::String *paramString = dynamic_cast<IpxGenParam::String*>(param);
                    if (paramString)
                    {
                        if (isGet)
                            std::cout << indent << "'" << paramName << "' -> " << paramString->GetValue() << std::endl;
                        else // set
                        {
                            std::cout << indent << "Enter parameter value: ";
                            std::getline(std::cin, paramValue);
                            std::cout << indent << paramValue << " -> '" << paramName << "' errcode = ";
                            std::cout << (int)paramString->SetValue(paramValue.c_str()) << std::endl;
                        }
                        continue;
                    }
                    break;
                }
                case IpxGenParam::ParamEnum:
                {
                    IpxGenParam::Enum *paramEnum = dynamic_cast<IpxGenParam::Enum*>(param);
                    if (paramEnum)
                    {
                        if (isGet)
                            std::cout << indent << "'" << paramName << "' -> " << paramEnum->GetValueStr() << std::endl;
                        else // set
                        {
                            std::cout << indent << "Enter parameter value: ";
                            std::getline(std::cin, paramValue);

                            // try to set as string first
                            auto err = paramEnum->SetValueStr(paramValue.c_str());
                            if (err != IPX_CAM_ERR_OK)
                            {
                                auto value = std::stoll(paramValue);
                                err = paramEnum->SetValue(value);
                                std::cout << indent << value << " -> '" << paramName << "' errcode = ";
                            }
                            else
                                std::cout << indent << paramValue << " -> '" << paramName << "' errcode = ";

                            std::cout << (int)err << std::endl;
                        }
                        continue;
                    }
                    break;
                }
                case IpxGenParam::ParamBoolean:
                {
                    IpxGenParam::Boolean *paramBoolean = dynamic_cast<IpxGenParam::Boolean*>(param);
                    if (paramBoolean)
                    {
                        if (isGet)
                            std::cout << indent << "'" << paramName << "' -> " << paramBoolean->GetValue() << std::endl;
                        else // set
                        {
                            std::cout << indent << "Enter parameter value: ";
                            std::getline(std::cin, paramValue);
                            bool value = false;
                            if (paramValue == "true" || paramValue == "True" || paramValue == "TRUE")
                                value = true;
                            else
                                value = (bool)std::stoll(paramValue);
                            std::cout << indent << value << " -> '" << paramName << "' errcode = ";
                            std::cout << (int)paramBoolean->SetValue(value) << std::endl;
                        }
                        continue;
                    }
                    break;
                }
                case IpxGenParam::ParamCommand:
                {
                    IpxGenParam::Command *paramCommand = dynamic_cast<IpxGenParam::Command*>(param);
                    if (paramCommand)
                    {
                        if (isGet)
                            std::cout << indent << "'" << paramName << "' is done -> " << paramCommand->IsDone() << std::endl;
                        else // set
                        {
                            std::cout << indent << "Enter any value to execute command: ";
                            std::getline(std::cin, paramValue);
                            std::cout << indent << "execute '" << paramName << "' errcode = ";
                            std::cout << (int)paramCommand->Execute() << std::endl;
                        }
                        continue;
                    }
                    break;
                }
                case IpxGenParam::ParamCategory:
                {
                    IpxGenParam::Category *paramCategory = dynamic_cast<IpxGenParam::Category*>(param);
                    if (paramCategory)
                    {
                        // print all the parameter names under the category
                        auto end = paramCategory->GetCount();
                        std::cout << indent << "'" << paramName << "' category has:\n";
                        for (decltype(end) i = 0; i < end; ++i)
                        {
                            auto paramTemp = paramCategory->GetParamByIndex(i, nullptr);
                            if (paramTemp)
                                std::cout << indent << indent << "'" << paramTemp->GetDisplayName() << "'\n";
                        }
                        continue;
                    }
                    break;
                }
                default:
                    std::cout << indent << "Not supported parameter!";
                    continue;
            }

            // if we got here, when an error occured
            std::cout << indent << "An error occured while conversion of '" << paramName << "'!\n";
        }
        catch (const std::exception &e)
        {
            std::cout << indent << "Exception occured: " << e.what();
        }
    }
}

std::string AcquireImages( IpxCam::Device *device, IpxCam::Stream *stream )
{
    g_result = "";
    uint64_t images = std::numeric_limits<uint64_t>::max();
    uint64_t every = 1; // to save images. nothing by default

    // determine how many ticks in one second
    // used to calculate FPS
    IpxCamErr err;
    double timestampFreq = 1000000000.0; // second in nanoseconds
    IpxGenParam::Int *tsFreqParam = device->GetCameraParameters()->GetInt("GevTimestampTickFrequency", &err);
    if (tsFreqParam && err == IPX_CAM_ERR_OK)
    {
        timestampFreq = (double)tsFreqParam->GetValue(&err);
        if (err != IPX_CAM_ERR_OK)
            timestampFreq = 1000000000.0;
    }

    // set TLParamsLocked to 1
    auto genParams = device->GetCameraParameters();
    if (genParams && genParams->SetIntegerValue("TLParamsLocked", 1) == IPX_CAM_ERR_OK)
    {
        std::cout << "'TLParamsLocked' set to 1\n";

        // tell the device to start streaming images.
        if (stream->StartAcquisition() == IPX_CAM_ERR_OK)
        {
            std::cout << "Streaming started\n";

            if (genParams->ExecuteCommand("AcquisitionStart") == IPX_CAM_ERR_OK)
            {
                std::cout << "'AcquisitionStart' command sent\n";

                uint64_t timestamp = 0, frameId = 0, frameIdPrev = 0, droped = 0, incomplete = 0;
                double bandwidth = .0, fps = .0;
                for (decltype(images) i = 0; i < images; ++i)
                {
                    if (g_isStop)
                        break;

                    auto* buffer = stream->GetBuffer(UINT64_MAX, &err);
                    if (buffer && err == IPX_CAM_ERR_OK)
                    {
                        auto imagePtr = static_cast<char*>(buffer->GetBufferPtr()) + buffer->GetImageOffset();

                        frameId = buffer->GetFrameID();
                        if (frameIdPrev < frameId)
                            droped += (frameId - frameIdPrev - 1);

                        // timestamp of the first frame
                        if (timestamp)
                        {
                            // FPS and bandwidth as provided by camera
                            // not how many frames have been processed
                            uint64_t diff = timestamp;
                            timestamp = buffer->GetTimestamp();
                            diff = timestamp - diff;

                            fps = diff ? timestampFreq / diff : .0; // camera provides
                            bandwidth = fps * buffer->GetBufferSize() / 1048576.0;
                        }
                        else
                        {
                            timestamp = buffer->GetTimestamp();
                        }

                        frameIdPrev = frameId;
                        if (buffer->IsIncomplete())
                            ++incomplete;

                        struct stat st = {0};

                        if (stat("/data/Imperx", &st) == -1) {
                            mkdir("/data/Imperx", 0755);
                        }

                        char filename[200];
                        uint64_t timestampStr = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                        // snprintf(filename, 200, "/data/Imperx/GigE-Frame-%lu.raw", timestampStr);
                        snprintf(filename, 200, "/data/Imperx/GigE-Frame-%" PRIu64 ".raw", frameId - 1);
                        g_result = filename;
                        FILE *fp;
                        fp = fopen("./GigE-log.txt", "a");
                        fprintf(fp, "In KILL: %d\n", shmmsg_gpio->killsignal);
                        fprintf(fp, "\n");
                        fclose(fp);

                        // std::cout << "OK FID:"  << std::uppercase << std::hex << std::setfill('0') << std::setw(16) << frameId << " "
                        //     << std::dec << std::setfill(' ') << std::setw(4) << buffer->GetWidth()<< "W "
                        //     << std::dec << std::setfill(' ') << std::setw(4) << buffer->GetHeight()<< "H "
                        //     << std::setfill(' ') << std::setw(7) << std::setprecision(2) << std::fixed << fps << "FPS "
                        //     << std::setfill(' ') << std::setw(7) << bandwidth << "MB/s"
                        //     << " inc:" << incomplete << " dr:" << droped << " "
                        //     << "Frame Location: " << filename << "\r" << std::flush;

                        // save each i(th) file
                        // if (every && (i % every) == 0) {
                            FILE *fp = fopen(filename, "wb");
                            if (fp)
                            {
                                fwrite(imagePtr, sizeof(char), buffer->GetBufferSize(), fp);
                                fclose(fp);
                            }
                        // }

                        // re-queue the buffer in the stream object
                        stream->QueueBuffer(buffer);
                    }
                    else
                    {
                        // re-queue the buffer in the stream object
                        if (buffer)
                            stream->QueueBuffer(buffer);

                        std::cout << "ERR\r";
                    }
                }

                std::cout << std::endl;

                if (genParams->ExecuteCommand("AcquisitionStop") == IPX_CAM_ERR_OK)
                {
                    std::cout << "'AcquisitionStop' command sent\n";
                }
                else
                {
                    genParams->ExecuteCommand("AcquisitionAbort");
                    std::cout << "'AcquisitionAbort' command sent\n";
                }
            }

            // tell the device to stop streaming images.
            stream->StopAcquisition(1);
            std::cout << "Streaming stoped\n";
        }

        // set TLParamsLocked to 0
        genParams->SetIntegerValue("TLParamsLocked", 0);
        std::cout << "'TLParamsLocked' set to 0\n";
    }

    return g_result;
}

void FreeDataStreamBuffers(IpxCam::Stream *stream, std::vector<IpxCam::Buffer*> *buffers )
{
   // Go through the buffer vector
    std::for_each(std::begin(*buffers), std::end(*buffers),
        [stream](IpxCam::Buffer* buffer){ stream->RevokeBuffer(buffer); }
    );

	//Clear the buffer List
    buffers->clear();
}

const char *GetAccessStatusStr( int32_t status )
{
    switch (status)
    {
    case IpxCam::DeviceInfo::AccessStatusUnknown:
        return "Unknown";
    case IpxCam::DeviceInfo::AccessStatusReadWrite:
        return "ReadWrite";
    case IpxCam::DeviceInfo::AccessStatusReadOnly:
        return "ReadOnly";
    case IpxCam::DeviceInfo::AccessStatusNoAccess:
        return "NoAccess";
    case IpxCam::DeviceInfo::IpSubnetMismatch:
        return "SubnetMismatch";
    default:
        return "Unknown";
    }
}

std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
      tokens.push_back(token);

   return tokens;
}

bool SetIpAddress( IpxCam::DeviceInfo* devInfo )
{
    std::cout << "Would you like to set camera Ip Address [Ny]: ";
    std::string str = "y";
    // std::getline(std::cin, str, '\n');
    if (!str.empty())
    {
        auto ans = str.c_str()[0];
        if (ans == 'y' || ans == 'Y')
        {
            auto iface = devInfo->GetInterface();
            auto ifaceParams = iface->GetParameters();
            if (!ifaceParams)
                return false;

			IpxCamErr err;
			std::string description;
			auto ipAddress = ifaceParams->GetIntegerValue("GevInterfaceSubnetIPAddress", &err);
			if (err == IpxCamErr::IPX_CAM_ERR_OK)
			{
				description =  std::to_string(ipAddress >> 24 & 0xFF) + ".";
				description += std::to_string(ipAddress >> 16 & 0xFF) + ".";
				description += std::to_string(ipAddress >> 8  & 0xFF) + ".";
				description += std::to_string(ipAddress       & 0xFF);
			}
			else
			{
				description = iface->GetDescription();
			}

            // ask and validate Ip Address
			std::cout << "Enter Ip Address (Iface:" << description << ")[x.x.x.x]: ";
            std::string str = "169.254.1.1";
            // std::getline(std::cin, str, '\n');
            auto ipAdr = ValidateIpAddress(str);
            if (!ipAdr)
                return false;

            // set the same SubnetMask as the interface has
            auto subMask = ifaceParams->GetIntegerValue("GevInterfaceSubnetMask", &err);
            if (err != IpxCamErr::IPX_CAM_ERR_OK)
                return false;

			if (devInfo->ForceIP((uint32_t)ipAdr, (uint32_t)subMask, 0) != IpxCamErr::IPX_CAM_ERR_OK)
				return false;

            return true;
        }
    }

    return false;
}

uint32_t ValidateIpAddress( const std::string &ipAddress )
{
    if (ipAddress.empty())
        return 0;

    auto quadro = split(ipAddress, '.');
    if (quadro.size() != 4)
        return 0;

    try
    {
        return std::stoi(quadro[0]) << 24 | std::stoi(quadro[1]) << 16
             | std::stoi(quadro[2]) << 8  | std::stoi(quadro[3]);
    }
    catch (std::exception &) // to catch std::stoi exceptions
    {
    }

    return 0;
}
