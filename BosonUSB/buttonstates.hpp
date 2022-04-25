

// Guard
#ifndef BUTTONSTATES_HPP
#define BUTTONSTATES_HPP

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

extern int stat_ln;
extern int stop_ln;
extern int delay_ln;
extern int record_ln;
extern int prev_ln;
extern int info_ln;
extern int previous_stat;
extern int delay_config;
extern int record_config;
extern int needsrestart;
extern int delay_sec;
extern int record_sec;
extern pid_t bosonstorepid;
extern pid_t bosoncampid;
extern int recordingwasstarted;

//using std::string;
//using std::ofstream;
//using std::ifstream;
//using std::fstream;
// using namespace std;
// using namespace std::experimental::filesystem::v1;

int button_main();

#endif