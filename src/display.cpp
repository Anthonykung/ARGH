/**
 * @file display.cpp
 * @author Anthony Kung (hi@anth.dev)
 * @name Waveshare e-Paper Interface
 * @brief This file is responsible for managing e-Paper display operation
 * @version 0.0.1
 * @date 2022-04-19
 * 
 * @copyright Copyright (c) 2022 Anthony Kung <hi@anth.dev>
 * @license Apache-2.0
 * 
 */

// Include Internal Libraries
#include "display.hpp"

// Control Variables
// int stat_ln
// int stop_ln
// int delay_ln
// int record_ln
// int prev_ln
// int info_ln

int display_init() {
  // Initialize the display
  if(DEV_Module_Init()!=0){
    return -1;
  }
  EPD_4IN2_Init();
  EPD_4IN2_Clear();
  DEV_Delay_ms(500);

  //Create a new image cache
  UBYTE *BlackImage;
  /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
  UWORD Imagesize = ((EPD_4IN2_WIDTH % 8 == 0)? (EPD_4IN2_WIDTH / 8 ): (EPD_4IN2_WIDTH / 8 + 1)) * EPD_4IN2_HEIGHT;
  if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
      printf("Failed to apply for black memory...\r\n");
      return -1;
  }
  Paint_NewImage(BlackImage, EPD_4IN2_WIDTH, EPD_4IN2_HEIGHT, 0, WHITE);

  Paint_SelectImage(BlackImage);
  Paint_Clear(WHITE);
  return 0;
}

int display_exit() {
  DEV_Module_Exit();
}

void write_display(string line1, string line2, string line3, string line4, string line5) {
  // Write the message to the display
  Paint_Clear(WHITE);
  EPD_4IN2_Clear();
  Paint_DrawString_EN(10,  0, line1.c_str(), &Font16, BLACK, WHITE);
  Paint_DrawString_EN(10, 20, line2.c_str(), &Font16, BLACK, WHITE);
  Paint_DrawString_EN(10, 40, line3.c_str(), &Font16, BLACK, WHITE);
  Paint_DrawString_EN(10, 60, line4.c_str(), &Font16, BLACK, WHITE);
  Paint_DrawString_EN(10, 80, line5.c_str(), &Font16, BLACK, WHITE);

  EPD_4IN2_Display(BlackImage);
	DEV_Delay_ms(2000);
  DEV_Module_Exit();
}