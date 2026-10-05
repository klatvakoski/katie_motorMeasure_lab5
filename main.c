
#include <stdio.h>
#include "main.h"


int main(void) {
  // Enable A-input and B-input in order to get the interupts 
    gpioEnable(GPIO_PORT_B);
    pinMode(A_input, GPIO_INPUT);
  // create a counter to keep track of how many pulses have been counted
    int pulse_count;             

  
  // 1. Enable SYSCFG clock domain in RCC
  RCC->APB2ENR |= (1 << 0); // SYSCFGEN
  // 2. Configure EXTICR for the input button interrupt
  // EXTI0 is bits 2:0 of EXTICR1 (EXTICR[0] in C). Port B is 0b001, so we select 001.
  SYSCFG->EXTICR[0] &= (0b001);
  // EXTI6 is bits 10:8 of EXTICR2 (EXTICR[1] in C). Port B is 0b001, so we select 001.
  SYSCFG->EXTICR[1] &= (0b001);

  // Enable interrupts globally
  __enable_irq();

  // Configure interrupt for falling edge of GPIO pin for button
  EXTI->IMR1 |= (1 << gpioPinOffset(A_input));   // 1. Configure mask bit for A
  EXTI->IMR1 |= (1 << gpioPinOffset(B_input));   // 1. Configure mask bit for B
  EXTI->RTSR1 |= (1 << gpioPinOffset(A_input)); // 2. Enable rising edge trigger for A 
  EXTI->RTSR1 |= (1 << gpioPinOffset(B_input)); // 2. Enable rising edge trigger for B
  EXTI->FTSR1 |= (1 << gpioPinOffset(A_input));  // 3. Enable falling edge trigger for A
  EXTI->FTSR1 |= (1 << gpioPinOffset(B_input));  // 3. Enable falling edge trigger for B
  NVIC_EnableIRQ(EXTI0_IRQn);                    // call func to turn on EXTI0 interrupt on NVIC_ISER
  NVIC_EnableIRQ(EXTI9_5_IRQn);                  // call func to turn on EXTI9_5 interrupt on NVIC_ISER

}
// IF pulse_count is positive, then it is clockwise, if negative then ccw

// A interrupt func
void EXTI0_IRQHandler(int pulse_count){
    // Check for clockwise rotation -- if A is high and B is low 
    if (digitalRead(A_input) & ~digitalRead(B_input)) {
      // clear the interrupt (NB: Write 1 to reset)
      EXTI->PR1 = (1 << gpioPinOffset(A_input));
      pulse_count = pulse_count + 1; 
      }

    // check for counter clockwise rotation -- if A is high and B is high
    else if(digitalRead(A_input) & digitalRead(B_input)) {
      // clear the interrupt (NB: Write 1 to reset)
      EXTI->PR1 = (1 << gpioPinOffset(A_input));
      pulse_count = pulse_count - 1;
      }
    

}

    (EXTI->PR1 & (1 << gpioPinOffset(A_input)))
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 = (1 << gpioPinOffset(BUTTON_PIN));

  }


// B interrupt func
void EXTI9_5_IRQHandler(void){
  int pulse_count = 0; 
  // first make sure that the interrupt was from B_input (bc line 6 is shared with 5-9)
  if (EXTI->PR1 & (1 << gpioPinOffset(A_input))) {
    //check for clockwise rotation -- if B is high and A is high
    if (digitalRead(A_input) & digitalRead(B_input)) {
      // clear the interrupt (NB: Write 1 to reset)
      EXTI->PR1 = (1 << gpioPinOffset(B_input));
      pulse_count = pulse_count + 1; 
      }

    // check for counter clockwise rotation -- if A is low and B is high
    else if(digitalRead(A_input) & digitalRead(B_input)) {
      // clear the interrupt (NB: Write 1 to reset)
      EXTI->PR1 = (1 << gpioPinOffset(A_input));
      pulse_count = pulse_count - 1;
      }
  }

  
  }


    

