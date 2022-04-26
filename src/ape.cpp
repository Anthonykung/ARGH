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
// #include "../BosonUSB/buttonstates.hpp"

int main(int argc, char *argv[]) {
  cout << "APE Started!" << endl;

  terminate = 0;
  
  // signal(SIGINT, InterruptHandler);

  //****************************************

  // Interprocess Communication Waveshare e-Paper Display

  // ANTH = 2684
  int shmid_wepd = shmget(SHM_KEY_WEPD, sizeof(struct shm_wepd), IPC_CREAT | 0644);
  if (shmid_wepd < 0) {
    printf("IPC Init Failed\n");
    return shmid_wepd;
  }
  shmmsg_wepd = (struct shm_wepd *) shmat(shmid_wepd, NULL, 0);
  if (shmmsg_wepd == (void *) -1) {
    printf("IPC Init Failed\n");
    return -1;
  }

  shmmsg_wepd->busy = 1;

  //****************************************

  //****************************************

  // Interprocess Communication GIGE Camera

  // ANTH = 2684
  int shmid_gige = shmget(SHM_KEY_GIGE, sizeof(struct shm_gige), IPC_CREAT | 0644);
  if (shmid_gige < 0) {
      std::cout << "IPC Init Failed\n" << std::endl;
      return shmid_gige;
  }
  shmmsg_gige = (struct shm_gige *) shmat(shmid_gige, NULL, 0);
  if (shmmsg_gige == (void *) -1) {
      std::cout << "IPC Init Failed\n" << std::endl;
      return -1;
  }

  shmmsg_gige->busy = 1;
  shmmsg_gige->started = 0;

  //****************************************

  //****************************************

  // Interprocess Communication GPIO Operations

  // ANTH = 2684
  int shmid_gpio = shmget(SHM_KEY_GPIO, sizeof(struct shm_gpio), IPC_CREAT | 0644);
  if (shmid_gpio < 0) {
      std::cout << "IPC Init Failed\n" << std::endl;
      return shmid_gpio;
  }
  shmmsg_gpio = (struct shm_gpio *) shmat(shmid_gpio, NULL, 0);
  if (shmmsg_gpio == (void *) -1) {
      std::cout << "IPC Init Failed\n" << std::endl;
      return -1;
  }

  shmmsg_gpio->killsignal = 0;
  
  //****************************************


  int sharedStatus = 0;
  //thread thermal(temp_regulator, ref(sharedStatus));
  thread display_app(display_exe, ref(sharedStatus));
  thread gpio(gpio_controller, ref(sharedStatus));
  thread gige(gige_controller, ref(sharedStatus));
  thread display(display_controller, ref(sharedStatus));
  // thread button(button_main);
  thread display_manager(display_state);
  //thread usbc(usbc_controller, ref(sharedStatus));


  // unique_lock<mutex> lck(mtx);
  // while (sharedStatus < 2) {
  //   cv.wait(lck);
  // }
  // while (shmmsg_gpio->killsignal == 0) {
  //   /* code */
  // }
  
  // if (shmmsg_gpio->killsignal == 1) {
  //     FILE *fp;
  //     fp = fopen("./APE-log.txt", "a");
  //     fprintf(fp, "In KILL: %d\n", shmmsg_gpio->killsignal);
  //     fprintf(fp, "\n");
  //     fclose(fp);
  //   kill(gige_pid, SIGKILL);
  //   // kill(wepd_pid, SIGKILL);
  //   // kill(gpio_pid, SIGKILL);
  //   shmmsg_gpio->startsignal = 0;
  //   shmmsg_gpio->killsignal = 0;
  // }
  
  // while (true) {
  //   if (shmmsg_gpio->killsignal == 1) {
  //     kill(gige_pid, SIGKILL);
  //     // cout << "\033[38;2;255;20;147mGIGE Controller Killed!\033[0m" << endl;
  //     shmmsg_gige->started == 0;
  //     shmmsg_gpio->killsignal == 0;
  //   }
  // }

  
  ape_log = fopen("./APE-log.log", "a");
  fprintf(ape_log, "Entering Loop\n");
  fprintf(ape_log, "\n");
  fclose(ape_log);

  while (true) {

  }

  
  ape_log = fopen("./APE-log.log", "a");
  fprintf(ape_log, "Exiting Loop\n");
  fprintf(ape_log, "\n");
  fclose(ape_log);
  
  
  //thermal.join();
  gige.join();
  // usbc.join();
  display_app.join();
  display.join();
  display_manager.join();
  gpio.join();
  cout << "Exiting" << endl;
  return sharedStatus;
}

int gige_controller(int &sharedStatus) {
  unique_lock<mutex> lck(mtx);
  sharedStatus += 1;
  cv.notify_one();
  cout << "\033[38;2;255;20;147mStarting GIGE Controller...\033[0m" << endl;
  // shmmsg_gpio->startsignal = 1;
  // shmmsg_gige->started = 0;
  // shmmsg_gige->request = 1;
  // shmmsg_gige->start = 1;
  // shmmsg_gige->started == 0 && shmmsg_gpio->killsignal == 0
  while (true) {
      // FILE *fp;
      // fp = fopen("./GigE-log.txt", "a");
      // fprintf(fp, "shmmsg_gpio->startsignal: %d\n", shmmsg_gpio->startsignal);
      // fprintf(fp, "\n");
      // fclose(fp);
    if (shmmsg_gpio->startsignal == 1 && shmmsg_gige->started == 0 && gige_pid == 0) {
      cout << "\033[38;2;255;20;147mGIGE Controller Started!\033[0m" << endl;
      gige_pid = fork();
      if (gige_pid == 0) {
        FILE *fp;
        fp = fopen("./APE-log.log", "a");
        fprintf(fp, "GigE Started PID: %d\n", gige_pid);
        fprintf(fp, "\n");
        fclose(fp);
        shmmsg_gpio->startsignal = 0;
        execl("../IpxSDK/scripts/build/api/IpxStreamAPI", "../IpxSDK/scripts/build/api/IpxStreamAPI", NULL);
      }
    }
    if (shmmsg_gpio->killsignal == 1) {
      // kill(gige_pid, SIGINT);
      // cout << "\033[38;2;255;20;147mGIGE Controller Killed!\033[0m" << endl;
      shmmsg_gige->started == 0;
      shmmsg_gpio->killsignal == 0;
      gige_pid = 0;
    }
  }
  
  // if (shmmsg_gpio->startsignal) {
  //   cout << "\033[38;2;255;20;147mGIGE Controller Started!\033[0m" << endl;
  //   gige_pid = fork();
  //   if (gige_pid == 0) {
  //     execl("../IpxSDK/scripts/build/api/IpxStreamAPI", "../IpxSDK/scripts/build/api/IpxStreamAPI", NULL);
  //     shmmsg_gpio->startsignal = 0;
  //   }
  // }
  // if (gige_pid != 0 && shmmsg_gpio->killsignal == 1) {
  //   cout << "GIGE Controller Killed!" << endl;
  //   kill(gige_pid, SIGKILL);
  // }
  
  return 0;
}

int gpio_controller(int &sharedStatus) {
  unique_lock<mutex> lck(mtx);
  sharedStatus += 1;
  cv.notify_one();
  gpio_pid = fork();
  if (gpio_pid == 0) {
    cout << "GPIO Controller Started!" << endl;
    execl("../BosonUSB/exstates", "../BosonUSB/exstates", NULL);
  }
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
  fstream fan_pwm;
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

int interactive() {
  while (true){
    while (shmmsg_wepd->busy) {
      // usleep(100);
    }
    char user_input[256];
    printf("APE Ready: ");
    scanf(user_input, 256);
    strcpy(shmmsg_wepd->cmd, "write");
    display_write(1, string(user_input));
    // Let Display Controller know that a message is ready to be read
    shmmsg_wepd->request = 1;
  }
  
}

void  InterruptHandler(int signo) {
    //System Exit
    printf("\r\nAPE Interrupt Handler\r\n");
    // shmmsg_gpio->startsignal = 0;
    // shmmsg_gpio->killsignal = 0;

    // // If Display is busy, wait for it to be free
    // while (shmmsg_wepd->busy == 1 || shmmsg_wepd->request == 1) {
    //   // Waiting For Display to be free
    // }
    // // Set command
    // strcpy(shmmsg_wepd->cmd, "exit");
    // // Let Display Controller know that a message is ready to be read
    // shmmsg_wepd->request = 1;

    // // If Display is busy, wait for it to be free
    // while (shmmsg_wepd->busy == 1 || shmmsg_wepd->request == 1) {
    //   // Waiting For Display to be free
    // }

    
    kill(wepd_pid, SIGINT);
    kill(gige_pid, SIGINT);
    // kill(gpio_pid, SIGINT);

    exit(0);
}

int display_controller(int &sharedStatus) {
  unique_lock<mutex> lck(mtx);
  sharedStatus += 1;
  cv.notify_one();

  ape_log = fopen("./APE-log.log", "a");
  fprintf(ape_log, "Entering Display Controller\n");
  fprintf(ape_log, "\n");
  fclose(ape_log);

  // *************************************************************************
  // printf("Initializing Display\n");
  // // If Display is busy, wait for it to be free
  // while (shmmsg_wepd->busy == 1 || shmmsg_wepd->request == 1) {
  //   // Waiting For Display to be free
  // }

  // // Set command
  // strcpy(shmmsg_wepd->cmd, "init");

  // // Let Display Controller know that a message is ready to be read
  // shmmsg_wepd->request = 1;

  // *************************************************************************

  // *************************************************************************
  // // If Display is busy, wait for it to be free
  // while (shmmsg_wepd->busy == 1 || shmmsg_wepd->request == 1) {
  //   // Waiting For Display to be free
  // }
  // // Set command
  // strcpy(shmmsg_wepd->cmd, "exit");
  // // Let Display Controller know that a message is ready to be read
  // shmmsg_wepd->request = 1;
  // *************************************************************************
  

  // *************************************************************************
  // If Display is busy, wait for it to be free
  while (shmmsg_wepd->busy == 1 || shmmsg_wepd->request == 1) {
    // Waiting For Display to be free
    ape_log = fopen("./APE-Loop.log", "a");
    fprintf(ape_log, "Display Busy: %d\n", shmmsg_wepd->busy);
    fprintf(ape_log, "Display Request: %d\n", shmmsg_wepd->request);
    fprintf(ape_log, "\n");
    fclose(ape_log);
  }
  ape_log = fopen("./APE-log.log", "a");
  fprintf(ape_log, "Display Freed\n");
  fprintf(ape_log, "\n");
  fclose(ape_log);
  // Set command
  strcpy(shmmsg_wepd->cmd, "start write");
  // Let Display Controller know that a message is ready to be read
  shmmsg_wepd->request = 1;
  // *************************************************************************

  ape_log = fopen("./APE-log.log", "a");
  fprintf(ape_log, "Start Write Requested: %d\n", shmmsg_wepd->request);
  fprintf(ape_log, "Command: %s\n", shmmsg_wepd->cmd);
  fprintf(ape_log, "\n");
  fclose(ape_log);

  display_ready = true;

  int display_status = 0;
  int display_interval = 0;
  int display_timer = 0;

  display_write(1, "Init Complete");

  // Refresh the display every 3 minutes to prevent failure
  while(display_status == 0) {
    if (display_interval == 3) {
      printf("Refreshing Display\n");
      // If Display is busy, wait for it to be free
      while (shmmsg_wepd->busy == 1 || shmmsg_wepd->request == 1) {
        // Waiting For Display to be free
      }
      // Set command
      strcpy(shmmsg_wepd->cmd, "refresh");
      printf("Refresh Complete\n");
      // Let Display Controller know that a message is ready to be read
      shmmsg_wepd->request = 1;
      // If Display is busy, wait for it to be free
      while (shmmsg_wepd->busy == 1 || shmmsg_wepd->request == 1) {
        // Waiting For Display to be free
      }
      // Set command
      strcpy(shmmsg_wepd->cmd, "start write");
      // Let Display Controller know that a message is ready to be read
      shmmsg_wepd->request = 1;
      display_interval = 0;
    }
    else {
      sleep(1);
      display_timer++;
      // display_write(11, "Refresh Timer: " + to_string(display_timer));
      if (display_timer == 60) {
        display_interval++;
        display_timer = 0;
      }
      else if (display_timer == 50 && display_interval == 2) {
        display_write(12, "Refresh Imminent");
        display_write(13, "This may take 20 sec");
      }
    }
  }

  // If Display is busy, wait for it to be free
  while (shmmsg_wepd->busy == 1 || shmmsg_wepd->request == 1) {
    // Waiting For Display to be free
  }
  // Set command
  strcpy(shmmsg_wepd->cmd, "write end");
  // Let Display Controller know that a message is ready to be read
  shmmsg_wepd->request = 1;
  return 0;
}

int display_exe(int &sharedStatus) {
  unique_lock<mutex> lck(mtx);
  sharedStatus += 1;
  cv.notify_one();
  wepd_pid = fork();
  if (wepd_pid == 0) {
    execl("../EPD/epd", "../EPD/epd", NULL);
  }
  return 0;
}

int display_write(int line, string message) {
  // If Display is busy, wait for it to be free
  while (shmmsg_wepd->busy == 1 || shmmsg_wepd->request == 1) {
    // Waiting For Display to be free
  }

  // Set command
  strcpy(shmmsg_wepd->cmd, "write");

  // Assign number of lines
  shmmsg_wepd->num_line = line;

  // Append message
  strcpy(shmmsg_wepd->msg, message.c_str());

  // Let Display Controller know that a message is ready to be read
  shmmsg_wepd->request = 1;
}

int display_state() {

  // Control Variables
  // int stat_ln;
  // int stop_ln;
  // int delay_ln;
  // int record_ln;
  // int prev_ln;
  // int info_ln;

  // int delay_sec;
  // int delay_config;
  // int record_sec;
  // int record_config;
  while (true) {
    ape_log = fopen("./APE-Loop.log", "a");
    fprintf(ape_log, "Checking Display State: %d\n", display_ready);
    fprintf(ape_log, "\n");
    fclose(ape_log);
    if (display_ready) {
      string info_str;

      switch (shmmsg_gpio->stat_ln) {
        case 0:
          display_write(0, "Waiting...");
          break;
        case 1:
          display_write(0, "Recording...");
          break;
        case 2:
          display_write(0, "Stopped...");
          break;
        case 3:
          display_write(0, "Configuring...");
          break;
        case 4:
          display_write(0, "Configuring Camera...");
          break;
        default:
          display_write(0, "ERROR");
          break;
      }

      switch (shmmsg_gpio->stop_ln) {
        case 0:
          display_write(1, "  Start/Stop Recording");
          break;
        case 1:
          display_write(1, "> Start/Stop Recording");
          break;
        default:
          display_write(1, "ERROR");
          break;
      }

      switch (shmmsg_gpio->delay_ln) {
        case 0:
          display_write(2, "  Preset Delay " + to_string(shmmsg_gpio->delay_sec/3600) + ":" + to_string((shmmsg_gpio->delay_sec%3600)/60) + ":" + to_string(shmmsg_gpio->delay_sec%60));
          break;
        case 1:
          display_write(2, ">  Preset Delay " + to_string(shmmsg_gpio->delay_sec/3600) + ":" + to_string((shmmsg_gpio->delay_sec%3600)/60) + ":" + to_string(shmmsg_gpio->delay_sec%60));
          break;
        case 2:
          display_write(2, ">  Preset Delay " + to_string(shmmsg_gpio->delay_config/3600) + ":" + to_string((shmmsg_gpio->delay_config%3600)/60) + ":" + to_string(shmmsg_gpio->delay_config%60));
          break;
        default:
          display_write(2, "ERROR");
          break;
      }

      switch (shmmsg_gpio->record_ln) {
        case 0:
          display_write(3, "  Record Time " + to_string(shmmsg_gpio->record_sec/3600) + ":" + to_string((shmmsg_gpio->record_sec%3600)/60) + ":" + to_string(shmmsg_gpio->record_sec%60));
          break;
        case 1:
          display_write(3, "> Record Time " + to_string(shmmsg_gpio->record_sec/3600) + ":" + to_string((shmmsg_gpio->record_sec%3600)/60) + ":" + to_string(shmmsg_gpio->record_sec%60));
          break;
        case 2:
          display_write(3, "> Record Time " + to_string(shmmsg_gpio->record_config/3600) + ":" + to_string((shmmsg_gpio->record_config%3600)/60) + ":" + to_string(shmmsg_gpio->record_config%60));
          break;
        default:
          display_write(3, "ERROR");
          break;
      }

      switch (shmmsg_gpio->prev_ln) {
        case 0:
          display_write(4, "  Previous Settings");
          break;
        case 1:
          display_write(4, "> Previous Settings");
          break;
        default:
          display_write(4, "ERROR");
          break;
      }

      switch (shmmsg_gpio->info_ln) {
        case 0:
          display_write(5, "");
          break;
        case 1:
          display_write(5, "> " + info_str);
          break;
        default:
          display_write(5, "ERROR");
          break;
      }
    }
  }

}
