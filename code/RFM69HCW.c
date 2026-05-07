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
    tx[1] = 0x00;        // unused

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
	  RFM69_WriteReg(0x07, 0x6C);
	  RFM69_WriteReg(0x08, 0x40);
	  RFM69_WriteReg(0x09, 0x00);
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
 * Data Processing Mode
 */
void RFM69_SetDataProcessingMode(RFM69_DataProcessingMode_t mode)
{
    uint8_t currentVal = RFM69_ReadReg(0x02);
    uint8_t newVal = (currentVal & 0x9F) | (uint8_t)mode;
    RFM69_WriteReg(0x02, newVal);
}

/*
 * Modulation Type
 */
void RFM69_SetModulationType(RFM69_Modulation_t modulation)
{
    uint8_t currentVal = RFM69_ReadReg(0x02);
    uint8_t newVal = (currentVal & 0xE7) | (uint8_t)modulation;

    RFM69_WriteReg(0x02, newVal);
}

/*
 * DataShaping
 */
void RFM69_SetDataShaping(RFM69_Data_Shaping_t shaping)
{
    uint8_t currentVal = RFM69_ReadReg(0x02);
    uint8_t modulation = (currentVal & 0x18); // On récupère la modulation actuelle pour la sécurité

    // Sécurité pour le mode OOK
    if (modulation == RFM69_MODUL_OOK && shaping == RFM69_SHAPING_Gaussianfilter_BT03) {
        printf("Warning: Shaping BT0.3 incompatible with OOK. Forcing NONE.\r\n");
        shaping = RFM69_SHAPING_NONE;
    }

    // Effacer les anciens bits 1-0 (Masque 0xFC = 1111 1100) puis appliquer le shaping
    uint8_t newVal = (currentVal & 0xFC) | (uint8_t)shaping;

    RFM69_WriteReg(0x02, newVal);
}

/*
 *  Set Bitrate
 */
void RFM69_SetBitrate(uint32_t bitrate)
{
    uint8_t msb = 0x1A; // Valeur par défaut (4.8kbps)
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
            printf("Warning: Bitrate non standard. Utilisation de 4.8kbps par defaut.\r\n");
            break;
    }

    RFM69_WriteReg(0x03, msb);
    RFM69_WriteReg(0x04, lsb);
}

/*
 * Automatic verification of word received
 */
void RFM69_AutoMessageDetectionReceive_Config(RFM69_SyncState_t state, RFM69_FifoFill_t fill, uint8_t size_of_key, RFM69_SyncTol_t tol)
{
    if (size_of_key < 1) size_of_key = 1;
    if (size_of_key > 8) size_of_key = 8;

    uint8_t config = (uint8_t)state | (uint8_t)fill | ((size_of_key - 1) << 3) | (uint8_t)tol;

    RFM69_WriteReg(0x2E, config);
}


/*
 * Word for synchronisation
 */
void RFM69_SetKeyValues_Sync(uint8_t *key, uint8_t len)
{
    if(len > 8) len = 8;

    for(uint8_t i = 0; i < len; i++)
    {
        RFM69_WriteReg(0x2F + i, key[i]);
    }
}

/*
 * AutoSetFdev for FSK
 */
void RFM69_AutoSetFdev(void)
{
    uint8_t msb_b = RFM69_ReadReg(0x03);
    uint8_t lsb_b = RFM69_ReadReg(0x04);
    uint16_t bitReg = (uint16_t)((msb_b << 8) | lsb_b);

    if (bitReg == 0) return;

    uint32_t currentBitrate = 32000000 / bitReg;
    uint32_t fdevHz = (currentBitrate * 125) / 100;
    uint32_t valReg = (fdevHz * 1000) / 61035;

    if (valReg > 0x3FFF) valReg = 0x3FFF;

    uint8_t msb = RFM69_ReadReg(0x05);
    msb &= 0xC0;
    msb |= (uint8_t)(valReg >> 8);

    RFM69_WriteReg(0x05, msb);
    RFM69_WriteReg(0x06, (uint8_t)(valReg & 0xFF));
}

/*
 * SetAutomatiquePacketConfig
 */

//WARNING A FINIR
//WARNING A FINIR
//WARNING A FINIR
//WARNING A FINIR

void RFM69_SetPacketConfig(void)
{
    RFM69_WriteReg(0x37, 0x90);
}

/*
 * Send Message
 */
void RFM69_SendMessage(uint8_t* payload, uint8_t len)
{
	printf("\r\n--- Sending Message ---\r\n");
    RFM69_SetMode(RFM69_MODE_STDBY);
    HAL_Delay(5);
    RFM69_SetMode(RFM69_MODE_SLEEP);
    RFM69_SetMode(RFM69_MODE_STDBY);

    RFM69_WriteReg(0x3C, 0x8F);//When send message
	RFM69_WriteReg(0x28, 0x10); //Flush FIFO

    if (len > 64) // Can be extended to 66 maximum
    {
        printf("ERROR_SEND : Len of message exceed 64\r\n");
        abord();
        RFM69_SetMode(RFM69_MODE_STDBY);
        return;
    }

    RFM69_WriteReg(0x00, len);
    for (uint8_t i = 0; i < len; i++) { // Can be optimized with DMA
        RFM69_WriteReg(0x00, payload[i]);
    }

    RFM69_SetMode(RFM69_MODE_FS);

    uint32_t tick = HAL_GetTick();
    while (!(RFM69_ReadReg(0x27) & PLLock)) {
        if ((HAL_GetTick() - tick) > 50)
		{
        	printf("ERROR_SEND : PLL never locked\r\n");
        	abord();
        	RFM69_SetMode(RFM69_MODE_STDBY);
        	return;
		}
    }

    RFM69_SetMode(RFM69_MODE_TX);


    tick = HAL_GetTick();
    while (!(RFM69_ReadReg(0x28) & PacketSend)) {
        if ((HAL_GetTick() - tick) > TimeoutPacketNotSend) {
            printf("ERROR_SEND: Nothing Send\r\n");
            abord();
            break;
        }
    }

    if (RFM69_ReadReg(0x28) & PacketSend)
    {
        printf("Message Send\r\n");
    }

    RFM69_SetMode(RFM69_MODE_STDBY);
    printf("\r\n---  ---\r\n");
}

/*
 * ABORD MESSAGE
 */
void abord(void)
{
	printf("ABORD\r\n");
}

/*
 * Message Well Send
 */
void RFM69_DEBUG_MessageSendOK(void)
{
    uint8_t flags = RFM69_ReadReg(0x28);

    if (flags & 0x08) {
        printf("Message Send with Succes : bit PacketSent up to 1.\r\n");
    }
}


/*
 * Read Message
 */
uint8_t RFM69_ReceiveMessage(uint8_t* buffer, uint8_t maxLen)
{
 // Next Step
	return 0;
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
        printf("Operation Mode : TRANSMITTING\r\n");
    } else if(opMode == RFM69_MODE_RX) {
        printf("Operation Mode : RECEIVEING\r\n");
    }

    uint8_t msb = RFM69_ReadReg(0x07);
    uint8_t mid = RFM69_ReadReg(0x08);
    uint8_t lsb = RFM69_ReadReg(0x09);
    uint32_t frf = (uint32_t)((msb << 16) | (mid << 8) | lsb);
    uint64_t freqHz = ((uint64_t)frf * 6103515625ULL) / 100000000ULL;
    uint32_t mhz = (uint32_t)(freqHz / 1000000);
    uint32_t khz = (uint32_t)((freqHz % 1000000) / 1000);
    printf("Fréquence : %lu.%03lu MHz\r\n", (unsigned long)mhz, (unsigned long)khz);


    uint8_t msb_b = RFM69_ReadReg(0x03);
    uint8_t lsb_b = RFM69_ReadReg(0x04);
    uint16_t bitReg = (uint16_t)((msb_b << 8) | lsb_b);
    uint32_t currentBitrate = (bitReg == 0) ? 0 : (32000000 / bitReg);
    printf("Bitrate        : %lu bps\r\n", (unsigned long)currentBitrate);

    printf("------------------------------\r\n");

}

