
#include <stdio.h>
#include "main.h"

volatile int32_t pulse_count = 0; 

int main(void) {
  volatile int32_t count_latch = 0; 
  float pulse_per_revolution = 408; 

  // Enable A-input (from the sensor at A) and B-input (from the sensor at B) in order to get the interupts on
  // PB0 and PB6 respectively
  sensorInputEnable();
    
  // create a counter to keep track of how many pulses have been counted
  initCounter(); 

  // setup SYSCFG, EXTICR, mask bits, rising edges, and falling edges for A & B 
  interruptSetup(); 

  // Enable interrupts globally
  __enable_irq();

  // enable interrupts specifically for PB0 and PB6 through NVIC_ISER
  a_b_interruptEnable();

  while(1){
    delay_millis(DELAY_TIM,1000);
    count_latch = pulse_count;  // grab current pulse count value
    pulse_count = 0; // reset pulse_count
    float report = count_latch/pulse_per_revolution; 
    printf("Speed: %f rev/s! \n", report);
  }

}
// IF pulse_count is positive, then it is clockwise, if negative then ccw

// A interrupt func
void EXTI0_IRQHandler(void){ 
    // clear the interrupt (NB: Write 1 to reset)
    EXTI->PR1 = (1 << gpioPinOffset(A_input));

    // set digital inputs
    int A_in = digitalRead(A_input); 
    int B_in = digitalRead(B_input); 

    // Check for clockwise rotation -- if A is high and B is low 
    if (A_in & ~B_in) {
      pulse_count = pulse_count + 1; 
      togglePin(A_output);
      }

    // check for counter clockwise rotation -- if A is high and B is high
    else if(A_in & B_in) {
      // clear the interrupt (NB: Write 1 to reset)
      EXTI->PR1 = (1 << gpioPinOffset(A_input));
      pulse_count = pulse_count - 1;
      }
     
}

// B interrupt func
void EXTI9_5_IRQHandler(void){
  // first make sure that the interrupt was from B_input (bc line 6 is shared with 5-9) 
  if (EXTI->PR1 & (1 << gpioPinOffset(B_input))) {
      // clear the interrupt (NB: Write 1 to reset)
      EXTI->PR1 = (1 << gpioPinOffset(B_input));

      // set digital inputs
      int A_in = digitalRead(A_input); 
      int B_in = digitalRead(B_input); 
    //check for clockwise rotation -- if B is high and A is high
    if (A_in & B_in) {
      pulse_count = pulse_count + 1; 
      }

    // check for counter clockwise rotation -- if A is low and B is high
    else if(~A_in & B_in) {
      pulse_count = pulse_count - 1;
      }
  }

  
  }


// Function used by printf to send characters to the laptop
int _write(int file, char *ptr, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    ITM_SendChar((*ptr++));
  }
  return len;
}

// enables the gpio ports and the inputs where A and B come in
void sensorInputEnable(void) {
  gpioEnable(GPIO_PORT_B);
  pinMode(A_input, GPIO_INPUT);
  pinMode(B_input, GPIO_INPUT); 
  GPIOB->PUPDR &= ~(0b11 << 2*gpioPinOffset(A_input)); 
  GPIOB->PUPDR &= ~(0b11 << 2*gpioPinOffset(B_input));
  GPIOB->PUPDR |= (0b01 << 2*gpioPinOffset(A_input)); 
  GPIOB->PUPDR |= (0b01 << 2*gpioPinOffset(B_input));
  pinMode(A_output, GPIO_OUTPUT);
}

//initializes counter two in order to count the amount of time that delays are running for
void initCounter(void) {
  RCC-> APB1ENR1 |= (1<<0); // TIM2EN
  initTIM(DELAY_TIM);
}

// sets up the interrupts for our A and B inputs through masking, rising/falling edges, etc.  
void interruptSetup(void) {
  // 1. Enable SYSCFG clock domain in RCC
  RCC->APB2ENR |= (1 << 0); // enable SYSCFGEN
  // 2. Configure EXTICR for the input button interrupt

  // EXTI0 (for PB0) is bits 2:0 of EXTICR1 (EXTICR[1] in C). Port B is 0b001, so we select 001.
  SYSCFG->EXTICR[0] &= (0b0);   //clear first
  SYSCFG->EXTICR[0] |= (0b001);
  // EXTI6 (for PB6) is bits 10:8 of EXTICR2 (EXTICR[1] in C). Port B is 0b001, so we select 001.
  SYSCFG->EXTICR[1] &= ~(0b111 <<8);     //clear first
  SYSCFG->EXTICR[1] |= (0b001 << 8);

  // Configure interrupt for falling edge of GPIO pin for button
  EXTI->IMR1 |= (1 << gpioPinOffset(A_input));   // 1. Configure mask bit for A
  EXTI->IMR1 |= (1 << gpioPinOffset(B_input));   // 1. Configure mask bit for B
  EXTI->RTSR1 |= (1 << gpioPinOffset(A_input)); // 2. Enable rising edge trigger for A 
  EXTI->RTSR1 |= (1 << gpioPinOffset(B_input)); // 2. Enable rising edge trigger for B
  EXTI->FTSR1 |= (1 << gpioPinOffset(A_input));  // 3. Enable falling edge trigger for A
  EXTI->FTSR1 |= (1 << gpioPinOffset(B_input));  // 3. Enable falling edge trigger for B
}

// enable interrupts specifically for PB0 and PB6 through NVIC_ISER -- final level of config for an interrupt before they are recognized. 
void a_b_interruptEnable(void) {
  NVIC->ISER[0] |= (1 << 23);                       // 4. Turn on EXTI interrupt in NVIC_ISER (EXTI9_5 is IRQ 23)
  NVIC->ISER[0] |= (1 << 13);                       // 4. Turn on EXTI interrupt in NVIC_ISER (EXTI0 is IRQ 12)
}