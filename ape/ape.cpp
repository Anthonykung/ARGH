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
  int sharedStatus = 0;
  cout << "Started" << endl;
  thread thermal(temp_regulator, std::ref(sharedStatus));
  thread gpio(gpio_controller, std::ref(sharedStatus));
  thread gige(gige_controller, std::ref(sharedStatus));
  thread usbc(usbc_controller, std::ref(sharedStatus));
  unique_lock<mutex> lck(mtx);
  while (sharedStatus < 4) {
    cv.wait(lck);
  }
  thermal.join();
  gpio.join();
  gige.join();
  usbc.join();
  cout << "Exiting" << endl;
  return sharedStatus;
}

int gige_controller(/*int &sharedStatus*/) {
  // unique_lock<mutex> lck(mtx);
  // sharedStatus += 1;
  // cv.notify_one();
  return 0;
}

int gpio_controller(/*int &sharedStatus*/) {
  // unique_lock<mutex> lck(mtx);
  // sharedStatus += 1;
  // cv.notify_one();
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

int temp_regulator(/*int &sharedStatus*/) {
  // unique_lock<mutex> lck(mtx);
  // sharedStatus += 1;
  // cv.notify_one();
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
  string temp = "/sys/class/gpio/gpio422/value";
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
  return stoi(state);
}
