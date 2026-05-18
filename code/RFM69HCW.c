/*
 * RFM69GCW.c
 *
 *  Created on: May 6, 2026
 *      Author: benja
 */

#include "RFM69HCW.h"

static SPI_HandleTypeDef *rfm69_hspi;
static GPIO_TypeDef *rfm69_cs_port;
static uint16_t rfm69_cs_pin;

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
 * Frequency Choice
 */
void RFM69_SetFrequencyTo433(void)
{
	  RFM69_WriteReg(RegFrfMsb, 0x6C);
	  RFM69_WriteReg(RegFrfMid, 0x40);
	  RFM69_WriteReg(RegFrfLsb, 0x00);
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
 * Data Processing Mode
 */
void RFM69_SetDataProcessingMode(RFM69_DataProcessingMode_t mode)
{
    uint8_t currentVal = RFM69_ReadReg(RegDataModul);
    uint8_t newVal = (currentVal & 0x9F) | (uint8_t)mode;
    RFM69_WriteReg(RegDataModul, newVal);
}

/*
 * Modulation Type
 */
void RFM69_SetModulationType(RFM69_Modulation_t modulation)
{
    uint8_t currentVal = RFM69_ReadReg(RegDataModul);
    uint8_t newVal = (currentVal & 0xE7) | (uint8_t)modulation;

    RFM69_WriteReg(RegDataModul, newVal);
}

/*
 * DataShaping
 */
void RFM69_SetDataShaping(RFM69_Data_Shaping_t shaping)
{
    uint8_t currentVal = RFM69_ReadReg(RegDataModul);
    uint8_t modulation = (currentVal & 0x18);

    if (modulation == RFM69_MODUL_OOK && shaping == RFM69_SHAPING_Gaussianfilter_BT03) {
    	RFM69_printf("\r\n !Warning: Shaping BT0.3 incompatible with OOK. Forcing NONE.\r\n");
        shaping = RFM69_SHAPING_NONE;
    }

    uint8_t newVal = (currentVal & 0xFC) | (uint8_t)shaping;

    RFM69_WriteReg(RegDataModul, newVal);
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
        	RFM69_printf("\r\n !Warning: Bitrate not standard. Default 4.8kbps.\r\n");
            break;
    }

    RFM69_WriteReg(RegBitrateMsb, msb);
    RFM69_WriteReg(RegBitrateLsb, lsb);
}

/*
 * Automatic verification of word received
 */
void RFM69_AutoMessageDetectionReceive_Config(RFM69_SyncState_t state, RFM69_FifoFill_t fill, uint8_t size_of_key, RFM69_SyncTol_t tol)
{
    if (size_of_key < 1) size_of_key = 1;
    if (size_of_key > 8) size_of_key = 8;

    uint8_t config = (uint8_t)state | (uint8_t)fill | ((size_of_key - 1) << 3) | (uint8_t)tol;

    RFM69_WriteReg(RegSyncConfig, config);
}


/*
 * Word for Synchronization
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
 * SetAutomatiquePacketConfig
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
 * Flush FIFO
 */
void RFM69_FlushFIFO(void)
{
	RFM69_WriteReg(FIFO, 0x10);
}

/*
 * Power Amplifier Selection
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
        	RFM69_printf("\r\n !This function haven't been implemented yet.\r\n");
        	RFM69_printf("\r\n !Abord, Nothing changed.\r\n");
            break;
    }

    if (outputPower < 0)  outputPower = 0;
    if (outputPower > 31) outputPower = 31;

    regVal |= (uint8_t)outputPower;

    RFM69_WriteReg(RegPaLevel, regVal);
}

/*
 * Send Message
 */
void RFM69_SendMessage_Packet_Mode(uint8_t* payload, uint8_t len)
{
	RFM69_printf("\r\n--- Packet Mode Sending Message ---\r\n");

    RFM69_SetMode(RFM69_MODE_STDBY);
    RFM69_FlushFIFO();

    if (len > 64) // Can be extended to 66 maximum
    {
    	RFM69_printf("\r\n !ERROR_SEND : Len of message exceed 64\r\n");
    	RFM69_printf("\r\n !ABORD \r\n");
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
        	RFM69_printf("\r\n !ERROR_SEND : PLL never locked\r\n");
        	RFM69_printf("\r\n !ABORD\r\n");
        	RFM69_SetMode(RFM69_MODE_STDBY);
        	return;
		}
    }

    RFM69_SetMode(RFM69_MODE_TX);

    tick = HAL_GetTick();
    while (!(RFM69_ReadReg(RegIrqFlagsPLL) & 0x80)) {
        if ((HAL_GetTick() - tick) > 20) {
        	RFM69_printf("\r\n !ERROR_SEND: Mode TX not Ready\r\n");
        	RFM69_printf("\r\n !ABORD\r\n");
        	RFM69_SetMode(RFM69_MODE_STDBY);
            break;
        }
    }

    tick = HAL_GetTick();
    while (!(RFM69_ReadReg(RegIrqFlagsFIFO) & PacketSend)) {
        if ((HAL_GetTick() - tick) > TimeoutPacketNotSend) {
        	RFM69_printf("\r\n !ERROR_SEND: Packet Send Control down 0.\r\n");
        	RFM69_printf("\r\n !ABORD\r\n");
        	RFM69_SetMode(RFM69_MODE_STDBY);
            break;
        }
    }

    RFM69_printf("\r\n !Message Send With Success\r\n");

    RFM69_SetMode(RFM69_MODE_STDBY);

    RFM69_printf("\r\n---  ---\r\n");
}

/*
 * Read Message/Packet
 */
uint8_t RFM69_ReceiveMessage_Packet_Mode(uint8_t* buffer, uint8_t maxLen)
{
	RFM69_printf("\r\n --- Packet Mode Reception Message ---\r\n");
    RFM69_WriteReg(RegIrqFlagsFIFO, 0x10);

    RFM69_SetMode(RFM69_MODE_RX);

    uint8_t rssi = RFM69_ReadReg(RegRssiValue);
    RFM69_printf("\r\n Ambient Noise (RSSI): -%d dBm\r\n", rssi/2);

    uint32_t tick = HAL_GetTick();
    while (!(RFM69_ReadReg(RegIrqFlagsFIFO) & 0x04)) {
        if ((HAL_GetTick() - tick) > TimeoutNoPacketReceived)
        {
        	RFM69_printf("\r\n RX Timeout: No packet received\r\n");
            RFM69_SetMode(RFM69_MODE_STDBY);
            return 0;
        }
    }

    uint8_t payloadLen = RFM69_ReadReg(RegFIFO);
    uint8_t bytesToRead = (payloadLen > maxLen) ? maxLen : payloadLen;

    for (uint8_t i = 0; i < bytesToRead; i++) {
        buffer[i] = RFM69_ReadReg(RegFIFO);
    }

    RFM69_printf("\r\n Packet Received [%d bytes]: \r\n ", bytesToRead);
    for (uint8_t i = 0; i < bytesToRead; i++) {
    	RFM69_printf("%c \r\n", buffer[i]);
    }

    RFM69_SetMode(RFM69_MODE_STDBY);
    RFM69_WriteReg(RegIrqFlagsFIFO, 0x10);

    return bytesToRead;
}


/*
 * Read RSSI in dBm
 */
uint8_t RFM69_RSSI(void)
{
	uint8_t rssi = RFM69_ReadReg(RegRssiValue);
	RFM69_printf("\r\n Ambient Noise (RSSI): -%d dBm\r\n", rssi/2);
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

    RFM69_printf("\r\n --- LNA Status ---\r\n");
    RFM69_printf("\r\n Impedance : %s\r\n", (zin == 0x80) ? "200 Ohms" : "50 Ohms");

    if (set == 0) printf("\r\n Mode : AUTOMATIC (AGC)\r\n");
    else          printf("\r\n Mode : MANUAL (G%d force)\r\n", set);

    RFM69_printf("\r\n Current Gain : G%d\r\n", real);
    RFM69_printf("\r\n ------------------\r\n");
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
		RFM69_printf("\r\n !Incorrect mode, must be in Standby Mode");
		RFM69_printf("\r\n !Abord");
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
void RFM69_getConfigData()
{
	uint8_t val = RFM69_ReadReg(0x02);

	uint8_t mode = (val & 0x60);
	uint8_t modulation = (val & 0x18);
	uint8_t shaping = (val & 0x03);

	RFM69_printf("\r\n --- Config Data RFM69HCW ---\r\n");

	if(mode == RFM69_PACKET_MODE)
	{
		RFM69_printf("Mode : PACKET\r\n");
	}else if(mode == RFM69_CONTINUOUS_SYNC){
		RFM69_printf("Mode : CONTINUOUS\r\n");
	}else if(mode == RFM69_CONTINUOUS_RAW){
		RFM69_printf("Mode : RAW\r\n");
	}

	if(modulation == RFM69_MODUL_FSK)
	{
		RFM69_printf("Modulation : FSK\r\n");
	}else if(modulation == RFM69_MODUL_OOK){
		RFM69_printf("Modulation : OOK\r\n");
	}

	if(shaping == RFM69_SHAPING_NONE)
	{
		RFM69_printf("Shaping : None\r\n");
	}else if(shaping == RFM69_SHAPING_Gaussianfilter_BT1){
		RFM69_printf("Shaping : Gaussian Filtre BT=1.0\r\n");
	}else if(shaping == RFM69_SHAPING_Gaussianfilter_BT05){
		RFM69_printf("Shaping : Gaussian Filtre BT=0.5\r\n");
	}else if(shaping == RFM69_SHAPING_Gaussianfilter_BT03){
		RFM69_printf("Shaping : Gaussian Filtre BT=0.3\r\n");
	}

    val = RFM69_ReadReg(0x01);
    uint8_t opMode = (val & 0x1C);
    if(opMode == RFM69_MODE_SLEEP) {
    	RFM69_printf("Operation Mode : SLEEP\r\n");
    } else if(opMode == RFM69_MODE_STDBY) {
    	RFM69_printf("Operation Mode : STANDBY\r\n");
    } else if(opMode == RFM69_MODE_TX) {
    	RFM69_printf("Operation Mode : TRANSMITTING\r\n");
    } else if(opMode == RFM69_MODE_RX) {
    	RFM69_printf("Operation Mode : RECEIVEING\r\n");
    }

    uint8_t msb = RFM69_ReadReg(0x07);
    uint8_t mid = RFM69_ReadReg(0x08);
    uint8_t lsb = RFM69_ReadReg(0x09);
    uint32_t frf = (uint32_t)((msb << 16) | (mid << 8) | lsb);
    uint64_t freqHz = ((uint64_t)frf * 6103515625ULL) / 100000000ULL;
    uint32_t mhz = (uint32_t)(freqHz / 1000000);
    uint32_t khz = (uint32_t)((freqHz % 1000000) / 1000);
    RFM69_printf("Frequency : %lu.%03lu MHz\r\n", (unsigned long)mhz, (unsigned long)khz);

    uint8_t msb_b = RFM69_ReadReg(0x03);
    uint8_t lsb_b = RFM69_ReadReg(0x04);
    uint16_t bitReg = (uint16_t)((msb_b << 8) | lsb_b);
    uint32_t currentBitrate = (bitReg == 0) ? 0 : (32000000 / bitReg);
    RFM69_printf("Bitrate    : %lu bps\r\n", (unsigned long)currentBitrate);

    RFM69_printf("------------------------------\r\n");

}

/*
 * Display trame for Debug !
 */
void display_trame(const TrameAX *trame) {

    const uint8_t *ptr = (const uint8_t *)trame;
    size_t taille = sizeof(TrameAX);

    RFM69_printf("\r\n --- Display TRAME (%d octets) ---\r\n", (int)taille);
    RFM69_printf("\r\nFlag/Address/Control:\r\n");

    size_t i = 0;
    while (i < taille) {

        if (i == 16) {
        	RFM69_printf("\r\n- RAW DATA -\r\n");
        }

        size_t debut_bloc = i;
        size_t taille_bloc = 0;

        for (size_t j = 0; j < 16; j++) {
            if ((i + j) >= taille) break;
            if ((i + j) == 16 && j > 0) break;
            if ((i + j) == 272 && j > 0) break;
            taille_bloc++;
        }

        for (size_t j = 0; j < taille_bloc; j++) {
        	RFM69_printf("%02X ", ptr[debut_bloc + j]);
        }

        for (size_t j = taille_bloc; j < 16; j++) {
        	RFM69_printf("   ");
        }

        if (debut_bloc >= 16 && debut_bloc < 272) {
        	RFM69_printf(" |  ");
            for (size_t j = 0; j < taille_bloc; j++) {
                uint8_t c = ptr[debut_bloc + j];
                if (isprint(c)) {
                	RFM69_printf("%c", c);
                } else {
                	RFM69_printf(".");
                }
            }
        }

        RFM69_printf("\r\n");
        i += taille_bloc;

        if (i == 272) {
        	RFM69_printf("\r\n- END of RAW DATA -\r\n");
        }
    }

    RFM69_printf("\r\n ------------------------\r\n");
}

/*
 * Fill Up Data Buffer
 */
int Fill_Up_Data(TrameAX *trame, const char *message) {

    size_t Size_Of_Data = strlen(message);

    if (Size_Of_Data > 256) {
    	RFM69_printf("\r\n ERROR : This message is way too long. (%d). Maximum = 256.\r\n", (int)Size_Of_Data);
        return -1;
    }

    memset(trame->data, 0, 256);
    memcpy(trame->data, message, Size_Of_Data);

    RFM69_printf("\r\n !Packet Loaded With Success.\r\n");

    return 0;
}
