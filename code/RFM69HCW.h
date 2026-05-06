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


#define RegVersion 0x10 //Version code of the chips





void RFM69_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *CS_Port, uint16_t CS_Pin);
void RFM69_WriteReg(uint8_t reg, uint8_t data);
uint8_t RFM69_ReadReg(uint8_t reg);

#endif /* INC_RFM69HCW_H_ */
