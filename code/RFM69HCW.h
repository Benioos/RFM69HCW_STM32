/******************************************************************************
 * @file    RFM69HCW.h
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

#ifndef INC_RFM69HCW_H_
#define INC_RFM69HCW_H_

#include "stm32L4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include  <string.h>
#include <ctype.h>
#include <MCAL_RFM69HCW.h>

#define RFM69_GOTO_XY(l, c) printf("\033[%d;%dH", l, c)

#define RegVersion 0x10 // Version code of the chips

#define RegFrfMsb 0x07  // MSB of the RF carrier frequency
#define RegFrfMid 0x08	// Middle byte of the RF carrier frequency
#define RegFrfLsb 0x09  // LSB of the RF carrier frequency

#define RegOpMode 0x01  // Transceiver's operating modes

#define RegDataModul 0x02 // DataOperationMode

#define RegBitrateMsb 0x03 // Bit Rate setting, Most Significant Bits
#define RegBitrateLsb 0x04 // Bit Rate setting, Least Significant Bits

#define RegSyncConfig 0x2E // Sync Word Recognition control
#define  RegSyncValue 0x2F // Sync Word bytes, 1 through 8

#define RegFdevMsb 0x05 // Frequency Deviation setting, Most Significant Bits
#define RegFdevLsb 0x06 // Frequency Deviation setting, Least Significant Bits

#define FIFO 0x28 // Message to send
#define RegIrqFlagsFIFO  0x28 //Satus register FIFO handling flags
#define RegIrqFlagsPLL  0x27 //Satus register PLL Lock State Timeout...
#define RegFIFO 0x00 // Register for text to send

#define RegPaLevel 0x11 // PA selection and Output Power control
#define RegRssiValue 0x24 // RSSI value in dBm
#define RegLna 0x18 // LNA settings

#define RegAfcLsb 0x20 // LSB of the frequency correction of the AFC
#define RegAfcMsb 0x1F // MSB of the frequency correction of the AFC
#define RegAfcFei 0x1E // AFC and FEI control and status
#define RegFeiMsb 0x21 // MSB of the calculated frequency error
#define RegFeiLsb 0x22 // LSB of the calculated frequency error

#define RegTemp1 0x4E // Temperature Sensor control

#define REG_PACKETCONFIG1 0x37 // PacketConfig

#define PLLock 0x10
#define TxReady 0x20
#define PacketSend 0x08
#define FStep 61 //Hz

#define TimeoutPacketNotSend 500 // Time before error when packet not send
#define TimeoutNoPacketReceived 10000 // Time before no packet received declared

#define RegFdevMsb 0x05 // Frequency Deviation setting, Most Significant Bits

#define REG_IRQFLAGS2 0x28
#define FIFO_NOT_EMPTY 0x40
#define RegIrqFlags1 0x27

#define RegDioMapping1 0x25

/*
 * Activate And Deactivate all DEBUG PRINTF and Delay in one time !
 * You only need to comment or uncomment the line below
 */
//#define RFM69_DEBUG_ENABLED
#define CLEAR_LINE       "\033[K"
#define CLEAR_SCREEN    "\033[2J"
#define CURSOR_HOME     "\033[H"
#define HIDE_CURSOR     "\033[?25l"
#define SHOW_CURSOR     "\033[?25h"


#ifdef RFM69_DEBUG_ENABLED
    #define RFM69_printf(color, prefix, ...) \
        printf(color prefix X __VA_ARGS__)
    #define RFM69_printfs(...) printf(__VA_ARGS__)

    #define RFM69_REFRESH_SCREEN() printf(CURSOR_HOME)

    #define RFM69_INIT_DASHBOARD() printf(CLEAR_SCREEN HIDE_CURSOR)
#else
    #define RFM69_printf(color, prefix, ...)
    #define RFM69_printfs(...)
    #define RFM69_REFRESH_SCREEN()
    #define RFM69_INIT_DASHBOARD()
#endif


#define RFM69_Delay(...) MCAL_RFM69H_DELAY_MS(__VA_ARGS__)


// Couleurs
#define R "\033[31m"  // Red
#define G "\033[32m"  // Green
#define J "\033[33m"  // Yellow
#define B "\033[34m"  // Blue
#define M "\033[35m"  // Magenta
#define C "\033[36m"  // Cyan
#define W "\033[37m"  // White
#define X "\033[0m"   // Reset

/*
 * Init SPI Pin and CS pin
 */
void RFM69_Init(void *hspi, void *CS_Port, uint16_t CS_Pin);

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
 * Get Actual Mode
 */
RFM69_Mode_t RFM69_GetMode(void);



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
 * Set DIO Mapping Default
 */
void RFM69_SetDefaultDioMapping(void);

/*
 * Modulation Type
 */
typedef enum {
    RFM69_MODUL_FSK = 0x00, // 00
    RFM69_MODUL_OOK = 0x08  // 01
} RFM69_Modulation_t;
void RFM69_SetModulationType(RFM69_Modulation_t modulation);

/*
 * Get Modulation Type
 */
RFM69_Modulation_t RFM69_GetModulationType(void);

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
 * PRINT CONFIG
 */
void PRINT_ConfigData();


/*
 * Set bitrate
 */
void RFM69_SetBitrate(uint32_t bitrate);

/*
 * Get Bitrate
 */
uint32_t RFM69_GetBitrate(void);
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
typedef enum {
    RFM69_PACKET_FORMAT_FIXED    = 0x00,
    RFM69_PACKET__FORMAT_VARIABLE = 0x80
} RFM69_PacketFormat_t;

typedef enum {
    RFM69_DC_FREE_NONE       = 0x00,
    RFM69_DC_FREE_MANCHESTER = 0x20,
    RFM69_DC_FREE_WHITENING  = 0x40
} RFM69_DcFree_t;

typedef enum {
    RFM69_CRC_OFF = 0x00,
    RFM69_CRC_ON  = 0x10
} RFM69_CrcOn_t;

typedef enum {
    RFM69_CRC_AUTOCLEAR_ON  = 0x00,
    RFM69_CRC_AUTOCLEAR_OFF = 0x08
} RFM69_CrcAutoClear_t;

typedef enum {
    RFM69_FILTER_NONE             = 0x00,
    RFM69_FILTER_NODE             = 0x02,
    RFM69_FILTER_NODE_BROADCAST   = 0x04
} RFM69_AddressFiltering_t;
void RFM69_SetPacketConfig(RFM69_PacketFormat_t format,
						   RFM69_DcFree_t dcFree,
						   RFM69_CrcOn_t crcOn,
						   RFM69_CrcAutoClear_t crcAutoClear,
						   RFM69_AddressFiltering_t filtering);

/*
 * Flush FIFO
 */
void RFM69_FlushFIFO(void);

/*
 * Send Message
 */
void RFM69_SendMessage_Packet_Mode(uint8_t* payload, uint8_t len);

/*
 * Power Amplifier Selection
 */
typedef enum {
    PA_0,           // Sortie RFIO (-18 à +13 dBm)
    PA_1,           // Sortie ANT (-18 à +13 dBm)
    PA_1_2,         // Sortie ANT (-14 à +17 dBm)
    PA_HIGH_POWER   // Sortie ANT (-11 à +20 dBm) NOT IMPLEMENTED YET
} RFM69_PA_Select_t;
void RFM69_PowerAmplifierSelection(RFM69_PA_Select_t pa, int8_t dbm_step);

RFM69_PA_Select_t RFM69_GetPowerAmplifier(void);
/*
 * Read RSSI in dBm
 */
uint8_t RFM69_RSSI(void);


/*
 * Read Message
 */
uint8_t RFM69_ReceiveMessage_Packet_Mode(uint8_t* buffer, uint8_t maxLen);

/*
 * Set LNA Impedance
 */
typedef enum {
    LNA_Impedance_50   = 0x00, // Standard
	LNA_Impedance_200  = 0x80  // High impedance
} RFM69_LnaZin_t;
void RFM69_SetLnaImpedance(RFM69_LnaZin_t zin);

/*
 * LNA Gain
 */
typedef enum {
    LNA_GAIN_AUTO = 0x00, // AGC (Automatique)
    LNA_GAIN_G1   = 0x01, // Max Gain
    LNA_GAIN_G2   = 0x02,
    LNA_GAIN_G3   = 0x03,
    LNA_GAIN_G4   = 0x04,
    LNA_GAIN_G5   = 0x05,
    LNA_GAIN_G6   = 0x06  // Min Gain
} RFM69_LnaGain_t;
void RFM69_SetLnaGain(RFM69_LnaGain_t gain);

/*
 * Get LNA Status
 */
uint8_t RFM69_GetLnaGain(void);

/*
 * GET AFC Correction Hz
 */
int32_t RFM69_GetAFCCorrectionHz(void);

/*
 * GET Frequency Error FEI Hz
 */
int32_t RFM69_GetFrequencyErrorFeiHz(void);

/*
 * Get Status of Reading Error Frequency
 */
uint8_t RFM69_Status_ReadingErrorFrequency(void);

/*
 * Get Status of Frequency recalibration AFC
 */
uint8_t RFM69_Status_RecalibrationFrequencyAFC(void);

/*
 * Get Error Frequency FEI
 */
void RFM69_StartFEI(void);

/*
 * Start Frequency Recalibration AFC
 */
void RFM69_StartAFC(void);

/*
 * Clear Frequency Recalibration AFC
 */
void RFM69_ClearAfc(void);

/*
 * Activate/Deactive Automatic Frequency Recalibration AFC
 */
typedef enum {
    AFC_AUTO_OFF = 0,
    AFC_AUTO_ON  = 1
} RFM69_AfcAuto_t;
void RFM69_SetAfcAuto(RFM69_AfcAuto_t state);

/*
 * Activate/Deactive Clearing Automatic Frequency Recalibration AFC
 */
typedef enum {
    AFC_AUTOCLEAR_OFF = 0,
    AFC_AUTOCLEAR_ON  = 1
} RFM69_AfcAutoclear_t;
void RFM69_SetAfcAutoclear(RFM69_AfcAutoclear_t state);

/*
 * Get Internal Temperature
 */
uint8_t RFM69_GetTemperature(void);

/*
 * AX25 Initialisation
 */

/*
 * AX.25 Structure of Data
 */
typedef struct {
    uint8_t control;          // 1 Byte
    uint8_t Type_Data;        // 1 Byte
    uint8_t data[257];        // 257 Bytes
}Payload;

typedef struct {
    uint8_t  flag_start;    // 1 Bytes
    uint8_t  adresse[14];   // 14 Bytes
    Payload payload;		// 259 Bytes

} TrameAX;

/*
 * RAW MODE : Text to Binary Payload
 */
uint16_t RFM69_Text_To_Binary_Payload(const char *texte, uint8_t *payload);

/*
 * CRC 16 bits
 */
uint16_t RFM69_RAW_CRC16_Calculation_Buffer(const uint8_t *data, uint16_t length);

/*
 * FIll Up Trame
 */
uint16_t RFM69_RAW_FillUp_Payload(TrameAX *trame,const uint8_t adresse[14],uint8_t  control, uint8_t Type_Data, const uint8_t *payload,uint8_t size_payload, uint16_t fcs_val);

/*
  * RAW Mode SEND PAYLOAD
*/
uint8_t RFM69_RAW_DATA_SEND(const uint8_t *buffer, uint16_t size);

/*
 * RAW MODE : Receive Data
 */
void Set_RX_MODE(void);

/*
 * RAW Mode Transmit Byte
 */
void RFM69_RAW_Transmit_Byte(uint8_t data, uint8_t enable_stuffing);

/*
 * Print Bytes
 */
void RFM69_PrintBits(uint8_t byte);

/*
 * WaitForPLLLock
 */
uint8_t RFM69_WaitForPLLLock(uint32_t timeout_ms);

/*
 * WaitForTxReady
 */
uint8_t RFM69_WaitForTxReady(uint32_t timeout_ms);

uint16_t RFM69_Padding(uint8_t *buffer, uint16_t len);

/*
 * CRC Check DASHBOARD
 */
void CRC_CHECK(const TrameAX *trame, uint16_t rx_longueur_paquet);

/*
 * PRINT CONFIGURATION OF RADIO
 */
void Set_DASHBOARD_CONFIG(void);

/*
 * PRINT DATA TRAME
 */
void Set_DASHBOARD_DATA(const volatile TrameAX *trame, uint16_t len);

/*
 * PRINT_DASHBOARD_PAYLOAD_RECEIVE
 */
void Set_DASHBOARD_PAYLOAD_INFO_RECEIVE(const volatile TrameAX *trame, uint16_t rx_longueur_paquet);

/*
 * PRINT_DASHBOARD_PAYLOAD_TRANSMIT
 */
void Set_DASHBOARD_PAYLOAD_INFO_TRANSMIT(const volatile TrameAX *trame, uint16_t payload_data_size);

/*
 * PRINT STATUS OF RADIO
 */
void Set_DASHBOARD_STATUS(void);

/*
 * PRINT_DASHBOARD_RSSI
 */
void Set_DASHBOARD_RSSI(void);

/*
 * SET PIN OUTPUT
 */
void Set_Pin_Output(void* GPIOx, uint16_t GPIO_Pin);

/*
 * SET PIN INPUT
 */
void Set_Pin_Input(void* GPIOx, uint16_t GPIO_Pin);

void SPI_Select_RFM69(void);
void SPI_Deselect_RFM69(void);
#endif /* INC_RFM69HCW_H_ */
