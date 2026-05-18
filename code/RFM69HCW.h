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
#include  <string.h>
#include <ctype.h>

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


/*
 * Activate And Deactivate all DEBUG PRINTF in one time !
 * You only need to comment or uncomment the line below
 */
#define RFM69_DEBUG_ENABLED

#ifdef RFM69_DEBUG_ENABLED
    #define RFM69_printf(...) printf(__VA_ARGS__)
#else
    #define RFM69_printf(...)
#endif


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
void RFM69_GetLnaStatus(void);

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
 * AX.25 Structure of Data
 */
typedef struct {
    uint8_t  flag_start;    // 1 octet
    uint8_t  adresse[14];   // 14 octets
    uint8_t  control;       // 1 octet
    uint8_t  data[256];     // 256 octets
    uint16_t fcs;           // 2 octets (Le compilateur va aligner ce champ)
    uint8_t  flag_end;      // 1 octet
} TrameAX;
void display_trame(const TrameAX *trame);
int Fill_Up_Data(TrameAX *trame, const char *message);

#endif /* INC_RFM69HCW_H_ */
