/**
 * @file ape.hpp
 * @author Anthony Kung (hi@anth.dev)
 * @name Primary Executable (Master Script)
 * @brief The primary program that runs on start up.
 * @version 0.0.1
 * @date 2022-02-04
 * 
 * @copyright Copyright (c) 2022 Anthony Kung <hi@anth.dev>
 * @license Apache-2.0
 * 
 */

// Include Guard
#ifndef APE_HPP
#define APE_HPP

// Include C++ Libraries
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <fstream>
#include <atomic>
#include <condition_variable>
#include <cmath>
#include <cstring>
#include <climits>

// Include C Libraries
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// Define Local Constants
#define CPU_TEMP "/sys/class/thermal/thermal_zone0/temp"
#define GPU_TEMP "/sys/class/thermal/thermal_zone1/temp"
#define SHM_KEY_WEPD 0x9373
#define SHM_KEY_GIGE 0x4443
#define STR_SIZE 1000

// Define Namespaces
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::to_string;
using std::thread;
using std::mutex;
using std::condition_variable;
using std::unique_lock;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::stoi;
using std::ref;

// Declare Global Structs
struct shm_wepd {
  int request;
  int busy;
  char cmd[STR_SIZE];
  int num_line;
  char msg[STR_SIZE];
};

// Declare Global Variables
std::mutex mtx;
std::condition_variable cv;
struct shm_wepd *shmmsg_wepd;
int wepd_pid;

// Declare Functions
int fan_control(int fan_speed);
int check_cpu_temp();
int check_gpu_temp();
int check_fan_speed();
int gpio_export(int gpio);
int gpio_unexport(int gpio);
int gpio_set_direction(int gpio, string direction);
int gpio_set_state(int gpio, string state);
int gpio_get_state(int gpio);
int temp_regulator(int &sharedStatus);
int gpio_controller(int &sharedStatus);
int gige_controller(int &sharedStatus);
int usbc_controller(int &sharedStatus);
int display_controller(int &sharedStatus);
int display_exe(int &sharedStatus);
int display_write(int line, string message);
int display_state();
int interactive();

#endif // APE_HPP