/******************************************************************************
 * @file    MCAL_RFMHCW.h
 * @author  Benjamin
 * @brief   RFM69HCW driver for STM32
 *
 * Copyright (c) 2026 Benjamin
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * SPDX-License-Identifier: MPL-2.0
 *
 * See LICENSE file in the project root for full license information.
 ******************************************************************************/
#ifndef INC_MCAL_RFM69HCW_H_
#define INC_MCAL_RFM69HCW_H_

#include "stm32L4xx_hal.h"

// Data pin (DATA)
#define RFM_DATA_PIN            GPIO_PIN_4
#define RFM_DATA_PORT           GPIOA

// Clock pin (DCLK)
#define RFM_DCLK_PIN            GPIO_PIN_15
#define RFM_DCLK_PORT           GPIOB
#define RFM_DCLK_EXTI_IRQn      EXTI15_10_IRQn

#define MCAL_RFM69H_INIT()                      do { } while(0)

#define MCAL_RFM69H_DELAY_MS(ms)                HAL_Delay(ms)
#define MCAL_RFM69H_GET_TICK()                  HAL_GetTick()

#define MCAL_RFM69H_SPI_TRANSMIT(hspi, tx_buf) \
    HAL_SPI_Transmit((hspi), (tx_buf), 2, HAL_MAX_DELAY)

#define MCAL_RFM69H_SPI_TRANSCEIVE(hspi, tx_buf, rx_buf) \
    HAL_SPI_TransmitReceive((hspi), (tx_buf), (rx_buf), 2, HAL_MAX_DELAY)

#define MCAL_RFM69H_CS_SELECT(port, pin)        HAL_GPIO_WritePin((port), (pin), GPIO_PIN_RESET)
#define MCAL_RFM69H_CS_DESELECT(port, pin)      HAL_GPIO_WritePin((port), (pin), GPIO_PIN_SET)

#define MCAL_RFM69H_DATA_SET_LOW(port, pin)     HAL_GPIO_WritePin((port), (pin), GPIO_PIN_RESET)
#define MCAL_RFM69H_DATA_SET_HIGH(port, pin)    HAL_GPIO_WritePin((port), (pin), GPIO_PIN_SET)

#define MCAL_RFM69H_READ_DCLK(port, pin)        HAL_GPIO_ReadPin((port), (pin))


#define MCAL_RFM69H_SET_PIN_OUTPUT(port, pin)   do { \
    GPIO_InitTypeDef GPIO_InitStruct = {0}; \
    GPIO_InitStruct.Pin = (pin); \
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; \
    GPIO_InitStruct.Pull = GPIO_NOPULL; \
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; \
    HAL_GPIO_Init((GPIO_TypeDef*)(port), &GPIO_InitStruct); \
} while(0)

#define MCAL_RFM69H_SET_PIN_INPUT(port, pin)    do { \
    GPIO_InitTypeDef GPIO_InitStruct = {0}; \
    GPIO_InitStruct.Pin = (pin); \
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; \
    GPIO_InitStruct.Pull = GPIO_NOPULL; \
    HAL_GPIO_Init((GPIO_TypeDef*)(port), &GPIO_InitStruct); \
} while(0)

#define MCAL_RFM69H_DISABLE_INTERRUPT()         HAL_NVIC_DisableIRQ(RFM_DCLK_EXTI_IRQn)
#define MCAL_RFM69H_ENABLE_INTERRUPT()          HAL_NVIC_EnableIRQ(RFM_DCLK_EXTI_IRQn)

#endif /* INC_MCAL_RFM69HCW_H_ */
