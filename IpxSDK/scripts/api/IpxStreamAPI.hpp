/**
 * @file IpxStreamAPI.hpp
 * @author Anthony Kung (hi@anth.dev)
 * @name Primary Executable (Master Script)
 * @brief The primary program that runs on start up.
 * @version 0.0.1
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022 Anthony Kung <hi@anth.dev>
 * @license Apache-2.0
 * 
 */

// Include Guard
#ifndef IPX_API_HPP
#define IPX_API_HPP

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

#endif // IPX_API_HPP