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
#define REG_PACKETCONFIG1 0x37 // PacketConfig

#define PLLock 0x10
#define TxReady 0x20
#define PacketSend 0x08

#define TimeoutPacketNotSend 500 // Time before error when packet not send

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
 * Data Processing Mode
 */
typedef enum {
    RFM69_PACKET_MODE     = 0x00, // 00
    RFM69_CONTINUOUS_SYNC = 0x40, // 10
    RFM69_CONTINUOUS_RAW  = 0x60  // 11
} RFM69_DataProcessingMode_t;
void RFM69_SetDataProcessingMode(RFM69_DataProcessingMode_t mode);


/*
 * Modulation Type
 */
typedef enum {
    RFM69_MODUL_FSK = 0x00, // 00
    RFM69_MODUL_OOK = 0x08  // 01
} RFM69_Modulation_t;
void RFM69_SetModulationType(RFM69_Modulation_t modulation);

/*
 * DataShaping
 */

typedef enum {
    RFM69_SHAPING_NONE    = 0x00, // 00
    RFM69_SHAPING_Gaussianfilter_BT1     = 0x01,  // 01
	RFM69_SHAPING_Gaussianfilter_BT05     = 0x02, // 10
	RFM69_SHAPING_Gaussianfilter_BT03     = 0x03  // 11
} RFM69_Data_Shaping_t;
void RFM69_SetDataShaping(RFM69_Data_Shaping_t shaping);

/*
 * Frequency Choice
 */
void RFM69_SetFrequencyTo433(void);

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

/*
 * Set bitrate
 */
void RFM69_SetBitrate(uint32_t bitrate);

/*
 * Automatic verification of word received
 */
typedef enum {
    RFM69_SYNC_OFF = 0x00,
    RFM69_SYNC_ON  = 0x80
} RFM69_SyncState_t;

typedef enum {
    RFM69_FIFO_FILL_IF_SYNC    = 0x00,
    RFM69_FIFO_FILL_ALWAYS     = 0x40
} RFM69_FifoFill_t;

typedef enum {
    RFM69_SYNC_TOLERANCE_ERROR_0 = 0x00,
    RFM69_SYNC_TOLERANCE_ERROR_1 = 0x01,
    RFM69_SYNC_TOLERANCE_ERROR_2 = 0x02,
    RFM69_SYNC_TOLERANCE_ERROR_3 = 0x03,
	RFM69_SYNC_TOLERANCE_ERROR_4 = 0x04,
	RFM69_SYNC_TOLERANCE_ERROR_5 = 0x05,
	RFM69_SYNC_TOLERANCE_ERROR_6 = 0x06,
	RFM69_SYNC_TOLERANCE_ERROR_7 = 0x07
} RFM69_SyncTol_t;
void RFM69_AutoMessageDetectionReceive_Config(RFM69_SyncState_t state, RFM69_FifoFill_t fill, uint8_t size_of_key, RFM69_SyncTol_t tol);

/*
 * Word for synchronisation
 */
void RFM69_SetKeyValues_Sync(uint8_t *key, uint8_t len);

/*
 * AutoSetFdev for FSK
 */
void RFM69_AutoSetFdev(void);

/*
 * SetAutomatiquePacketConfig
 */
void RFM69_SetPacketConfig(void);

/*
 * Send Message
 */
void RFM69_SendMessage(uint8_t* payload, uint8_t len);

/*
 * ABORD MESSAGE
 */
void abord(void);

/*
 * Message Well Send
 */
void RFM69_DEBUG_MessageSendOK(void);


/*
 * Read Message
 */
uint8_t RFM69_ReceiveMessage(uint8_t* buffer, uint8_t maxLen);
#endif /* INC_RFM69HCW_H_ */
