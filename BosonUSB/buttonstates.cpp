#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <sstream>
#include <thread>
#include <signal.h>
#include <unistd.h>
#include <filesystem>
#include <experimental/filesystem>
//#include <dirent.h>
//#include <sys/stat.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_KEY_GPIO 0x4746
#define STR_SIZE 1000

struct shm_gpio
{
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

struct shm_gpio *shmmsg_gpio;
int previous_stat;
int needsrestart;
pid_t bosonstorepid;
pid_t bosoncampid;
pid_t impstorepid;
int recordingwasstarted;
// using std::string;
// using std::ofstream;
// using std::ifstream;
// using std::fstream;
//  using namespace std;
//  using namespace std::experimental::filesystem::v1;

using namespace std;
using namespace std::experimental::filesystem::v1;

void callbosonstore(time_t currtime)
{
  printf("WE ARE IN THE CALL STORE BOSON\n");
  char summontheboson[] = "../BosonUSB/exrunusb";
      printf("WHAT IS IT -2\n");
  char *bosstr[3];
      printf("WHAT IS IT -1\n");
  bosstr[0] = "../BosonUSB/exrunusb";
      printf("WHAT IS IT 0\n");

    printf("WHAT IS IT 4\n");

  char temp[15];
  sprintf(temp,%d,currtime);
  bosstr[1] = *temp; 
  printf("WHAT IS IT bos: %s\n",bosstr[1]);
  //snprintf(bosstr[1],15,"%i",currtime);
  bosstr[2] = NULL;
  execvp(summontheboson, bosstr);
  printf("exiting boson call\n");
  exit(0);
}

void callimpstore(time_t currtime)
{
  printf("WE ARE IN THE IMP STORE \n");
  char summontheimp[] = "../BosonUSB/eximpstore";
  char *impstr[3];
  impstr[0] = "../BosonUSB/eximpstore";
  stringstream ss;
  ss << currtime;
  string timestring = ss.str();
  ss.str("");
  strcpy(impstr[1],timestring.c_str());
  printf("WHAT IS IT imp: %s",impstr[1]);
  impstr[2] = NULL;
  execvp(summontheimp, impstr);
  printf("exiting impstore call\n");
  exit(0);
}

void callbosoncam()
{
  //printf("ARE WE EVEN GETTING HERE???????????\n");
  char commandboson[] = "../BosonUSB/BosonUSB"; // removed t1000
  char *fstr[3];
  fstr[0] = "../BosonUSB/BosonUSB";
  fstr[1] = "f";
  fstr[2] = NULL;
  execvp(commandboson, fstr);
  printf("END OF bosoncam\n");
  exit(0);
}

int gpio_set_direction(int gpio, string direction)
{
  ofstream gpio_direction;
  string temp = "/sys/class/gpio/gpio" + to_string(gpio) + "/direction";
  gpio_direction.open(temp);
  gpio_direction << direction;
  gpio_direction.close();
  return 0;
}

int gpio_export(int gpio)
{
  ofstream gpio_export;
  gpio_export.open("/sys/class/gpio/export");
  gpio_export << gpio;
  gpio_export.close();
  return 0;
}

int gpio_get_state(int gpio)
{
  ifstream gpio_state;
  string temp = "/sys/class/gpio/gpio" + to_string(gpio) + "/value";
  gpio_state.open(temp);
  string state;
  string line;
  while (getline(gpio_state, line))
  {
    state += line;
  }
  gpio_state.close();
  //  cout << "GPIO State: " << state << endl;
  // printf("GETSTATE STOI \n");
  return stoi(state);
}
// Power loss during this any issues?

// need to wire in stat_ln 0->1 transition
// stat_ln = 3 should put it in standby mode as far as other functions are concerned and also should cause camera system restart after
// stat_ln = 2 should stop the recording (it should restart after it is set back to 0)

// The program should restart once it receives a stat_ln 0 again.
// Need to figure out when delay and record are read from files
// How will these be run? If stat is only set to 3 during stuff will it ever be shown on screen? Mainly for set to prev
// Add in some way to get back to the top after config mode
// Need to make sure that
// Send a signal to kill cameras when 2 or 3 entered
// Buttons must be released for two of them
// Make sure flight number also increments on restart

// for me:
// Do I need to protect other modes against the 3 setting?

void selectpressed()
{
  // Starting State stop/start mode
  if (shmmsg_gpio->stat_ln != 2 && shmmsg_gpio->stop_ln == 1)
  {
    shmmsg_gpio->stat_ln = 2; // stops recording
    if (recordingwasstarted == 1)
    {
      shmmsg_gpio->killsignal = 1;
      kill(bosoncampid, SIGKILL);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      kill(bosonstorepid, SIGKILL);
      recordingwasstarted = 0;
      while (shmmsg_gpio->startsignal == 1)
      {
        // Waits for imprex to be killed
      }
      kill(impstorepid, SIGKILL);
    }
    needsrestart = 1;
  }

  // Line 1 but recording is stopped already
  else if (shmmsg_gpio->stat_ln == 2 && shmmsg_gpio->stop_ln == 1)
  {
    shmmsg_gpio->stat_ln = 0; // start recording back up
  }

  // pushes the variable into the real file and resumes recording
  else if (shmmsg_gpio->delay_ln == 2)
  {
    shmmsg_gpio->delay_ln = 1;
    shmmsg_gpio->info_ln = 0;
    shmmsg_gpio->delay_sec = shmmsg_gpio->delay_config; // set file var to config var

    ofstream rewritedelay;
    rewritedelay.open("../BosonUSB/timeddelay.txt", ios::out | ios::trunc);
    rewritedelay << shmmsg_gpio->delay_sec << endl;
    rewritedelay.close();

    ofstream rewriteprevdelay;
    rewriteprevdelay.open("../BosonUSB/previousdelay.txt", ios::out | ios::trunc);
    rewriteprevdelay << shmmsg_gpio->delay_sec << endl;
    rewriteprevdelay.close();
    if (previous_stat == 2)
    {
      shmmsg_gpio->stat_ln = 2;
    }
    else
    {
      shmmsg_gpio->stat_ln = 0;
    }
  }

  // pushes the variable into the real file and resumes recording
  else if (shmmsg_gpio->record_ln == 2)
  {
    shmmsg_gpio->record_ln = 1;
    shmmsg_gpio->info_ln = 0;
    shmmsg_gpio->record_sec = shmmsg_gpio->record_config; // set file var to config var
    ofstream rewriterecord;
    rewriterecord.open("../BosonUSB/recordingtime.txt", ios::out | ios::trunc);
    rewriterecord << shmmsg_gpio->record_sec << endl;
    rewriterecord.close();

    ofstream rewriteprevrecord;
    rewriteprevrecord.open("../BosonUSB/previousrecord.txt", ios::out | ios::trunc);
    rewriteprevrecord << shmmsg_gpio->record_sec << endl;
    rewriteprevrecord.close();
    if (previous_stat == 2)
    {
      shmmsg_gpio->stat_ln = 2;
    }
    else
    {
      shmmsg_gpio->stat_ln = 0;
    }
  }

  // sets variables to previously selected data
  else if (shmmsg_gpio->prev_ln == 1)
  {
    previous_stat = shmmsg_gpio->stat_ln;
    shmmsg_gpio->stat_ln = 3;
    if (recordingwasstarted == 1)
    {
      shmmsg_gpio->killsignal = 1;
      kill(bosoncampid, SIGKILL);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      kill(bosonstorepid, SIGKILL);
      recordingwasstarted = 0;
      while (shmmsg_gpio->startsignal == 1)
      {
        // Waits for imprex to be killed
      }
      kill(impstorepid, SIGKILL);
    }

    fstream prevdelayfile;
    string delaytime;
    prevdelayfile.open("../BosonUSB/previousdelay.txt", ios::in);
    if (prevdelayfile.is_open())
    {
      getline(prevdelayfile, delaytime);
      prevdelayfile.close();
    }
    printf("Delay1 STOI \n");
    shmmsg_gpio->delay_sec = stoi(delaytime);

    fstream prevrecfile;
    string recordtime;
    prevrecfile.open("../BosonUSB/previousrecord.txt", ios::in);
    if (prevrecfile.is_open())
    {
      getline(prevrecfile, recordtime);
      prevrecfile.close();
    }
    printf("Record1 STOI \n");
    shmmsg_gpio->record_sec = stoi(recordtime);

    ofstream rewritedelay;
    rewritedelay.open("../BosonUSB/timeddelay.txt", ios::out | ios::trunc);
    rewritedelay << shmmsg_gpio->delay_sec << endl;
    rewritedelay.close();

    ofstream rewriterecord;
    rewriterecord.open("../BosonUSB/recordingtime.txt", ios::out | ios::trunc);
    rewriterecord << shmmsg_gpio->record_sec << endl;
    rewriterecord.close();

    if (previous_stat == 2)
    {
      shmmsg_gpio->stat_ln = 2;
    }
    else
    {
      shmmsg_gpio->stat_ln = 0;
    }

    needsrestart = 1;
  }
}

void menupressed()
{
  // Starting State stop/start mode
  if (shmmsg_gpio->stop_ln == 1)
  {
    shmmsg_gpio->stop_ln = 0;  // removes > from stop line
    shmmsg_gpio->delay_ln = 1; // adds > to delay_ln
  }

  // switches from delay to record mode
  else if (shmmsg_gpio->delay_ln == 1)
  {
    shmmsg_gpio->delay_ln = 0;  // removes > from delay_ln
    shmmsg_gpio->record_ln = 1; // adds > to record_ln
  }

  // switches from delay to record mode and does not save any changes
  else if (shmmsg_gpio->delay_ln == 2)
  {
    shmmsg_gpio->delay_ln = 0;  // removes > from delay_ln and switches back to original time
    shmmsg_gpio->record_ln = 1; // adds > to record_ln
    shmmsg_gpio->info_ln = 0;   // turns off info_ln
    if (previous_stat == 2)
    { // if previously stopped returns to that
      shmmsg_gpio->stat_ln = 2;
    }
    else
    { // else restarts recording
      shmmsg_gpio->stat_ln = 0;
    }
  }

  // switches from record mode to prev mode
  else if (shmmsg_gpio->record_ln == 1)
  {
    shmmsg_gpio->record_ln = 0; // removes > from record_ln
    shmmsg_gpio->prev_ln = 1;   // adds > to prev_ln
  }

  // switches from record to prev mode and does not save any changes
  else if (shmmsg_gpio->record_ln == 2)
  {
    shmmsg_gpio->record_ln = 0; // removes > from record_ln and switches back to original time
    shmmsg_gpio->prev_ln = 1;   // adds > to prev_ln
    shmmsg_gpio->info_ln = 0;   // turns off info_ln
    if (previous_stat == 2)
    { // if previously stopped returns to that
      shmmsg_gpio->stat_ln = 2;
    }
    else
    { // else restarts recording
      shmmsg_gpio->stat_ln = 0;
    }
  }

  // switches from prev to stop mode
  else if (shmmsg_gpio->prev_ln == 1)
  {
    shmmsg_gpio->prev_ln = 0; // removes > from prev_ln
    shmmsg_gpio->stop_ln = 1; // adds > to stop_ln
  }
}

void pluspressed()
{
  // Puts device into config mode and adds to the delay
  if (shmmsg_gpio->delay_ln == 1)
  {
    previous_stat = shmmsg_gpio->stat_ln;
    shmmsg_gpio->stat_ln = 3; // puts it into config mode
    if (recordingwasstarted == 1)
    {
      shmmsg_gpio->killsignal = 1;
      kill(bosoncampid, SIGKILL);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      kill(bosonstorepid, SIGKILL);
      recordingwasstarted = 0;
      while (shmmsg_gpio->startsignal == 1)
      {
        // Waits for imprex to be killed
      }
      kill(impstorepid, SIGKILL);
    }
    shmmsg_gpio->delay_ln = 2;                          // changes delay to display config variable
    shmmsg_gpio->info_ln = 1;                           // displays press select to push
    shmmsg_gpio->delay_config = shmmsg_gpio->delay_sec; // Save current file variable as config variable
    shmmsg_gpio->delay_config = shmmsg_gpio->delay_config - shmmsg_gpio->delay_config % 60;
    shmmsg_gpio->delay_config = shmmsg_gpio->delay_config + 60; // add 1 minute to config variable
    needsrestart = 1;
  }

  // add an aditional minute to delay time
  else if (shmmsg_gpio->delay_ln == 2)
  {
    shmmsg_gpio->delay_config = shmmsg_gpio->delay_config + 60; // add 1 minute to config variable
  }

  // Puts device into config mode and adds to the record time
  if (shmmsg_gpio->record_ln == 1)
  {
    previous_stat = shmmsg_gpio->stat_ln;
    shmmsg_gpio->stat_ln = 3; // puts it into config mode
    if (recordingwasstarted == 1)
    {
      shmmsg_gpio->killsignal = 1;
      kill(bosoncampid, SIGKILL);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      kill(bosonstorepid, SIGKILL);
      recordingwasstarted = 0;
      while (shmmsg_gpio->startsignal == 1)
      {
        // Waits for imprex to be killed
      }
      kill(impstorepid, SIGKILL);
    }
    shmmsg_gpio->record_ln = 2;                           // changes record to display config variable
    shmmsg_gpio->info_ln = 1;                             // displays press select to push
    shmmsg_gpio->record_config = shmmsg_gpio->record_sec; // Save current file variable as config variable
    shmmsg_gpio->record_config = shmmsg_gpio->record_config - shmmsg_gpio->record_config % 60;
    shmmsg_gpio->record_config = shmmsg_gpio->record_config + 60; // add 1 minute to record config variable
    needsrestart = 1;
  }

  // add an aditional minute to record time
  else if (shmmsg_gpio->record_ln == 2)
  {
    shmmsg_gpio->record_config = shmmsg_gpio->record_config + 60; // add 1 minute to record config variable
  }
}

void minuspressed()
{
  // puts device into config mode and subtracts from the delay
  if (shmmsg_gpio->delay_ln == 1 && shmmsg_gpio->delay_sec >= 60)
  { // && minutes is > 0
    previous_stat = shmmsg_gpio->stat_ln;
    shmmsg_gpio->stat_ln = 3; // puts it into config mode
    if (recordingwasstarted == 1)
    {
      shmmsg_gpio->killsignal = 1;
      kill(bosoncampid, SIGKILL);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      kill(bosonstorepid, SIGKILL);
      recordingwasstarted = 0;
      while (shmmsg_gpio->startsignal == 1)
      {
        // Waits for imprex to be killed
      }
      kill(impstorepid, SIGKILL);
    }
    shmmsg_gpio->delay_ln = 2;                          // changes delay to display config variable
    shmmsg_gpio->info_ln = 1;                           // displays press select to push
    shmmsg_gpio->delay_config = shmmsg_gpio->delay_sec; // Save current file variable as config variable
    shmmsg_gpio->delay_config = shmmsg_gpio->delay_config - shmmsg_gpio->delay_config % 60;
    shmmsg_gpio->delay_config = shmmsg_gpio->delay_config - 60;
    needsrestart = 1;
  }

  // subtract an additional minute from delay time
  else if (shmmsg_gpio->delay_ln == 2 && shmmsg_gpio->delay_config >= 60)
  {                                                             //&& minutes is > 0
    shmmsg_gpio->delay_config = shmmsg_gpio->delay_config - 60; // subtract 1 minute from config variable
  }

  // puts device into config mode and subtracts from the record time
  if (shmmsg_gpio->record_ln == 1 && shmmsg_gpio->record_sec >= 60)
  { // && minutes is > 0
    previous_stat = shmmsg_gpio->stat_ln;
    shmmsg_gpio->stat_ln = 3; // puts it into config mode
    if (recordingwasstarted == 1)
    {
      shmmsg_gpio->killsignal = 1;
      kill(bosoncampid, SIGKILL);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      kill(bosonstorepid, SIGKILL);
      recordingwasstarted = 0;
      while (shmmsg_gpio->startsignal == 1)
      {
        // Waits for imprex to be killed
      }
      kill(impstorepid, SIGKILL);
    }
    shmmsg_gpio->record_ln = 2;                           // changes record to display config variable
    shmmsg_gpio->info_ln = 1;                             // displays press select to push
    shmmsg_gpio->record_config = shmmsg_gpio->record_sec; // Save current file variable as config variable
    shmmsg_gpio->record_config = shmmsg_gpio->record_config - shmmsg_gpio->record_config % 60;
    shmmsg_gpio->record_config = shmmsg_gpio->record_config - 60; // subtract 1 minute from config variable
    needsrestart = 1;
  }

  // subtract an additional minute from record time
  else if (shmmsg_gpio->record_ln == 2 && shmmsg_gpio->record_config >= 60)
  {                                                               //&& minutes is > 0
    shmmsg_gpio->record_config = shmmsg_gpio->record_config - 60; // subtract 1 minute from config variable
  }
}

int main()
{
  

  //****************************************

  // Interprocess Communication

  // ANTH = 2684
  int shmid_gpio = -1;
  while (shmid_gpio < 0) {
    shmid_gpio = shmget(SHM_KEY_GPIO, sizeof(struct shm_gpio), IPC_CREAT | 0644);
    if (shmid_gpio < 0)
    {
      std::cout << "IPC Init Failed\n"
                << std::endl;
      return shmid_gpio;
    }
  }
  
  shmmsg_gpio = (struct shm_gpio *)shmat(shmid_gpio, NULL, 0);
  if (shmmsg_gpio == (void *)-1)
  {
    std::cout << "IPC Attach Failed\n"
              << std::endl;
    return -1;
  }

  //****************************************

  shmmsg_gpio->stat_ln = 0;
  shmmsg_gpio->stop_ln = 1;
  shmmsg_gpio->delay_ln = 0;
  shmmsg_gpio->record_ln = 0;
  shmmsg_gpio->prev_ln = 0;
  shmmsg_gpio->info_ln = 0;
  previous_stat = 0;
  needsrestart = 0;
  int menustillpressed = 0;
  int selstillpressed = 0;

  gpio_export(251);
  gpio_export(250);
  gpio_export(248);
  gpio_export(354);
  gpio_set_direction(251, "in");
  gpio_set_direction(250, "in");
  gpio_set_direction(248, "in");
  gpio_set_direction(354, "in");
  remove_all("/home/anthony/ARGH/src/Boson_320");
  remove_all("/data/Imperx");

  while (1)
  {
    recordingwasstarted = 0;
    needsrestart = 0;

    fstream recordfile;
    string recordstring;
    recordfile.open("../BosonUSB/recordingtime.txt", ios::in);
    if (recordfile.is_open())
    {
      getline(recordfile, recordstring);
      recordfile.close();
    }
    else
    {
      printf("Error opening record time file.");
    }
    printf("record2 STOI \n");
    shmmsg_gpio->record_sec = stoi(recordstring);
    cout << "record_sec: " << shmmsg_gpio->record_sec << endl;

    string delaytime;

    fstream delayfile;
    delayfile.open("../BosonUSB/timeddelay.txt", ios::in);
    if (delayfile.is_open())
    {
      getline(delayfile, delaytime);
      delayfile.close();
    }
    printf("delay 2 STOI \n");
    shmmsg_gpio->delay_sec = stoi(delaytime);
    ofstream rewritedelay;

    if (shmmsg_gpio->record_sec == 0)
    {
      shmmsg_gpio->stat_ln = 2;
      needsrestart = 1;
    }

    // This section is used in the case that record seconds was 0 when this started
    while (shmmsg_gpio->stat_ln >= 2)
    {
      if (gpio_get_state(251) == 1 && menustillpressed == 0)
      {
        menupressed();
        menustillpressed = 1;
      }
      else if (gpio_get_state(251) == 0)
      {
        menustillpressed = 0;
      }
      if (gpio_get_state(250) == 1 && selstillpressed == 0)
      {
        selectpressed();
        selstillpressed = 1;
      }
      else if (gpio_get_state(250) == 0)
      {
        selstillpressed = 0;
      }
      if (gpio_get_state(248) == 1)
      {
        pluspressed();
      }
      if (gpio_get_state(354) == 1)
      {
        minuspressed();
      }
      printf("%i,%i,%i,%i,%i,%i delaytime: %i recordingtime: %i needsrestart: %i configdel: %i configrec: %i\n", shmmsg_gpio->stat_ln, shmmsg_gpio->stop_ln, shmmsg_gpio->delay_ln, shmmsg_gpio->record_ln, shmmsg_gpio->prev_ln, shmmsg_gpio->info_ln, shmmsg_gpio->delay_sec, shmmsg_gpio->record_sec, needsrestart, shmmsg_gpio->delay_config, shmmsg_gpio->record_config);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    int run = 0;
    while (shmmsg_gpio->delay_sec > 0 && needsrestart == 0)
    {
      run = 0;
      while (shmmsg_gpio->stat_ln >= 2 || run < 5)
      {
        if (gpio_get_state(251) == 1 && menustillpressed == 0)
        {
          menupressed();
          menustillpressed = 1;
        }
        else if (gpio_get_state(251) == 0)
        {
          menustillpressed = 0;
        }
        if (gpio_get_state(250) == 1 && selstillpressed == 0)
        {
          selectpressed();
          selstillpressed = 1;
        }
        else if (gpio_get_state(250) == 0)
        {
          selstillpressed = 0;
        }
        if (gpio_get_state(248) == 1)
        {
          pluspressed();
        }
        if (gpio_get_state(354) == 1)
        {
          minuspressed();
        }
        run++;
        printf("%i,%i,%i,%i,%i,%i delaytime: %i recordingtime: %i needsrestart: %i configdel: %i configrec: %i\n", shmmsg_gpio->stat_ln, shmmsg_gpio->stop_ln, shmmsg_gpio->delay_ln, shmmsg_gpio->record_ln, shmmsg_gpio->prev_ln, shmmsg_gpio->info_ln, shmmsg_gpio->delay_sec, shmmsg_gpio->record_sec, needsrestart, shmmsg_gpio->delay_config, shmmsg_gpio->record_config);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
      }
      if (needsrestart == 0)
      {
        shmmsg_gpio->delay_sec = shmmsg_gpio->delay_sec - 1;
        rewritedelay.open("../BosonUSB/timeddelay.txt", ios::out | ios::trunc);
        rewritedelay << shmmsg_gpio->delay_sec << endl;
        rewritedelay.close();
        //  printf("delay time remaining: %i \n",delay_sec);
      }
    }

    if (needsrestart == 0 && shmmsg_gpio->stat_ln == 0 && shmmsg_gpio->record_sec > 0)
    {
      shmmsg_gpio->stat_ln = 4;
      shmmsg_gpio->startsignal = 1;

      // char *flightstringc = malloc(sizeof(char) * 6);
      string flightstring;

      fstream flightfile;
      flightfile.open("../BosonUSB/flightnumber.txt", ios::in);
      if (flightfile.is_open())
      {
        getline(flightfile, flightstring);
        flightfile.close();
      }
      else
      {
        printf("Error opening flight number\n");
      }

      // flightstringc = flightstring.c_str();
      printf("%s \n", flightstring.c_str());
      printf("FLINTSTOI\n");
      int flint = stoi(flightstring);
      flint = flint + 1;
      string updatedflight;
      stringstream flints;
      flints << flint;
      flints >> updatedflight;
      flints.str("");

      ofstream newflight;
      newflight.open("../BosonUSB/flightnumber.txt", ios::out | ios::trunc);
      newflight << updatedflight << endl;
      newflight.close();

      bosoncampid = fork();

      if (bosoncampid == 0)
      {
        callbosoncam();
        exit(0);
      }

      bosonstorepid = fork();
      time_t currtime = time(nullptr);

      if (bosonstorepid == 0)
      {
        callbosonstore(currtime);
        exit(0);
      }

      impstorepid = fork();
      if (impstorepid == 0)
      {
        callimpstore(currtime);
        exit(0);
      }

      while (shmmsg_gpio->startsignal == 1)
      {
        // loops wating for imprex to start
      }

      recordingwasstarted = 1;
      // THIS IS WHERE RECORDING STUFF WOULD GO
      shmmsg_gpio->stat_ln = 1;
    }

    ofstream rewriterecord;
    while (shmmsg_gpio->record_sec > 0 && needsrestart == 0)
    {
      run = 0;
      while (shmmsg_gpio->stat_ln >= 2 || run < 5)
      {
        if (gpio_get_state(251) == 1 && menustillpressed == 0)
        {
          menupressed();
          menustillpressed = 1;
        }
        else if (gpio_get_state(251) == 0)
        {
          menustillpressed = 0;
        }
        if (gpio_get_state(250) == 1 && selstillpressed == 0)
        {
          selectpressed();
          selstillpressed = 1;
        }
        else if (gpio_get_state(250) == 0)
        {
          selstillpressed = 0;
        }
        if (gpio_get_state(248) == 1)
        {
          pluspressed();
        }
        if (gpio_get_state(354) == 1)
        {
          minuspressed();
        }
        run++;
        printf("%i,%i,%i,%i,%i,%i delaytime: %i recordingtime: %i needsrestart: %i configdel: %i configrec: %i\n", shmmsg_gpio->stat_ln, shmmsg_gpio->stop_ln, shmmsg_gpio->delay_ln, shmmsg_gpio->record_ln, shmmsg_gpio->prev_ln, shmmsg_gpio->info_ln, shmmsg_gpio->delay_sec, shmmsg_gpio->record_sec, needsrestart, shmmsg_gpio->delay_config, shmmsg_gpio->record_config);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
      }
      if (needsrestart == 0)
      {
        shmmsg_gpio->record_sec = shmmsg_gpio->record_sec - 1;
        rewriterecord.open("../BosonUSB/recordingtime.txt", ios::out | ios::trunc);
        rewriterecord << shmmsg_gpio->record_sec << endl;
        rewriterecord.close();
        //  printf("recording time remaining: %i \n",record_sec);
      }
    }

    printf("restarting\n");
    if (recordingwasstarted == 1)
    {
      shmmsg_gpio->killsignal = 1;
      kill(bosoncampid, SIGKILL);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      kill(bosonstorepid, SIGKILL);
      recordingwasstarted = 0;
      while (shmmsg_gpio->startsignal == 1)
      {
        // Waits for imprex to be killed
      }
      kill(impstorepid, SIGKILL);
    }

    // Will need signal handler and to make sure everything can just simply restart
  }
}
