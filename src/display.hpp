/**
 * @file display.hpp
 * @author Anthony Kung (hi@anth.dev)
 * @name Waveshare e-Paper Interface
 * @brief This file is responsible for managing e-Paper display operation
 * @version 0.0.1
 * @date 2022-04-19
 * 
 * @copyright Copyright (c) 2022 Anthony Kung <hi@anth.dev>
 * @license Apache-2.0
 * 
 */

// Include Guard
#ifndef DISPLAY_HPP
#define DISPLAY_HPP

// Include C++ Libraries
#include <cstdio>
#include <string>

// Include C Libraries
#include <stdlib.h>
#include <time.h>
#include <signal.h>

// Include Waveshare Libraries
#include "EPD_4in2.h"
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"
#include "ImageData.h"
#include "Debug.h"

// Define Namespaces
using std::string;

#endif // DISPLAY_HPP