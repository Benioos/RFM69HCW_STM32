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

    /* WARNING NEED TO BE ERASE IN A FINAL PROJECT*/
    /* WARNING NEED TO BE ERASE IN A FINAL PROJECT*/
    /* WARNING NEED TO BE ERASE IN A FINAL PROJECT*/
    if(RFM69_ReadReg(reg) != data)
    {
    	printf("Error Writing Register\r\n");
    }
}

/*
 * Read 1 value from a register
 */

uint8_t RFM69_ReadReg(uint8_t reg)
{
    uint8_t tx[2];
    uint8_t rx[2];

    tx[0] = reg & 0x7F;  // read (bit7 = 0)
    tx[1] = 0x00;        // unused

    HAL_GPIO_WritePin(rfm69_cs_port, rfm69_cs_pin, GPIO_PIN_RESET);

    HAL_SPI_TransmitReceive(rfm69_hspi, tx, rx, 2, HAL_MAX_DELAY);

    HAL_GPIO_WritePin(rfm69_cs_port, rfm69_cs_pin, GPIO_PIN_SET);

    return rx[1];
}

/*
 * Frequency Choice
 */
void RFM69_SetFrequency(uint32_t freqMHz)
{
    // On ignore l'argument pour l'instant et on force 433 MHz
    // pour correspondre à l'antenne et au filtrage du module.
    uint32_t freqForced = 433;

    uint32_t frf = (uint32_t)((double)freqForced * 1000000.0 / 61.03515625);

    RFM69_WriteReg(RegFrfMsb, (uint8_t)(frf >> 16));
    RFM69_WriteReg(RegFrfMid, (uint8_t)(frf >> 8));
    RFM69_WriteReg(RegFrfLsb, (uint8_t)(frf));
}

/*
 * Mode Choice
 */
void RFM69_SetMode(RFM69_Mode_t mode)
{
    RFM69_WriteReg(0x01, (uint8_t)mode);

    if (mode != RFM69_MODE_SLEEP) {
        HAL_Delay(1);
    }
}

/*
 * Modulation Choice
 */
void RFM69_SetModulationType(RFM69_DataProcessingMode_t mode, RFM69_Modulation_t modulation, RFM69_Data_Shaping_t shaping)
{
    /* WARNING NEED TO BE ERASE IN A FINAL PROJECT*/
    /* WARNING NEED TO BE ERASE IN A FINAL PROJECT*/
    /* WARNING NEED TO BE ERASE IN A FINAL PROJECT*/
    if (modulation == RFM69_MODUL_OOK && shaping == RFM69_SHAPING_Gaussianfilter_BT03) {
    	printf("This type of shaping is not compatible with OOK modulation\r\n");
    	shaping = RFM69_SHAPING_NONE;
    	printf("Shaping reset to none... \r\n");
    }

    uint8_t regVal = (uint8_t)mode | (uint8_t)modulation | (uint8_t)shaping;
    RFM69_WriteReg(0x02, regVal);
}

/*
 * Get information of Configuration
 */
void getConfigData()
{
	uint8_t val = RFM69_ReadReg(0x02);

	uint8_t mode = (val & 0x60);
	uint8_t modulation = (val & 0x18);
	uint8_t shaping = (val & 0x03);

	printf("--- Configuration RFM69HCW ---\r\n");

	if(mode == RFM69_PACKET_MODE)
	{
		printf("Mode : PACKET\r\n");
	}else if(mode == RFM69_CONTINUOUS_SYNC){
		printf("Mode : CONTINUOUS\r\n");
	}else if(mode == RFM69_CONTINUOUS_RAW){
		printf("Mode : RAW\r\n");
	}

	if(modulation == RFM69_MODUL_FSK)
	{
		printf("Modulation : FSK\r\n");
	}else if(modulation == RFM69_MODUL_OOK){
		printf("Modulation : OOK\r\n");
	}

	if(shaping == RFM69_SHAPING_NONE)
	{
		printf("Shaping : None\r\n");
	}else if(shaping == RFM69_SHAPING_Gaussianfilter_BT1){
		printf("Shaping : Gaussian Filtre BT=1.0\r\n");
	}else if(shaping == RFM69_SHAPING_Gaussianfilter_BT05){
		printf("Shaping : Gaussian Filtre BT=0.5\r\n");
	}else if(shaping == RFM69_SHAPING_Gaussianfilter_BT03){
		printf("Shaping : Gaussian Filtre BT=0.3\r\n");
	}

    val = RFM69_ReadReg(0x01);
    uint8_t opMode = (val & 0x1C);
    if(opMode == RFM69_MODE_SLEEP) {
        printf("Operation Mode : SLEEP\r\n");
    } else if(opMode == RFM69_MODE_STDBY) {
        printf("Operation Mode : STANDBY\r\n");
    } else if(opMode == RFM69_MODE_TX) {
        printf("Operation Mode : TRANSMIT\r\n");
    } else if(opMode == RFM69_MODE_RX) {
        printf("Operation Mode : RECEIVE\r\n");
    }

    uint8_t msb = RFM69_ReadReg(0x07);
    uint8_t mid = RFM69_ReadReg(0x08);
    uint8_t lsb = RFM69_ReadReg(0x09);

    uint32_t frf = (uint32_t)((msb << 16) | (mid << 8) | lsb);

    uint64_t freqHz = ((uint64_t)frf * 6103515625ULL) / 100000000ULL;

    uint32_t mhz = (uint32_t)(freqHz / 1000000);
    uint32_t khz = (uint32_t)((freqHz % 1000000) / 1000);

    printf("Fréquence : %lu.%03lu MHz\r\n", (unsigned long)mhz, (unsigned long)khz);

    printf("------------------------------\r\n");

}

