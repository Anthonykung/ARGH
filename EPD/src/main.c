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

#define SHM_KEY 0x9373
#define STR_SIZE 1000

FILE *fp;

struct shm_wepd {
  int request;
  int busy;
  char cmd[STR_SIZE];
  int num_line;
  char msg[STR_SIZE];
};

void print_current_time_with_ms() {
    struct timespec spec;
    clock_gettime(0, &spec);
    printf("Current time: %ld.%ld\n", spec.tv_sec, spec.tv_nsec);
}

void  Handler(int signo) {
    //System Exit
    printf("\r\nHandler:exit\r\n");
    DEV_Module_Exit();

    exit(0);
}

int main(int argc, char *argv[]) {
  signal(SIGINT, Handler);

  //****************************************

  // Interprocess Communication

  // ANTH = 2684
  struct shm_wepd *shmmsg_wepd;
  int shmid_wepd = shmget(SHM_KEY, sizeof(struct shm_wepd), IPC_CREAT | 0644);
  if (shmid_wepd < 0) {
    printf("IPC Init Failed\n");
    return shmid_wepd;
  }
  shmmsg_wepd = shmat(shmid_wepd, NULL, 0);
  if (shmmsg_wepd == (void *) -1) {
    printf("IPC Init Failed\n");
    return -1;
  }
  shmmsg_wepd->busy = 1;
  shmmsg_wepd->request = 0;


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

  fp = fopen("./display.log", "a");
  fprintf(fp, "Busy: %d\n", shmmsg_wepd->busy);
  fprintf(fp, "Request: %d\n", shmmsg_wepd->request);
  fprintf(fp, "Line: %d\n", shmmsg_wepd->num_line);
  fprintf(fp, "CMD: %s\n", shmmsg_wepd->cmd);
  fprintf(fp, "MSG: %s\n", shmmsg_wepd->msg);
  fprintf(fp, "\n");
  fclose(fp);

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
  
  shmmsg_wepd->busy = 0;

  fp = fopen("./display.log", "a");
  fprintf(fp, "Busy: %d\n", shmmsg_wepd->busy);
  fprintf(fp, "Request: %d\n", shmmsg_wepd->request);
  fprintf(fp, "Line: %d\n", shmmsg_wepd->num_line);
  fprintf(fp, "CMD: %s\n", shmmsg_wepd->cmd);
  fprintf(fp, "MSG: %s\n", shmmsg_wepd->msg);
  fprintf(fp, "\n");
  fclose(fp);

  // printf("Initializing...\n");
  // if(DEV_Module_Init()!=0){
  //   return -1;
  // }
  // EPD_4IN2_Init_Fast();

  // Paint_SelectImage(BlackImage);

  // Paint_Clear(WHITE);
  // Paint_DrawString_EN(10,   0, "Initializing...", &Font24, WHITE, BLACK);
  // Paint_DrawString_EN(10,  20, "Please wait...", &Font24, WHITE, BLACK);

  // EPD_4IN2_Display(BlackImage);
  // EPD_4IN2_Sleep();


  while (exit == 0) {

    char user_input[27] = "";

    printf("Enter command: \n");
    scanf("%s", user_input);

    // Only process if request is activated or user input is not empty
    if (shmmsg_wepd->request == 1 || strcmp(user_input, "") != 0) {
  
      fp = fopen("./display.log", "a");
      fprintf(fp, "Busy: %d\n", shmmsg_wepd->busy);
      fprintf(fp, "Request: %d\n", shmmsg_wepd->request);
      fprintf(fp, "Line: %d\n", shmmsg_wepd->num_line);
      fprintf(fp, "CMD: %s\n", shmmsg_wepd->cmd);
      fprintf(fp, "MSG: %s\n", shmmsg_wepd->msg);
      fprintf(fp, "\n");
      fclose(fp);

      printf("Request\n");
      shmmsg_wepd->busy = 1;
      shmmsg_wepd->request = 0;

      if (strcmp(shmmsg_wepd->cmd, "init") == 0 || strcmp(user_input, "init") == 0) {
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

      else if (strcmp(shmmsg_wepd->cmd, "refresh") == 0 || strcmp(user_input, "refresh") == 0) {
        printf("Refreshing...\n");
        if(DEV_Module_Init()!=0){
          return -1;
        }
        EPD_4IN2_Init_Fast();

        Paint_SelectImage(BlackImage);

        EPD_4IN2_Clear();
        Paint_Clear(WHITE);
        Paint_DrawString_EN(10,   0, "Refreshing...", &Font24, WHITE, BLACK);
        Paint_DrawString_EN(10,  20, "Please wait...", &Font24, WHITE, BLACK);

        EPD_4IN2_Display(BlackImage);
        EPD_4IN2_Sleep();
      }

      else if (strcmp(user_input, "test-write") == 0) {
        printf("Writing...\n");

        EPD_4IN2_Init_Partial();
        EPD_4IN2_Clear();
        EPD_4IN2_PartialDisplay(0, 0, 400, 30, BlackImage);
        Paint_Clear(WHITE);
        Paint_DrawString_EN(10,  0, user_input, &Font24, WHITE, BLACK);
        print_current_time_with_ms();
        Paint_DrawString_EN(10, 20, user_input, &Font24, WHITE, BLACK);

        EPD_4IN2_PartialDisplay(0, 0, 400, 300, BlackImage);
        print_current_time_with_ms();
        // EPD_4IN2_Sleep();

        EPD_4IN2_Init_Partial();
        Paint_DrawString_EN(10, 40, "Write Complete", &Font24, WHITE, BLACK);
        print_current_time_with_ms();
        Paint_DrawString_EN(10, 60, user_input, &Font24, WHITE, BLACK);

        EPD_4IN2_PartialDisplay(0, 35, 400, 80, BlackImage);
        EPD_4IN2_Sleep();
        print_current_time_with_ms();
      }

      else if (strcmp(shmmsg_wepd->cmd, "write") == 0) {
        Paint_ClearWindows(0, shmmsg_wepd->num_line * 20, 400, shmmsg_wepd->num_line * 20 + 20, WHITE);
        Paint_DrawString_EN(10, shmmsg_wepd->num_line * 20, shmmsg_wepd->msg, &Font24, WHITE, BLACK);
        EPD_4IN2_PartialDisplay(0, shmmsg_wepd->num_line * 20, 400, shmmsg_wepd->num_line * 20 + 20, BlackImage);
        strcpy(shmmsg_wepd->msg, "");
        strcpy(shmmsg_wepd->cmd, "");
        shmmsg_wepd->num_line = 0;
      }

      else if (strcmp(shmmsg_wepd->cmd, "start write") == 0) {
        EPD_4IN2_Init_Partial();
        EPD_4IN2_Clear();
        EPD_4IN2_PartialDisplay(0, 0, 400, 300, BlackImage);
        Paint_ClearWindows(0, 0, 400, 300, WHITE);
      }

      else if (strcmp(shmmsg_wepd->cmd, "end write") == 0) {
        EPD_4IN2_Sleep();
      }

      else if (strcmp(user_input, "write") == 0) {
        printf("Writing...\n");

        EPD_4IN2_Init_Partial();
        EPD_4IN2_Clear();
        EPD_4IN2_PartialDisplay(0, 0, 400, 300, BlackImage);
        Paint_ClearWindows(0, 0, 400, 300, WHITE);
      
        while (strcmp(user_input, "EXIT()") != 0) {
          int num_line = 0;
          printf("Exit with `EXIT()`\n");
          printf("Enter Line Number: \n");
          scanf("%d", &num_line);

          printf("Enter text: \n");
          scanf("%s", user_input);

          print_current_time_with_ms();
          Paint_ClearWindows(0, num_line * 20, 400, num_line * 20 + 20, WHITE);

          Paint_DrawString_EN(10, num_line * 20, user_input, &Font24, WHITE, BLACK);

          EPD_4IN2_PartialDisplay(0, num_line * 20, 400, num_line * 20 + 20, BlackImage);
          print_current_time_with_ms();
        }
        EPD_4IN2_Sleep();
      }

      else if (strcmp(shmmsg_wepd->cmd, "exit") == 0 || strcmp(user_input, "exit") == 0) {
        printf("Exiting...\n");
        EPD_4IN2_Init_Fast();
        EPD_4IN2_Clear();
        Paint_Clear(WHITE);

        // Clean Up e-Paper Display
        DEV_Module_Exit();

        // Detach SHM
        shmdt(shmmsg_wepd);

        // Exit Program
        exit = 1;
        break;
      }
    }
    strcpy(user_input, "");
    shmmsg_wepd->busy = 0;
  }

  return 0;
}
