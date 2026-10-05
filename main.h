// main.h
// Josh Brake
// jbrake@hmc.edu
// 10/31/22

// modified by Katie Latvakoski 10/4/2026

#ifndef MAIN_H
#define MAIN_H

#include "STM32L432KC.h"
#include <stm32l432xx.h>

///////////////////////////////////////////////////////////////////////////////
// Custom defines
///////////////////////////////////////////////////////////////////////////////

#define A_input PB0 // connected to line 0 interrupt 
#define B_input PB6
#define DELAY_TIM TIM2

#endif // MAIN_H