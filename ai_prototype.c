#include "stm32l4xx_hal.h"

volatile int32_t encoder_position = 0;

void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Configure PA0 and PA1 as Interrupt Pins (Rising & Falling edges)
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Enable EXTI Interrupt Vector Lines
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

// Low-Level IRQ Handlers
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

// HAL Callback executed on edge transition
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static uint8_t last_state = 0;

    // Read current pin states
    uint8_t a = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
    uint8_t b = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
    uint8_t current_state = (a << 1) | b;

    // Lookup table for 4x quadrature decoding step evaluation
    // Index: (last_state << 2) | current_state
    static const int8_t lookup_table[16] = {
         0, -1,  1,  0,
         1,  0,  0, -1,
        -1,  0,  0,  1,
         0,  1, -1,  0
    };

    uint8_t index = ((last_state & 0x03) << 2) | (current_state & 0x03);
    encoder_position += lookup_table[index];

    last_state = current_state;
}
