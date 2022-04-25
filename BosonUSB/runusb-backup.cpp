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

using namespace std;
int Storage(char* oldname, int camnum, char* timestamp){
char newname[100] = "/media/caden/CapstoneSSD/";
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
strcat(oldmoved,oldname);
strcat(newname,camerastr);
strcat(newname,"_");
strcat(newname,timestamp);
rename(oldmoved,newname);
return 3;
}







main(){


pid_t childpid = fork();

if(childpid == 0){
char commandboson[] = "./BosonUSB f t1000";
system(commandboson);
printf("END OF CHILD\n");
exit(0);
}


char filename[100] = "/home/caden/Desktop/BosonUSB/Boson_320/Boson_320_agc_";
char othername[100] = "/home/caden/Desktop/BosonUSB/Boson_320/Boson_320_raw16_";
char tiff[6] = ".tiff";
char fullname[100];
int picnum = 0;
char temptimestampstart[20] = "PicNumber";
char temptimestamp[20];
char cstr[8];
char fullremoval[100];
int stored = 0;
std::this_thread::sleep_for(std::chrono::seconds(2));
while(stored < 1000){
 std::this_thread::sleep_for(std::chrono::milliseconds(10));
 strcpy(fullname,filename);
 sprintf(cstr,"%d",picnum);
 strcat(fullname,cstr);
 strcat(fullname,tiff);
 strcpy(temptimestamp,temptimestampstart);
 strcat(temptimestamp,cstr);
 //HERE IS STORAGE FUNCTION
 Storage(fullname, 2, temptimestamp);
 strcpy(fullremoval,othername);
 strcat(fullremoval,cstr);
 strcat(fullremoval,tiff);
 remove(fullremoval);
 picnum++;
 stored++;
}


printf("YOOOOOOO\n");


return 0;
}
