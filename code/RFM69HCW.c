/*
 * RFM69GCW.c
 *
 *  Created on: May 6, 2026
 *      Author: benjamin
 */

#include "RFM69HCW.h"

static SPI_HandleTypeDef *rfm69_hspi;
static GPIO_TypeDef *rfm69_cs_port;
static uint16_t rfm69_cs_pin;

/*
 * Write 1 value in a register
 */
void RFM69_WriteReg(uint8_t reg, uint8_t data)
{
    uint8_t tx[2];

    tx[0] = reg | 0x80;  // write (bit7 = 1)
    tx[1] = data;        // value to write

    HAL_GPIO_WritePin(rfm69_cs_port, rfm69_cs_pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(rfm69_hspi, tx, 2, HAL_MAX_DELAY);

    HAL_GPIO_WritePin(rfm69_cs_port, rfm69_cs_pin, GPIO_PIN_SET);
}

/*
 * Read 1 value from a register
 */

uint8_t RFM69_ReadReg(uint8_t reg)
{
    uint8_t tx[2];
    uint8_t rx[2];

    tx[0] = reg & 0x7F;  // read (bit7 = 0)
    tx[1] = 0x00;        // unused in this case

    HAL_GPIO_WritePin(rfm69_cs_port, rfm69_cs_pin, GPIO_PIN_RESET);

    HAL_SPI_TransmitReceive(rfm69_hspi, tx, rx, 2, HAL_MAX_DELAY);

    HAL_GPIO_WritePin(rfm69_cs_port, rfm69_cs_pin, GPIO_PIN_SET);

    return rx[1];
}

/*
 * Init SPI Pin and CS pin
 */
void RFM69_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *CS_Port, uint16_t CS_Pin)
{
    rfm69_hspi = hspi;
    rfm69_cs_port = CS_Port;
    rfm69_cs_pin = CS_Pin;

    HAL_GPIO_WritePin(rfm69_cs_port, rfm69_cs_pin, GPIO_PIN_SET);
}

/*
 * Select Mode Choice
 */
void RFM69_SetMode(RFM69_Mode_t mode)
{
    RFM69_WriteReg(RegOpMode, (uint8_t)mode);

    if (mode != RFM69_MODE_SLEEP) {
        HAL_Delay(1);
    }
}

/*
 * Get Actual Mode
 */
RFM69_Mode_t RFM69_GetMode(void)
{
    uint8_t val = RFM69_ReadReg(RegOpMode);
    return (RFM69_Mode_t)(val & 0x1C);
}

/*
 * Frequency Choice
 */
void RFM69_SetFrequencyTo433(void)
{
	  RFM69_WriteReg(RegFrfMsb, 0x6C);
	  RFM69_WriteReg(RegFrfMid, 0x40);
	  RFM69_WriteReg(RegFrfLsb, 0x00);
}

/*
 *  Set Bitrate
 */
void RFM69_SetBitrate(uint32_t bitrate)
{
    uint8_t msb = 0x1A; //Default value of 4800
    uint8_t lsb = 0x0B;

    switch (bitrate)
    {
        case 1200:
            msb = 0x68; lsb = 0x2B;
            break;
        case 2400:
            msb = 0x34; lsb = 0x15;
            break;
        case 4800:
            msb = 0x1A; lsb = 0x0B;
            break;
        case 9600:
            msb = 0x0D; lsb = 0x05;
            break;
        case 19200:
            msb = 0x06; lsb = 0x83;
            break;
        case 38400:
            msb = 0x03; lsb = 0x41;
            break;
        case 76800:
            msb = 0x01; lsb = 0xA1;
            break;
        case 153600:
            msb = 0x00; lsb = 0xD0;
            break;
        default:
        	msb = 0x1A; lsb = 0x0B;
        	RFM69_printf(R, "!Warning", "Bitrate not standard. Default 4.8kbps.\r\n");
            break;
    }
    RFM69_WriteReg(RegBitrateMsb, msb);
    RFM69_WriteReg(RegBitrateLsb, lsb);
}

/*
 *  Get Bitrate
 */
uint32_t RFM69_GetBitrate(void)
{
    uint8_t msb = RFM69_ReadReg(RegBitrateMsb);
    uint8_t lsb = RFM69_ReadReg(RegBitrateLsb);
    uint16_t regValue = (msb << 8) | lsb;

    switch (regValue)
    {
        case 0x682B: return 1200;
        case 0x3415: return 2400;
        case 0x1A0B: return 4800;
        case 0x0D05: return 9600;
        case 0x0683: return 19200;
        case 0x0341: return 38400;
        case 0x01A1: return 76800;
        case 0x00D0: return 153600;
        default:     return 1;
    }
}

/*
 * AutoSetFdev for FSK
 */
void RFM69_AutoSetFdev(void)
{
    uint8_t msb_b = RFM69_ReadReg(RegBitrateMsb);
    uint8_t lsb_b = RFM69_ReadReg(RegBitrateLsb);
    uint16_t bitReg = (uint16_t)((msb_b << 8) | lsb_b);

    if (bitReg == 0) return;

    uint32_t currentBitrate = 32000000 / bitReg;
    uint32_t fdevHz = (currentBitrate * 125) / 100;
    uint32_t valReg = (fdevHz * 1000) / 61035;

    if (valReg > 0x3FFF) valReg = 0x3FFF;

    uint8_t msb = RFM69_ReadReg(RegFdevMsb);
    msb &= 0xC0;
    msb |= (uint8_t)(valReg >> 8);

    RFM69_WriteReg(RegFdevMsb, msb);
    RFM69_WriteReg(RegFdevLsb, (uint8_t)(valReg & 0xFF));
}

/*
 * GetFdev for FSK
 */
uint32_t RFM69_GetFdev(void)
{
    uint8_t msb = RFM69_ReadReg(RegFdevMsb);
    uint8_t lsb = RFM69_ReadReg(RegFdevLsb);

    uint16_t valReg = ((msb & 0x3F) << 8) | lsb;
    uint32_t fdevHz = (valReg * 61035) / 1000;

    return fdevHz;
}

/*
 * Set Modulation Type
 */
void RFM69_SetModulationType(RFM69_Modulation_t modulation)
{
    uint8_t currentVal = RFM69_ReadReg(RegDataModul);
    uint8_t newVal = (currentVal & 0xE7) | (uint8_t)modulation;

    RFM69_WriteReg(RegDataModul, newVal);
}

/*
 * Get Modulation Type
 */
RFM69_Modulation_t RFM69_GetModulationType(void)
{
    uint8_t currentVal = RFM69_ReadReg(RegDataModul);

    uint8_t modulationVal = currentVal & 0x18;

    return (RFM69_Modulation_t)modulationVal;
}

/*
 * Set Data Processing Mode
 */
void RFM69_SetDataProcessingMode(RFM69_DataProcessingMode_t mode)
{
    uint8_t currentVal = RFM69_ReadReg(RegDataModul);
    uint8_t newVal = (currentVal & 0x9F) | (uint8_t)mode;
    RFM69_WriteReg(RegDataModul, newVal);
}

/*
 * Get Data Processing Mode
 */
RFM69_DataProcessingMode_t RFM69_GetDataProcessingMode(void)
{
    uint8_t currentVal = RFM69_ReadReg(RegDataModul);
    uint8_t modeVal = currentVal & 0x60;

    return (RFM69_DataProcessingMode_t)modeVal;
}

/*
 * Set DataShaping
 */
void RFM69_SetDataShaping(RFM69_Data_Shaping_t shaping)
{
    uint8_t currentVal = RFM69_ReadReg(RegDataModul);
    uint8_t modulation = (currentVal & 0x18);

    if (modulation == RFM69_MODUL_OOK && shaping == RFM69_SHAPING_Gaussianfilter_BT03) {
    	RFM69_printf(R, "!Warning", "Shaping BT0.3 incompatible with OOK. Forcing NONE.\r\n");
    	shaping = RFM69_SHAPING_NONE;
    }

    uint8_t newVal = (currentVal & 0xFC) | (uint8_t)shaping;

    RFM69_WriteReg(RegDataModul, newVal);
}

/*
 * Get DataShaping
 */
RFM69_Data_Shaping_t RFM69_GetDataShaping(void)
{
    uint8_t currentVal = RFM69_ReadReg(RegDataModul);
    uint8_t shapingVal = currentVal & 0x03;

    return (RFM69_Data_Shaping_t)shapingVal;
}

/*
 * WaitForPLLLock
 */
uint8_t RFM69_WaitForPLLLock(uint32_t timeout_ms)
{
    uint32_t tick = HAL_GetTick();

    while (!(RFM69_ReadReg(RegIrqFlagsPLL) & PLLock))
    {
        if ((HAL_GetTick() - tick) > timeout_ms)
        {
            RFM69_printf(R, "!ERROR", "PLL never locked\r\n");
            RFM69_printf(R, "!ABORD", "\r\n");

            RFM69_SetMode(RFM69_MODE_STDBY);

            return 0;
        }
    }

    return 1;
}

/*
 * WaitForTxReady
 */
uint8_t RFM69_WaitForTxReady(uint32_t timeout_ms)
{
    uint32_t tick = HAL_GetTick();

    while (!(RFM69_ReadReg(RegIrqFlags1) & TxReady))
    {
        if ((HAL_GetTick() - tick) > timeout_ms)
        {
            RFM69_printf(R, "!ERROR", "TX Not Ready\r\n");
            RFM69_printf(R, "!ABORD", "\r\n");
            RFM69_SetMode(RFM69_MODE_STDBY);
            return 0;
        }
    }

    return 1;
}

/*
 * PACKET MODE : Word for Synchronization
 */
void RFM69_SetKeyValues_Sync(uint8_t *key, uint8_t len)
{
    if(len > 8) len = 8;

    for(uint8_t i = 0; i < len; i++)
    {
        RFM69_WriteReg(RegSyncValue + i, key[i]);
    }
}

/*
 * PACKET MODE : SetAutomatiquePacketConfig
 */
void RFM69_SetPacketConfig(RFM69_PacketFormat_t format,
                           RFM69_DcFree_t dcFree,
                           RFM69_CrcOn_t crcOn,
                           RFM69_CrcAutoClear_t crcAutoClear,
                           RFM69_AddressFiltering_t filtering)
{
    uint8_t regValue = (uint8_t)(format | dcFree | crcOn | crcAutoClear | filtering);
    RFM69_WriteReg(REG_PACKETCONFIG1, regValue);
}

/*
 * PACKET MODE : Automatic verification of word received
 */
void RFM69_AutoMessageDetectionReceive_Config(RFM69_SyncState_t state, RFM69_FifoFill_t fill, uint8_t size_of_key, RFM69_SyncTol_t tol)
{
    if (size_of_key < 1) size_of_key = 1;
    if (size_of_key > 8) size_of_key = 8;

    uint8_t config = (uint8_t)state | (uint8_t)fill | ((size_of_key - 1) << 3) | (uint8_t)tol;

    RFM69_WriteReg(RegSyncConfig, config);
}

/*
 * PACKET MODE : Flush FIFO
 */
void RFM69_FlushFIFO(void)
{
	RFM69_WriteReg(FIFO, 0x10);
}

/*
 * PACKET MODE : Send Message
 */
void RFM69_SendMessage_Packet_Mode(uint8_t* payload, uint8_t len)
{
	RFM69_printfs("\r\n--- Packet Mode Sending Message ---\r\n");

    RFM69_SetMode(RFM69_MODE_STDBY);
    RFM69_FlushFIFO();

    if (len > 64) // Can be extended to 66 maximum
    {
    	RFM69_printf(R, "!ERROR_SEND", "Len of message exceed 64\r\n");
    	RFM69_printf(R, "!ABORD", "\r\n");
        RFM69_SetMode(RFM69_MODE_STDBY);
        return;
    }

    RFM69_WriteReg(RegFIFO, len);
    for (uint8_t i = 0; i < len; i++) { // Can be optimized with DMA
        RFM69_WriteReg(RegFIFO, payload[i]);
    }

    RFM69_SetMode(RFM69_MODE_FS);

    uint32_t tick = HAL_GetTick();
    while (!(RFM69_ReadReg(RegIrqFlagsPLL) & PLLock)) {
        if ((HAL_GetTick() - tick) > 200)
		{
        	RFM69_printf(R, "!ERROR_SEND", "PLL never locked\r\n");
        	RFM69_printf(R, "!ABORD", "\r\n");
        	RFM69_SetMode(RFM69_MODE_STDBY);
        	return;
		}
    }

    RFM69_SetMode(RFM69_MODE_TX);

    tick = HAL_GetTick();
    while (!(RFM69_ReadReg(RegIrqFlagsPLL) & 0x80)) {
        if ((HAL_GetTick() - tick) > 20) {
        	RFM69_printf(R, "!ERROR_SEND", "Mode TX not Ready\r\n");
        	RFM69_printf(R, "!ABORD", "\r\n");
        	RFM69_SetMode(RFM69_MODE_STDBY);
            break;
        }
    }

    tick = HAL_GetTick();
    while (!(RFM69_ReadReg(RegIrqFlagsFIFO) & PacketSend)) {
        if ((HAL_GetTick() - tick) > TimeoutPacketNotSend) {

        	RFM69_printf(R, "!ERROR_SEND", "Packet Send Control down 0.\r\n");
        	RFM69_printf(R, "!ABORD", "\r\n");
        	RFM69_SetMode(RFM69_MODE_STDBY);
            break;
        }
    }

    RFM69_printf(G, "!Success", "Message Send With Success\r\n");
    RFM69_SetMode(RFM69_MODE_STDBY);

    RFM69_printfs("\r\n---  ---\r\n");
}


/*
 * PACKET MODE : Read Packet
 */
uint8_t RFM69_ReceiveMessage_Packet_Mode(uint8_t* buffer, uint8_t maxLen)
{
	RFM69_printfs("\r\n --- Packet Mode Reception Message ---\r\n");
    RFM69_WriteReg(RegIrqFlagsFIFO, 0x10);

    RFM69_SetMode(RFM69_MODE_RX);

    uint8_t rssi = RFM69_ReadReg(RegRssiValue);
    RFM69_printfs("\r\n Ambient Noise (RSSI): -%d dBm\r\n", rssi/2);

    uint32_t tick = HAL_GetTick();
    while (!(RFM69_ReadReg(RegIrqFlagsFIFO) & 0x04)) {
        if ((HAL_GetTick() - tick) > TimeoutNoPacketReceived)
        {
        	RFM69_printf(R, "!RX Timeout", "No packet received\r\n");
            RFM69_SetMode(RFM69_MODE_STDBY);
            return 0;
        }
    }

    uint8_t payloadLen = RFM69_ReadReg(RegFIFO);
    uint8_t bytesToRead = (payloadLen > maxLen) ? maxLen : payloadLen;

    for (uint8_t i = 0; i < bytesToRead; i++) {
        buffer[i] = RFM69_ReadReg(RegFIFO);
    }

    RFM69_printfs("\r\n Packet Received [%d bytes]: \r\n ", bytesToRead);
    for (uint8_t i = 0; i < bytesToRead; i++) {
    	RFM69_printfs("%c \r\n", buffer[i]);
    }

    RFM69_SetMode(RFM69_MODE_STDBY);
    RFM69_WriteReg(RegIrqFlagsFIFO, 0x10);

    return bytesToRead;
}

/*
 * RAW MODE : Set DIO Mapping Default
 */
void RFM69_SetDefaultDioMapping(void)
{
    RFM69_WriteReg(RegDioMapping1, 0x00);
}

/*
 * RAW MODE : Text to Binary Payload
 */
uint16_t RFM69_Text_To_Binary_Payload(const char *texte, uint8_t *payload)
{
	uint16_t len = (uint16_t) strlen(texte);
    uint16_t limited_len = (len > 254) ? 254 : len;
    memcpy(payload, texte, limited_len);
    return len;
}


/*
 * RAW MODE : Fill Up Payload
 */
#define CLEAR_BUFFER(buf) memset((buf), 0, sizeof(buf))
void RFM69_RAW_FillUp_Payload(TrameAX *trame, const uint8_t adresse[14], uint8_t control, uint8_t Type_Data, const uint8_t *payload_data, uint8_t size_payload, uint16_t fcs_val)
{
    trame->flag_start = 0x7E;
    memcpy(trame->adresse, adresse, 14);

    trame->payload.control   = control;
    trame->payload.Type_Data = Type_Data;

    memcpy(trame->payload.data, payload_data, 254);

    trame->fcs        = (fcs_val >> 8) | (fcs_val << 8);
    trame->flag_end   = 0x7E;
}

/*
  * RAW MODE : SEND PAYLOAD
*/
void RFM69_RAW_DATA_SEND(const uint8_t *buffer, uint16_t size)
{
    RFM69_SetMode(RFM69_MODE_TX);

    if (RFM69_WaitForPLLLock(200) == 0)
    {
        return;
    }

    if (RFM69_WaitForTxReady(200)== 0)
    {
        return;
    }

    RFM69_RAW_Transmit_Byte(0, 2);

    // Module Synchronization by sending 0x55 = 01010101..
    for (int i = 0; i < 4; i++) {
    	RFM69_RAW_Transmit_Byte(0x55, 0);
    }

    // Send PayLoad
    for (uint16_t i = 0; i < size; i++) {
    	uint8_t use_stuffing = (i > 0 && i < (size - 1)) ? 1 : 0;
    	RFM69_RAW_Transmit_Byte(buffer[i], use_stuffing);
    }

    RFM69_SetMode(RFM69_MODE_STDBY);

    HAL_GPIO_WritePin(RFM_DATA_PORT, RFM_DATA_PIN, GPIO_PIN_RESET);

	RFM69_Delay(200);
}

/*
 * RAW MODE : Transmit Byte
 */

void RFM69_RAW_Transmit_Byte(uint8_t data, uint8_t enable_stuffing)
{
    static uint8_t consecutive_ones = 0;

    if (enable_stuffing == 2) {
        consecutive_ones = 0;
        return;
    }

    for (int i = 7; i >= 0; i--)
    {
        while(HAL_GPIO_ReadPin(RFM_DCLK_PORT, RFM_DCLK_PIN) == GPIO_PIN_SET);

        if ((data >> i) & 0x01) {
            HAL_GPIO_WritePin(RFM_DATA_PORT, RFM_DATA_PIN, GPIO_PIN_SET);
            if (enable_stuffing) consecutive_ones++;
        } else {
            HAL_GPIO_WritePin(RFM_DATA_PORT, RFM_DATA_PIN, GPIO_PIN_RESET);
            if (enable_stuffing) consecutive_ones = 0;
        }

        while(HAL_GPIO_ReadPin(RFM_DCLK_PORT, RFM_DCLK_PIN) == GPIO_PIN_RESET);

        if (enable_stuffing && consecutive_ones == 5) {

            while(HAL_GPIO_ReadPin(RFM_DCLK_PORT, RFM_DCLK_PIN) == GPIO_PIN_SET);

            HAL_GPIO_WritePin(RFM_DATA_PORT, RFM_DATA_PIN, GPIO_PIN_RESET);
            consecutive_ones = 0;

            while(HAL_GPIO_ReadPin(RFM_DCLK_PORT, RFM_DCLK_PIN) == GPIO_PIN_RESET);
        }
    }
}


/*
 * RAW MODE : CRC 16 bits
 */
uint16_t RFM69_RAW_CRC16_Calculation_Buffer(const uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < length; i++)
    {
        crc ^= data[i];

        for (uint8_t bit = 0; bit < 8; bit++)
        {
            if (crc & 0x0001)
            {
                crc = (crc >> 1) ^ 0x8408;
            }
            else
            {
                crc >>= 1;
            }
        }
    }

    return ~crc;
}

/*
 * RAW MODE : CRC CHECK
 */
void CRC_CHECK(const TrameAX *trame)
{
   if (trame == NULL) {
   	RFM69_printf(R, "CRC_CHECK", "NULL\r\n");
       return;
   }

   uint16_t crc_received = ((trame->fcs & 0x00FF) << 8) | ((trame->fcs & 0xFF00) >> 8);
   uint16_t crc_calculated = RFM69_RAW_CRC16_Calculation_Buffer((const uint8_t *)trame->payload.data, 254);

   if (crc_calculated == crc_received)
   {
   	RFM69_printf(G, "[GOOD CRC]", "0x%02X\r\n", crc_calculated);
   }
   else
   {
		RFM69_printf(R, "[BAD CRC ]", "0x%02X\r\n", crc_calculated);
   }
}

/*
 * RAW MODE : Complete Payload up to 256 bytes
 */
void RFM69_Pad_To_256(uint8_t *buffer, uint16_t len)
{
    if (len > 254)
    {
        RFM69_printf(R, "!ERROR", "Too long.. %d >254 bytes\r\n", len);
        RFM69_printf(R, "!ERROR", "Payload won't be send normally\r\n");
        RFM69_printf(R, "!ERROR", "NEED TO ABORD\r\n");
        len = 254;
    }

    for (uint16_t i = len; i < 254; i++)
    {
        buffer[i] = 0x55;
    }
}

/*
 * Set Power Amplifier
 */
void RFM69_PowerAmplifierSelection(RFM69_PA_Select_t pa, int8_t dbm_step)
{
    uint8_t regVal = 0;
    int8_t outputPower = 0;

    switch (pa) {
        case PA_0:
            outputPower = dbm_step + 18;
            regVal = 0x80; // Bit 7 (PA0)
            break;

        case PA_1:
            outputPower = dbm_step + 18;
            regVal = 0x40; // Bit 6 (PA1)
            break;

        case PA_1_2:
            outputPower = dbm_step + 14;
            regVal = 0x60; // Bits 6 & 5 (PA1 + PA2)
            break;

        case PA_HIGH_POWER:
        	RFM69_printf(R, "!Incorrect", "This function haven't been implemented yet.\r\n");
        	RFM69_printf(R, "!ABORD", "\r\n");
            break;
    }

    if (outputPower < 0)  outputPower = 0;
    if (outputPower > 31) outputPower = 31;

    regVal |= (uint8_t)outputPower;

    RFM69_WriteReg(RegPaLevel, regVal);
}

/*
 * Get Power Amplifier
 */
RFM69_PA_Select_t RFM69_GetPowerAmplifier(void)
{
    uint8_t regVal = RFM69_ReadReg(RegPaLevel);

    // Isolation des bits de sélection PA (bits 7, 6, 5) -> Masque 0xE0
    uint8_t paBits = regVal & 0xE0;

    switch (paBits)
    {
        case 0x80: return PA_0;       // Bit 7 seul
        case 0x40: return PA_1;       // Bit 6 seul
        case 0x60: return PA_1_2;     // Bits 6 et 5 ensemble
        default:   return PA_0;       // Valeur par défaut par sécurité
    }
}


/*
 * Get RSSI in dBm
 */
uint8_t RFM69_RSSI(void)
{
	uint8_t rssi = RFM69_ReadReg(RegRssiValue);
	RFM69_printfs("\r\n Ambient Noise (RSSI): -%d dBm\r\n", rssi/2);
	return rssi;
}

/*
 * Set LNA Impedance
 */
void RFM69_SetLnaImpedance(RFM69_LnaZin_t zin)
{
    uint8_t oldVal = RFM69_ReadReg(RegLna);
    uint8_t newVal = (oldVal & 0x7F) | (uint8_t)zin;
    RFM69_WriteReg(RegLna, newVal);
}

/*
 * LNA Gain
 */
void RFM69_SetLnaGain(RFM69_LnaGain_t gain)
{
    uint8_t oldVal = RFM69_ReadReg(RegLna);
    uint8_t newVal = (oldVal & 0xF8) | (uint8_t)gain;
    RFM69_WriteReg(RegLna, newVal);
}

/*
 * Get LNA Status
 */
void RFM69_GetLnaStatus(void)
{
    uint8_t val = RFM69_ReadReg(RegLna);

    uint8_t zin  = (val & 0x80);      // Bit 7
    uint8_t real = (val & 0x38) >> 3; // Bits 5-3 (Effective gain)
    uint8_t set  = (val & 0x07);      // Bits 2-0 (Controlled gain)

    RFM69_printfs("\r\n --- LNA Status ---\r\n");
    RFM69_printfs("\r\n Impedance : %s\r\n", (zin == 0x80) ? "200 Ohms" : "50 Ohms");

    if (set == 0) printf("\r\n Mode : AUTOMATIC (AGC)\r\n");
    else          printf("\r\n Mode : MANUAL (G%d force)\r\n", set);

    RFM69_printfs("\r\n Current Gain : G%d\r\n", real);
    RFM69_printfs("\r\n ------------------\r\n");
}

/*
 * GET AFC Correction Hz
 */
int32_t RFM69_GetAFCCorrectionHz(void)
{
    int16_t val = (int16_t)(RFM69_ReadReg(RegAfcMsb) << 8); // MSB
    val |= RFM69_ReadReg(RegAfcLsb);                         // LSB
    return (int32_t)val * FStep;
}

/*
 * GET Frequency Error FEI Hz
 */
int32_t RFM69_GetFrequencyErrorFeiHz(void)
{
    uint8_t regAfcFei = RFM69_ReadReg(RegAfcFei);

    if (!(regAfcFei & 0x04))
    {
        RFM69_WriteReg(RegAfcFei, regAfcFei | 0x20);

        uint32_t tick = HAL_GetTick();
        while (!(RFM69_ReadReg(RegAfcFei) & 0x40)) {
            if ((HAL_GetTick() - tick) > 10) break;
        }
    }

    int16_t val = (int16_t)(RFM69_ReadReg(RegFeiMsb) << 8);
    val |= RFM69_ReadReg(RegFeiLsb);

    return (int32_t)val * 61;
}

/*
 * Get Status of Reading Error Frequency FEI
 */
uint8_t RFM69_Status_ReadingErrorFrequency(void)
{
    return (RFM69_ReadReg(RegAfcFei) & 0x40) >> 6;
}

/*
 * Get Status of Frequency recalibration AFC
 */
uint8_t RFM69_Status_RecalibrationFrequencyAFC(void)
{
    return (RFM69_ReadReg(RegAfcFei) & 0x40) >> 4;
}

/*
 * Start Error Frequency Measure FEI
 */
void RFM69_StartFEI(void)
{
    uint8_t val = RFM69_ReadReg(RegAfcFei);
    RFM69_WriteReg(RegAfcFei, val | 0x20);
}

/*
 * Start Frequency Recalibration AFC
 */
void RFM69_StartAFC(void)
{
    uint8_t val = RFM69_ReadReg(RegAfcFei);
    RFM69_WriteReg(RegAfcFei, val | 0x01);
}

/*
 * Clear Frequency Recalibration AFC
 */
void RFM69_ClearAfc(void)
{
    uint8_t val = RFM69_ReadReg(RegAfcFei);
    RFM69_WriteReg(RegAfcFei, val | 0x02);
}

/*
 * Activate/Deactivate Automatic Frequency Recalibration AFC
 */
void RFM69_SetAfcAuto(RFM69_AfcAuto_t state)
{
	uint8_t val = RFM69_ReadReg(RegAfcFei);
    if (state == AFC_AUTO_ON) {
        val |= 0x04;  // Put bit 2 at 1
    } else {
        val &= 0xFB;  // Put bit 2 at 0
    }
    RFM69_WriteReg(RegAfcFei, val);
}

/*
 * Activate/Deactivate Clearing Automatic Frequency Recalibration AFC
 */
void RFM69_SetAfcAutoclear(RFM69_AfcAutoclear_t state)
{
    uint8_t val = RFM69_ReadReg(RegAfcFei);
    if (state == AFC_AUTOCLEAR_ON) {
        val |= 0x08;
    } else {
        val &= 0xF7;
    }
    RFM69_WriteReg(RegAfcFei, val);
}


/*
 * Get Internal Temperature
 */
uint8_t RFM69_GetTemperature(void)
{
	if (RFM69_GetMode()!= RFM69_MODE_STDBY)
	{
		RFM69_printf(R, "!Incorrect", "must be in Standby Mode\r\n");
		RFM69_printf(R, "!ABORD", "\r\n");
		return 0;
	}

    RFM69_WriteReg(RegTemp1, 0x08);

    while (RFM69_ReadReg(RegTemp1) & 0x04);

    uint8_t rawTemp = RFM69_ReadReg(0x4F);

    return rawTemp;
}


/*
 * Get information of Configuration
 */
void PRINT_ConfigData(void)
{
 	uint8_t val = RFM69_ReadReg(0x02);

 	uint8_t mode = (val & 0x60);
 	uint8_t modulation = (val & 0x18);
 	uint8_t shaping = (val & 0x03);

 	RFM69_printfs("\r\n\033[36m[CONFIG RFM69HCW]\033[0m\r\n");

 	if(mode == RFM69_PACKET_MODE)
 	{
 		RFM69_printfs("Mode : PACKET\r\n");
 	}else if(mode == RFM69_CONTINUOUS_SYNC){
 		RFM69_printfs("Mode : RAW SYNC\r\n");
 	}else if(mode == RFM69_CONTINUOUS_RAW){
 		RFM69_printfs("Mode : RAW\r\n");
 	}

 	if(modulation == RFM69_MODUL_FSK)
 	{
 		RFM69_printfs("Modulation : FSK\r\n");
 	}else if(modulation == RFM69_MODUL_OOK){
 		RFM69_printfs("Modulation : OOK\r\n");
 	}

 	if(shaping == RFM69_SHAPING_NONE)
 	{
 		RFM69_printfs("Shaping : None\r\n");
 	}else if(shaping == RFM69_SHAPING_Gaussianfilter_BT1){
 		RFM69_printfs("Shaping : Gaussian Filtre BT=1.0\r\n");
 	}else if(shaping == RFM69_SHAPING_Gaussianfilter_BT05){
 		RFM69_printfs("Shaping : Gaussian Filtre BT=0.5\r\n");
 	}else if(shaping == RFM69_SHAPING_Gaussianfilter_BT03){
 		RFM69_printfs("Shaping : Gaussian Filtre BT=0.3\r\n");
 	}

     val = RFM69_ReadReg(0x01);
     uint8_t opMode = (val & 0x1C);
     if(opMode == RFM69_MODE_SLEEP) {
     	RFM69_printfs("Operation Mode : SLEEP\r\n");
     } else if(opMode == RFM69_MODE_STDBY) {
     	RFM69_printfs("Operation Mode : STANDBY\r\n");
     } else if(opMode == RFM69_MODE_TX) {
     	RFM69_printfs("Operation Mode : TRANSMITTING\r\n");
     } else if(opMode == RFM69_MODE_RX) {
     	RFM69_printfs("Operation Mode : RECEIVEING\r\n");
     }

     uint8_t msb = RFM69_ReadReg(0x07);
     uint8_t mid = RFM69_ReadReg(0x08);
     uint8_t lsb = RFM69_ReadReg(0x09);
     uint32_t frf = (uint32_t)((msb << 16) | (mid << 8) | lsb);
     uint64_t freqHz = ((uint64_t)frf * 6103515625ULL) / 100000000ULL;
     uint32_t mhz = (uint32_t)(freqHz / 1000000);
     uint32_t khz = (uint32_t)((freqHz % 1000000) / 1000);
     RFM69_printfs("Frequency : %lu.%03lu MHz\r\n", (unsigned long)mhz, (unsigned long)khz);

     uint8_t msb_b = RFM69_ReadReg(0x03);
     uint8_t lsb_b = RFM69_ReadReg(0x04);
     uint16_t bitReg = (uint16_t)((msb_b << 8) | lsb_b);
     uint32_t currentBitrate = (bitReg == 0) ? 0 : (32000000 / bitReg);
     RFM69_printfs("Bitrate    : %lu bps\r\n", (unsigned long)currentBitrate);


}

/*
 * PRINT DATA TRAME
 */
void PRINT_Data_TRAME(const volatile TrameAX *trame)
{
  	for (int i = 0; i < 254; i += 16)
		{
			RFM69_printfs("Line %3d : ", i);

			for (int j = 0; j < 16; j++)
			{
				if (i + j < 254) {
					RFM69_printfs("%02X ", trame->payload.data[i + j]);
				} else {
					RFM69_printfs("   ");
				}
			}

			RFM69_printfs(" | ");

			for (int j = 0; j < 16; j++)
			{
				if (i + j < 254) {
					uint8_t c = trame->payload.data[i + j];
					if (c >= 32 && c <= 126) {
						RFM69_printfs("%c", c);
					} else {
						RFM69_printfs(".");
					}
				}
			}
			RFM69_printfs(" | ");
			RFM69_printfs("\r\n");
		}

}









/*
 * Print Mode
 */
void PRINT_MODE(void)
{
	RFM69_Mode_t currentMode = RFM69_GetMode();
	switch (currentMode)
	{
		case RFM69_MODE_SLEEP:
			RFM69_printf(B, "[STATUS] ", "MODE SLEEP...");
			break;

		case RFM69_MODE_STDBY:
			RFM69_printf(W, "[STATUS] ", "MODE STANDBY.");
			break;

		case RFM69_MODE_FS:
			RFM69_printf(M, "[STATUS] ", "MODE FS");
			break;

		case RFM69_MODE_TX:
			RFM69_printf(R, "[STATUS] ", "MODE (TX)");
			break;

		case RFM69_MODE_RX:
			RFM69_printf(C, "[STATUS] ", "MODE (RX)");
			break;

		default:
			RFM69_printf(J, "[STATUS] ", "MODE UNDEFINED.");
			break;
	}
}




/*
 * PayLoad Print :
 */void GET_DASHBOARD(const volatile TrameAX *trame)
 {
 	RFM69_REFRESH_SCREEN();
 	RFM69_printfs("\033[36m===================================== RADIO MONITORING =====================================\033[0m\r\n");

 	RFM69_Mode_t currentMode = RFM69_GetMode();
 	switch (currentMode) {
 	        case RFM69_MODE_SLEEP:
 	            RFM69_printf(B, "[STATUS] ", "Module en veille (Sleep)...");
 	            break;

 	        case RFM69_MODE_STDBY:
 	            RFM69_printf(W, "[STATUS] ", "Mode Standby (Prêt).");
 	            break;

 	        case RFM69_MODE_FS:
 	            RFM69_printf(M, "[STATUS] ", "Synthétiseur de fréquence actif (FS)...");
 	            break;

 	        case RFM69_MODE_TX:
 	            RFM69_printf(R, "[STATUS] ", "Transmission en cours (TX)...");
 	            break;

 	        case RFM69_MODE_RX:
 	            RFM69_printf(C, "[STATUS] ", "En attente de paquets AX.25 (RX)...");
 	            break;

 	        default:
 	            RFM69_printf(J, "[STATUS] ", "Mode inconnu ou non défini.");
 	            break;
 	    }

 	RFM69_printfs("\033[36m\r\n============================================================================================\033[0m\r\n");
 	PRINT_ConfigData();
 	RFM69_printfs("\033[36m============================================================================================\033[0m\r\n");

 	if (currentMode == RFM69_MODE_RX) {
 		RFM69_printfs("\033[36m[RX METRICS]\033[0m\r\n");

 		RFM69_printfs("\033[36m============================================================================================\033[0m\r\n");

 		if (trame == NULL) {
 			RFM69_printf(R, "X_Radio_Show_TrameAX", "NULL\r\n");
 		} else {
 			RFM69_printfs("\033[36m[Payload AX.25]\033[0m\r\n\r\n");

 			if( trame->flag_start == 0x7E)
 			{
 				RFM69_printf(G, "[FLAG START]", "0x%02X\r\n", trame->flag_start);
 			}else{
 				RFM69_printf(R, "[FLAG START]", "0x%02X\r\n", trame->flag_start);
 			}

 			RFM69_printf(W, "[ADRESSE   ]", "");
 			for(int i = 0; i < 14; i++) {
 				uint8_t c = trame->adresse[i];
 				if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '-' || c == ' ') {
 					RFM69_printfs("%c", c);
 				} else {
 					RFM69_printfs("[%02X]", c);
 				}
 			}
 			RFM69_printfs("\r\n");
 			RFM69_printf(W, "[CONTROL   ]", "0x%02X\r\n", trame->payload.control);
 			RFM69_printf(W, "[TYPE DATA ]", "0x%02X", trame->payload.Type_Data);

 			switch(trame->payload.Type_Data) {
 				case 0x01: RFM69_printfs("  ( COMMAND )\r\n"); break;
 				case 0x02: RFM69_printfs("  (   TEXT  )\r\n"); break;
 				default:   RFM69_printfs("  (UNDEFINED)\r\n"); break;
 			}

 			for (int i = 0; i < 254; i += 16)
 			{
 				RFM69_printfs("Line %3d : ", i);

 				for (int j = 0; j < 16; j++)
 				{
 					if (i + j < 254) {
 						RFM69_printfs("%02X ", trame->payload.data[i + j]);
 					} else {
 						RFM69_printfs("   ");
 					}
 				}

 				RFM69_printfs(" | ");

 				for (int j = 0; j < 16; j++)
 				{
 					if (i + j < 254) {
 						uint8_t c = trame->payload.data[i + j];
 						if (c >= 32 && c <= 126) {
 							RFM69_printfs("%c", c);
 						} else {
 							RFM69_printfs(".");
 						}
 					}
 				}
 				RFM69_printfs(" | ");
 				RFM69_printfs("\r\n");
 			}

 			RFM69_printf(W, "[CRC      ]", "0x%04X\r\n", ((trame->fcs & 0x00FF) << 8) | ((trame->fcs & 0xFF00) >> 8));
 			if( trame->flag_end == 0x7E)
 			{
 				RFM69_printf(G, "[FLAG END]", "0x%02X\r\n", trame->flag_end);
 			}else{
 				RFM69_printf(R, "[FLAG END]", "0x%02X\r\n", trame->flag_end);
 			}
 			CRC_CHECK((const TrameAX *)trame);
 			RFM69_printfs("\033[36m[Payload AX.25]\033[0m\r\n");
 			RFM69_printfs("-----------------------------------------------------------\r\n");
 			RFM69_printfs("\033[36m[RSSI]\033[0m");
 			uint8_t rssi = RFM69_RSSI();


 		}
 	}
 }

