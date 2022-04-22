/**
 * @file ape.cpp
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

// Include Internal Libraries
#include "ape.hpp"

int main(int argc, char *argv[]) {
  cout << "APE Started!" << endl;
  int sharedStatus = 0;
  thread thermal(temp_regulator, ref(sharedStatus));
  thread gpio(gpio_controller, ref(sharedStatus));
  //thread gige(gige_controller, ref(sharedStatus));
  //thread usbc(usbc_controller, ref(sharedStatus));
  unique_lock<mutex> lck(mtx);
  while (sharedStatus < 4) {
    cv.wait(lck);
  }
  thermal.join();
  gpio.join();
  // gige.join();
  // usbc.join();
  cout << "Exiting" << endl;
  return sharedStatus;
}

int gige_controller(int &sharedStatus) {
  unique_lock<mutex> lck(mtx);
  sharedStatus += 1;
  cv.notify_one();
  return 0;
}

int gpio_controller(int &sharedStatus) {
  //unique_lock<mutex> lck(mtx);
  //sharedStatus += 1;
  //cv.notify_one();
  gpio_export(422);  // PIN 7
  gpio_export(248);  // PIN 33
  gpio_set_direction(422, "in");
  gpio_set_direction(248, "out");
  for (int i = 0; i < 100; i++) {
    gpio_set_state(248, "1");
    cout << gpio_get_state(422) << endl;
    usleep(100);
    gpio_set_state(248, "0");
    cout << gpio_get_state(422) << endl;
    usleep(100);
  }
  gpio_unexport(248);
  gpio_unexport(422);
  cout << "GPIO Controller Terminated" << endl;
  return 0;
}

int temp_regulator(int &sharedStatus) {
  //unique_lock<mutex> lck(mtx);
  //sharedStatus += 1;
  //cv.notify_one();
  int cpu_temp = check_cpu_temp();
  int gpu_temp = check_gpu_temp();
  int avg_temp = (cpu_temp + gpu_temp) / 2;
  int fan_speed = avg_temp * 2;
  fan_control(fan_speed);
  cout << "CPU Temp: " << cpu_temp << endl;
  cout << "GPU Temp: " << gpu_temp << endl;
  cout << "Average Temp: " << avg_temp << endl;
  cout << "Fan Speed: " << fan_speed << endl;
  return 0;
}

int fan_control(int fan_speed) {
  ofstream fan_pwm;
  fan_pwm.open("/sys/devices/pwm-fan/target_pwm");
  fan_pwm << fan_speed;
  fan_pwm.close();
  return 0;
}

int check_fan_speed() {
  ifstream fan_pwm;
  fan_pwm.open("/sys/devices/pwm-fan/target_pwm");
  string fan_speed;
  string line;
  while (getline(fan_pwm, line)) {
    fan_speed = line;
  }
  fan_pwm.close();
  cout << "Fan Speed: " << fan_speed << endl;
  return stoi(fan_speed);
}

int check_cpu_temp() {
  ifstream cpu_temp(CPU_TEMP);
  string cpu_temp_str;
  string line;
  while (getline(cpu_temp, line)) {
    cpu_temp_str += line;
  }
  cpu_temp.close();
  cout << "CPU Temp: " << cpu_temp_str << endl;
  return stoi(cpu_temp_str);
}

int check_gpu_temp() {
  ifstream gpu_temp(GPU_TEMP);
  string gpu_temp_str;
  string line;
  while (getline(gpu_temp, line)) {
    gpu_temp_str += line;
  }
  gpu_temp.close();
  cout << "GPU Temp: " << gpu_temp_str << endl;
  return stoi(gpu_temp_str);
}

int gpio_export(int gpio) {
  ofstream gpio_export;
  gpio_export.open("/sys/class/gpio/export");
  gpio_export << gpio;
  gpio_export.close();
  return 0;
}

int gpio_unexport(int gpio) {
  ofstream gpio_unexport;
  gpio_unexport.open("/sys/class/gpio/unexport");
  gpio_unexport << gpio;
  gpio_unexport.close();
  return 0;
}

/**
 * @name gpio_set_direction
 * @brief Sets the direction of the GPIO pin.
 * 
 * @param gpio The GPIO pin to set the direction of.
 * @param direction The direction to set the GPIO pin to ("in", "out").
 * @return int
 */
int gpio_set_direction(int gpio, string direction) {
  ofstream gpio_direction;
  string temp = "/sys/class/gpio/gpio" + to_string(gpio) + "/direction";
  gpio_direction.open(temp);
  gpio_direction << direction;
  gpio_direction.close();
  return 0;
}

/**
 * @name gpio_set_state
 * @brief Sets the state of the GPIO pin.
 * 
 * @param gpio The GPIO pin to set the state of.
 * @param state The state to set the GPIO pin to ("0", "1").
 * @return int 
 */
int gpio_set_state(int gpio, string state) {
  ofstream gpio_state;
  string temp = "/sys/class/gpio/gpio" + to_string(gpio) + "/value";
  gpio_state.open(temp);
  gpio_state << state;
  gpio_state.close();
  return 0;
}

/**
 * @name gpio_get_state
 * @brief Gets the state of the GPIO pin.
 * 
 * @param gpio The GPIO pin to get the state of.
 * @return int
 */
int gpio_get_state(int gpio) {
  ifstream gpio_state;
  string temp = "/sys/class/gpio/gpio" + to_string(gpio) + "/value";
  gpio_state.open(temp);
  string state;
  string line;
  while (getline(gpio_state, line)) {
    state += line;
  }
  gpio_state.close();
  cout << "GPIO State: " << state << endl;
  return stoi(state);
}

int display_write(int line, string message) {

}

int display_state() {

  // Control Variables
  int stat_ln;
  int stop_ln;
  int delay_ln;
  int record_ln;
  int prev_ln;
  int info_ln;

  int delay_sec;
  int delay_config;
  int record_sec;
  int record_config;
  string info_str;

  switch (stat_ln) {
    case 0:
      display_write(1, "Waiting...");
      break;
    case 1:
      display_write(1, "Recording...");
      break;
    case 2:
      display_write(1, "Stopped...");
      break;
    case 3:
      display_write(1, "Configuring...");
      break;
    default:
      display_write(1, "ERROR");
      break;
  }

  switch (stop_ln) {
    case 0:
      display_write(2, "  Start/Stop Recording");
      break;
    case 1:
      display_write(2, "> Start/Stop Recording");
      break;
    default:
      display_write(2, "ERROR");
      break;
  }

  switch (delay_ln) {
    case 0:
      display_write(3, "  Preset Delay" + to_string(delay_sec/3600) + ":" + to_string((delay_sec%3600)/60) + ":" + to_string(delay_sec%60));
      break;
    case 1:
      display_write(3, ">  Preset Delay" + to_string(delay_sec/3600) + ":" + to_string((delay_sec%3600)/60) + ":" + to_string(delay_sec%60));
      break;
    case 2:
      display_write(3, ">  Preset Delay" + to_string(delay_config/3600) + ":" + to_string((delay_config%3600)/60) + ":" + to_string(delay_config%60));
      break;
    default:
      display_write(3, "ERROR");
      break;
  }

  switch (record_ln) {
    case 0:
      display_write(4, "  Recording Time" + to_string(record_sec/3600) + ":" + to_string((record_sec%3600)/60) + ":" + to_string(record_sec%60));
      break;
    case 1:
      display_write(4, "> Recording Time" + to_string(record_sec/3600) + ":" + to_string((record_sec%3600)/60) + ":" + to_string(record_sec%60));
      break;
    case 2:
      display_write(4, "> Recording Time" + to_string(record_config/3600) + ":" + to_string((record_config%3600)/60) + ":" + to_string(record_config%60));
      break;
    default:
      display_write(4, "ERROR");
      break;
  }

  switch (prev_ln) {
    case 0:
      display_write(5, "  Previous Settings");
      break;
    case 1:
      display_write(5, "> Previous Settings");
      break;
    default:
      display_write(5, "ERROR");
      break;
  }

  switch (info_ln) {
    case 0:
      display_write(6, "");
      break;
    case 1:
      display_write(6, "> " + info_str);
      break;
    default:
      display_write(6, "ERROR");
      break;
  }

}