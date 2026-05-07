# STM32 RFM69HCW/SX1231 Driver (Basée sur HAL)

This C driver was developed to quickly program the **RFM69HCW** radio module using an **STM32** microcontroller.

This driver uses **HAL**.

---

## ✅Currently implemented features

| Fonction | Use | Example |
| :--- | :---: | :--- |
| **RFM69_Init** | Initialisation des pins SPI (Chip Select, SPI#) | RFM69_Init(&hspi1, GPIOC, GPIO_PIN_7); |
| **RFM69_WriteReg** | Write a value in any register | RFM69_WriteReg(0x37, 0x90); |
| **RFM69_ReadReg** | Read a value in any register | uint8_t currentVal = RFM69_ReadReg(0x02); |
| **RFM69_SetMode** | Choice of working mode  |`RFM69_MODE_SLEEP`<br> `RFM69_MODE_FREQUENCYSYNTHER`  <br> `RFM69_MODE_STDBY` <br> `RFM69_MODE_RX` <br> `RFM69_MODE_TX` |
| **RFM69_SetDataProcessingMode** | Choice of Data Processing  |`RFM69_PACKET_MODE`<br> `RFM69_CONTINUOUS_SYNC`  <br> `RFM69_CONTINUOUS_RAW` |
| **RFM69_SetModulationType** | Choice of Modulation  |`RFM69_MODUL_FSK`<br> `RFM69_MODUL_OOK`  |
| **RFM69_SetDataShaping** | Choice of Data Shaping  |`RFM69_SHAPING_NONE`<br> `RFM69_SHAPING_Gaussianfilter_BT1`<br> `RFM69_SHAPING_Gaussianfilter_BT05`<br> `RFM69_SHAPING_Gaussianfilter_BT03`  |
| **RFM69_SetFrequencyTo433** | Set Frequency to 433  | Nothing (void) |
| **RFM69_SetBitrate** | Set bitrate of communication : 4.8 kbps = 4800 | RFM69_SetBitrate(4800);  |
| **RFM69_ConfigSync** | Chose type of syncing between two module  |`RFM69_SYNC_OFF`<br> `RFM69_SYNC_ON`<br>`RFM69_FIFO_FILL_IF_SYNC`<br>`RFM69_FIFO_FILL_ALWAYS`<br>`RFM69_SYNC_TOLERANCE_ERROR_0`<br>`RFM69_SYNC_TOLERANCE_ERROR_1`<br>`RFM69_SYNC_TOLERANCE_ERROR_2`<br>`RFM69_SYNC_TOLERANCE_ERROR_3`<br>`RFM69_SYNC_TOLERANCE_ERROR_4`<br>`RFM69_SYNC_TOLERANCE_ERROR_5`<br>`RFM69_SYNC_TOLERANCE_ERROR_6`<br>`RFM69_SYNC_TOLERANCE_ERROR_7`<br> |
| **RFM69_SetSyncValues** | Set Key to link two modules. <br> **Have to be the same key on both module to communicate**  <br> **Max key lenght 8**  |  uint8_t mykey[] = {0x42, 0x24}; <br> RFM69_SetSyncValues(mykey, 2); |
| **RFM69_AutoSetFdev** | Select auto the space au frequency depending of the bitrates  |RFM69_AutoSetFdev(); |
| **RFM69_getConfigData** | Get all parameters set in one command  | Nothing (void) |

## 🛠️features on work

| Fonction | Use | Example |
| :--- | :---: | :--- |
| **RFM69_SetPacketConfig** |Select a flexible lenght for message |RFM69_AutoSetFdev(); |

---

## 💻 SPI recommanded configuration

The SPI was configured as follows:

- **Frame Format :** Motorola
- **Mode :** Full-Duplex Master  
- **CPOL :** Low   
- **CPHA :** 2 Edge  
- **First Bit :** MSB First  

---

## 🚀 Integration into a new project

```c
#include "RFM69HCW.h"

int main(void)
{
      RFM69_Init(&hspi1, GPIOC, GPIO_PIN_7);
      while (1)
      {
            uint8_t version = RFM69_ReadReg(RegVersion);
            if (version == 0x24) {
                // Communication établie avec succès !
            } else {
                // Erreur de câblage ou de configuration SPI
            }
      }
}
```
## 📂 Structure of the project
- `RFM69HCW.h` : Definitions of register addresses and function prototypes.
- `RFM69HCW.c` : Implémentation function in C.

## Useful Documents
Datasheet RFM69HCW : https://cdn.sparkfun.com/datasheets/Wireless/General/RFM69HCW-V1.1.pdf

## 📝 Licence
EN : This project was developed for educational purposes. Feel free to use and improve it!
FR : Ce projet est développé à des fins pédagogiques. Libre à vous de l'utiliser et de l'améliorer !
