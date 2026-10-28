#include "stm32l4xx_hal.h"

/**
 * @brief Initialize PA0 (EXTI0) and PA1 (EXTI1) for dual-edge interrupts
 */
void Encoder_EXTI_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Configure PA0 and PA1 as EXTI inputs triggering on rising & falling edges
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP; // Set to GPIO_NOPULL if external pull-ups exist
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Enable EXTI Line 0 Interrupt
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    // Enable EXTI Line 1 Interrupt
    HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

/**
 * @brief EXTI Line 0 Interrupt Handler (PA0 / Channel A)
 */
void EXTI0_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/**
 * @brief EXTI Line 1 Interrupt Handler (PA1 / Channel B)
 */
void EXTI1_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

/**
 * @brief Edge-triggered callback for channel pin state changes
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_0 || GPIO_Pin == GPIO_PIN_1) {
        // Read current state of both signals (Bit 1 = PA0, Bit 0 = PA1)
        uint8_t ch_a = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
        uint8_t ch_b = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
        
        uint8_t current_state = (ch_a << 1) | ch_b;

        /* Insert custom logic here using current_state */
    }
}