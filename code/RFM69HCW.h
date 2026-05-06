/*
 * RFM69HCW.h
 *
 *  Created on: May 6, 2026
 *      Author: benja
 */

#ifndef INC_RFM69HCW_H_
#define INC_RFM69HCW_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>

#define RegVersion 0x10 // Version code of the chips
#define RegFrfMsb 0x07  // MSB of the RF carrier frequency
#define RegFrfMid 0x08	// Middle byte of the RF carrier frequency
#define RegFrfLsb 0x09  // LSB of the RF carrier frequency
#define RegOpMode 0x01  // Transceiver's operating modes

/*
 * Init SPI Pin and CS pin
 */
void RFM69_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *CS_Port, uint16_t CS_Pin);

/*
 * Mode Choice
 */
typedef enum {
    RFM69_MODE_SLEEP   = 0x00,
    RFM69_MODE_STDBY   = 0x04,
    RFM69_MODE_FS      = 0x08,
    RFM69_MODE_TX      = 0x0C,
    RFM69_MODE_RX      = 0x10
} RFM69_Mode_t;
void RFM69_SetMode(RFM69_Mode_t mode);

/*
 * Modulation Choice
 */
typedef enum {
    RFM69_PACKET_MODE     = 0x00, // 00
    RFM69_CONTINUOUS_SYNC = 0x40, // 10
    RFM69_CONTINUOUS_RAW  = 0x60  // 11
} RFM69_DataProcessingMode_t;

typedef enum {
    RFM69_MODUL_FSK = 0x00, // 00
    RFM69_MODUL_OOK = 0x08  // 01
} RFM69_Modulation_t;

typedef enum {
    RFM69_SHAPING_NONE    = 0x00, // 00
    RFM69_SHAPING_Gaussianfilter_BT1     = 0x01,  // 01
	RFM69_SHAPING_Gaussianfilter_BT05     = 0x02, // 10
	RFM69_SHAPING_Gaussianfilter_BT03     = 0x03  // 11
} RFM69_Data_Shaping_t;
void RFM69_SetModulationType(RFM69_DataProcessingMode_t mode, RFM69_Modulation_t modulation, RFM69_Data_Shaping_t shaping);

/*
 * Frequency Choice
 */
void RFM69_SetFrequency(uint32_t freqMHz);

/*
 * Write 1 value in a register
 */
void RFM69_WriteReg(uint8_t reg, uint8_t data);

/*
 * Read 1 value from a register
 */
uint8_t RFM69_ReadReg(uint8_t reg);
/*
 * Get information of Configuration
 */
void RFM69_getConfigData();

#endif /* INC_RFM69HCW_H_ */
