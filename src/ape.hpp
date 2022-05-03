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
#define SHM_KEY_GPIO 0x4746
#define STR_SIZE 30
#define DEVICES 5

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
  char line1[STR_SIZE];
  char line2[STR_SIZE];
  char line3[STR_SIZE];
  char line4[STR_SIZE];
  char line5[STR_SIZE];
  char line6[STR_SIZE];
  char line7[STR_SIZE];
  char line8[STR_SIZE];
  char line9[STR_SIZE];
  char line10[STR_SIZE];
  char line11[STR_SIZE];
  char line12[STR_SIZE];
  char line13[STR_SIZE];
};

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

struct shm_gpio {
    int stat_ln;
    int stop_ln;
    int delay_ln;
    int record_ln;
    int prev_ln;
    int info_ln;
    int delay_config;
    int record_config;
    int delay_sec;
    int record_sec;
    int startsignal;
    int killsignal;
};

// Declare Global Variables
std::mutex mtx;
std::condition_variable cv;
struct shm_wepd *shmmsg_wepd;
struct shm_gige *shmmsg_gige;
struct shm_gpio *shmmsg_gpio;
int wepd_pid;
int gige_pid = 0;
int gpio_pid;
int terminate;
int display_ready = 0;
FILE* ape_log;

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
void  InterruptHandler(int signo);
void display_stats();
int display_init();

#endif // APE_HPP