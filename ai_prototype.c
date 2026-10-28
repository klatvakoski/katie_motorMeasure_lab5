#include <stm32l4xx.h>

/**
 * @brief Configure PA0 and PA1 as EXTI inputs (Rising + Falling edges)
 */
void Encoder_EXTI_Init(void) {
    // 1. Enable GPIOA and SYSCFG clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // 2. Set PA0 and PA1 as Input (00)
    GPIOA->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1);

    // 3. Enable Pull-up resistors for PA0 and PA1
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1);
    GPIOA->PUPDR |= (1 << GPIO_PUPDR_PUPD0_Pos) | (1 << GPIO_PUPDR_PUPD1_Pos);

    // 4. Connect EXTI0 and EXTI1 lines to GPIOA (PA0 and PA1)
    SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI0 | SYSCFG_EXTICR1_EXTI1); // Select Port A (0x0)

    // 5. Unmask EXTI0 and EXTI1 interrupts
    EXTI->IMR1 |= (EXTI_IMR1_IM0 | EXTI_IMR1_IM1);

    // 6. Trigger on BOTH Rising and Falling edges
    EXTI->RTSR1 |= (EXTI_RTSR1_RT0 | EXTI_RTSR1_RT1);
    EXTI->FTSR1 |= (EXTI_FTSR1_FT0 | EXTI_FTSR1_FT1);

    // 7. Enable EXTI0 and EXTI1 in the NVIC
    NVIC_SetPriority(EXTI0_IRQn, 2);
    NVIC_EnableIRQ(EXTI0_IRQn);

    NVIC_SetPriority(EXTI1_IRQn, 2);
    NVIC_EnableIRQ(EXTI1_IRQn);
}

/**
 * @brief EXTI Line 0 Interrupt Handler (PA0 / Channel A)
 */
void EXTI0_IRQHandler(void) {
    if (EXTI->PR1 & EXTI_PR1_PIF0) {
        EXTI->PR1 = EXTI_PR1_PIF0; // Clear pending flag by writing 1

        // Read PA0 and PA1 pin states directly from IDR
        uint8_t ch_a = (GPIOA->IDR & GPIO_IDR_ID0) ? 1 : 0;
        uint8_t ch_b = (GPIOA->IDR & GPIO_IDR_ID1) ? 1 : 0;
        uint8_t current_state = (ch_a << 1) | ch_b;

        /* Your interrupt logic here */
    }
}

/**
 * @brief EXTI Line 1 Interrupt Handler (PA1 / Channel B)
 */
void EXTI1_IRQHandler(void) {
    if (EXTI->PR1 & EXTI_PR1_PIF1) {
        EXTI->PR1 = EXTI_PR1_PIF1; // Clear pending flag by writing 1

        // Read PA0 and PA1 pin states directly from IDR
        uint8_t ch_a = (GPIOA->IDR & GPIO_IDR_ID0) ? 1 : 0;
        uint8_t ch_b = (GPIOA->IDR & GPIO_IDR_ID1) ? 1 : 0;
        uint8_t current_state = (ch_a << 1) | ch_b;

        /* Your interrupt logic here */
    }
}