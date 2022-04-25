#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <sys/types.h>
#include <sstream>

using namespace std;
int Storage(char* oldname, int camnum, char* timestamp){
printf("STORAGE FOR PIC: %s\n", oldname);
char newname[100] = "/media/anthony/CapstoneSSD/";
char camerastr[7] = "Camera";
char cstr[8];
sprintf(cstr, "%d", camnum);
strcat(camerastr,cstr);
strcat(newname,camerastr);
strcat(newname,"/");
DIR* dir = opendir(newname);
if(dir){
        closedir(dir);
}
else{
char maker[100] = "mkdir ";
strcat(maker,newname);
system(maker);
}
char cmdstr[250];
char oldmoved[250];
strcpy(cmdstr, "cp");
strcat(cmdstr, " ");
strcat(cmdstr, oldname);
strcat(cmdstr, " ");
strcat(cmdstr, newname);
system(cmdstr);
remove(oldname);
strcpy(oldmoved,newname);
strcat(oldmoved,oldname + 33); //THIS WILL NEED CHANGING
strcat(newname,camerastr);
strcat(newname,"_");
strcat(newname,timestamp);
strcat(newname,".tiff");
//printf("OLD: %s NEW: %s \n",oldmoved,newname);
rename(oldmoved,newname);
return 3;
}







int main(int argc, char *argv[]){

int originaltime = stoi(argv[1]);
struct stat originalstats;
struct stat currentfilestats;
const char filename[100] = "/home/anthony/ARGH/src/Boson_320/Boson_320_agc_";
char othername[100] = "/home/anthony/ARGH/src/Boson_320/Boson_320_raw16_";
char tiff[6] = ".tiff";
char fullname[100];
int picnum = 0;
char temptimestampstart[20] = "PicNumber";
char temptimestamp[20];
char cstr[8];
char fullremoval[100];
int stored = 0;
std::this_thread::sleep_for(std::chrono::seconds(2));
int firstfile = 0;
int timepiece;
//char timestr[20];
int seconds = 0;
int minutes = 0;
int hours = 0;
char mst[10];
char min[10];
char sec[10];
char hr[10];


printf("Storage function started\n");




while(1){
 //std::this_thread::sleep_for(std::chrono::milliseconds(10));

 strcpy(fullname,filename);
 sprintf(cstr,"%d",picnum);
 strcat(fullname,cstr);
 strcat(fullname,tiff);

printf("ABOUT TO LOOK FOR THIS: %s\n",fullname);
 while(stat(fullname,&currentfilestats) != 0){
//printf("looping in here cause it works \n");
 }

//std::this_thread::sleep_for(std::chrono::milliseconds(10));
// if(firstfile == 0){
//	originalstats = currentfilestats;
//	firstfile = 1;
 //}
timepiece = currentfilestats.st_mtime * 1000 + currentfilestats.st_mtim.tv_nsec / 1000000 - originaltime;//originalstats.st_mtime * 1000 + originalstats.st_mtim.tv_nsec / 1000000;
//sprintf(timestr,"%d",timepiece);

if(timepiece >= 1000){
 seconds = timepiece / 1000;
 timepiece = timepiece - (1000 * (timepiece / 1000));
}
if(seconds>=60){
 minutes = seconds / 60;
 seconds = seconds - 60 * (seconds / 60);
}
if(minutes>=60){
 hours = minutes / 60;
 minutes = minutes - 60 * (minutes / 60);
}

sprintf(mst,"%d",timepiece);
sprintf(sec,"%d",seconds);
sprintf(min,"%d",minutes);
sprintf(hr,"%d",hours);


string flightstring;

  
  fstream flightfile;
  flightfile.open("/home/anthony/ARGH/flightnumber.txt",ios::in);
  if(flightfile.is_open()){
        getline(flightfile,flightstring);
        flightfile.close();
  }
  else{
  printf("Error opening flight number\n");
  }

printf("2: %s\n",fullname);

 strcpy(temptimestamp, "Flight");
 strcat(temptimestamp, flightstring.c_str());
 strcat(temptimestamp, "_");
 strcat(temptimestamp, hr);
 strcat(temptimestamp, ":");
 strcat(temptimestamp, min);
 strcat(temptimestamp, ":");
 strcat(temptimestamp, sec);
 strcat(temptimestamp, ".");
 strcat(temptimestamp, mst);
 strcat(temptimestamp, "_");
 //strcat(temptimestamp, temptimestampstart);
 //strcat(temptimestamp,cstr);
 //HERE IS STORAGE FUNCTION
 printf("3: %s\n",fullname);

 Storage(fullname, 1, temptimestamp);
 printf("4: %s\n",fullname);

 strcpy(fullremoval,othername);
 strcat(fullremoval,cstr);
 strcat(fullremoval,tiff);
 remove(fullremoval);
 picnum++;
 stored++;
 printf("5: %s\n",fullname);

}


printf("YOOOOOOO\n");


return 0;
}
