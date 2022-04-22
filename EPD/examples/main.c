/**
 * @file main.c
 * @author Anthony Kung (hi@anth.dev)
 * @name e-Paper Display Executable
 * @brief This file controls the e-Paper Display with Interprocess Communcation
 * @version 0.0.1
 * @date 2022-04-18
 * 
 * Modified From Waveshare e-Paper Display Examples
 * 
 * @copyright Copyright (c) 2022 Anthony Kung <hi@anth.dev>
 * @license Apache-2.0
 * 
 */

#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include "EPD_Test.h"   //Examples
#include "EPD_4in2.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#define SHM_KEY 0x2684
#define SHM_SIZE 1000

struct shmstu {
  int request;
  int busy;
  char cmd[SHM_SIZE];
  int num_line;
  char* msg[SHM_SIZE];
};

void print_current_time_with_ms() {
    struct timespec spec;
    clock_gettime(0, &spec);
    printf("Current time: %ld.%ld\n", spec.tv_sec, spec.tv_nsec);
}

int main(int argc, char *argv[]) {

  //****************************************

  // Interprocess Communication

  // ANTH = 2684
  struct shmstu *shmmsg;
  int shmid = shmget(SHM_KEY, sizeof(struct shmstu), IPC_CREAT | 0644);
  if (shmid < 0) {
    printf("IPC Init Failed\n");
    return shmid;
  }
  shmmsg = shmat(shmid, NULL, 0);
  if (shmmsg == (void *) -1) {
    printf("IPC Init Failed\n");
    return -1;
  }

  //****************************************

  //Create a new image cache
  UBYTE *BlackImage;
  /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
  UWORD Imagesize = ((EPD_4IN2_WIDTH % 8 == 0)? (EPD_4IN2_WIDTH / 8 ): (EPD_4IN2_WIDTH / 8 + 1)) * EPD_4IN2_HEIGHT;
  if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
      printf("Failed to apply for black memory...\r\n");
      return -1;
  }
  Paint_NewImage(BlackImage, EPD_4IN2_WIDTH, EPD_4IN2_HEIGHT, 0, WHITE);

  int exit = 0;

  while (exit == 0) {

    char user_input[27] = "";

    printf("Enter command: \n");
    scanf("%s", user_input);

    // Only process if request is activated or user input is not empty
    if (shmmsg->request == 1 || strcmp(user_input, "") != 0) {
      printf("Request\n");
      shmmsg->busy = 1;
      shmmsg->request = 0;

      if (strcmp(shmmsg->cmd, "init") == 0 || strcmp(user_input, "init") == 0) {
        printf("Initializing...\n");
        if(DEV_Module_Init()!=0){
          return -1;
        }
        EPD_4IN2_Init_Fast();

        Paint_SelectImage(BlackImage);

        Paint_Clear(WHITE);
        Paint_DrawString_EN(10,   0, "Initializing...", &Font24, WHITE, BLACK);
        Paint_DrawString_EN(10,  20, "Please wait...", &Font24, WHITE, BLACK);

        EPD_4IN2_Display(BlackImage);
        EPD_4IN2_Sleep();
      }

      else if (strcmp(shmmsg->cmd, "refresh") == 0 || strcmp(user_input, "refresh") == 0) {
        printf("Refreshing...\n");
        if(DEV_Module_Init()!=0){
          return -1;
        }
        EPD_4IN2_Init_Fast();

        Paint_SelectImage(BlackImage);

        Paint_Clear(WHITE);
        Paint_DrawString_EN(10,   0, "Refreshing...", &Font24, WHITE, BLACK);
        Paint_DrawString_EN(10,  20, "Please wait...", &Font24, WHITE, BLACK);
        Paint_DrawString_EN(10,  40, "Please wait...", &Font24, WHITE, BLACK);

        EPD_4IN2_Display(BlackImage);
        EPD_4IN2_Sleep();
      }

      else if (strcmp(shmmsg->cmd, "write") == 0) {
        printf("Writing...\n");
      
        printf("Enter text: \n");
        scanf("%s", user_input);

        EPD_4IN2_Init_Partial();
        EPD_4IN2_Clear();
        EPD_4IN2_PartialDisplay(0, 0, 400, 30, BlackImage);
        Paint_Clear(WHITE);
        Paint_DrawString_EN(10,  0, user_input, &Font24, WHITE, BLACK);
        print_current_time_with_ms();
        Paint_DrawString_EN(10, 20, user_input, &Font24, WHITE, BLACK);

        EPD_4IN2_PartialDisplay(0, 0, 400, 300, BlackImage);
        EPD_4IN2_Sleep();

        EPD_4IN2_Init_Partial();
        Paint_DrawString_EN(10, 40, "Write Complete", &Font24, WHITE, BLACK);
        print_current_time_with_ms();
        Paint_DrawString_EN(10, 60, user_input, &Font24, WHITE, BLACK);

        EPD_4IN2_PartialDisplay(0, 35, 400, 80, BlackImage);
        EPD_4IN2_Sleep();
      }

      else if (strcmp(shmmsg->cmd, "exit") == 0 || strcmp(user_input, "exit") == 0) {
        printf("Exiting...\n");
        EPD_4IN2_Init_Fast();
        EPD_4IN2_Clear();
        Paint_Clear(WHITE);

        // Clean Up e-Paper Display
        DEV_Module_Exit();

        // Detach SHM
        shmdt(shmmsg);

        // Exit Program
        exit = 1;
	break;
      }
      strcpy(user_input, "");
      shmmsg->busy = 0;
    }
  }

  return 0;
}
