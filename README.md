# STM32 RFM69HCW / SX1231 Driver (HAL-Based)

C driver for the **RFM69HCW / SX1231** RF transceiver, designed for **STM32** microcontrollers using the **STM32 HAL** library.

This library provides a simple and efficient library for configuring and controlling the radio module over SPI.


## 🚀 QUICK START 

```c
#include "RFM69HCW.h"

RFM69_Init(&hspi1, GPIOC, GPIO_PIN_7);

RFM69_SetMode(RFM69_MODE_STDBY);

RFM69_SetFrequencyTo433();

RFM69_SetBitrate(4800);

uint8_t key[] = {0x42, 0x24};
RFM69_SetSyncValues(key, 2);

RFM69_SetMode(RFM69_MODE_RX);

```


## ✨ Implemented Features

| Core API | Description | Example 
| :--- | :---: | :--- | 
|  ` RFM69_WriteReg ` | Write a value in a register |  ```RFM69_WriteReg(0x37, 0x90);``` | 
|  `RFM69_ReadReg ` | Read a value in a register |  ```uint8_t currentVal = RFM69_ReadReg(0x02);``` | 

| Radio Initialisation | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_Init` | Initialize SPI interface and Chip Select pin | ```RFM69_Init(&hspi1, GPIOC, GPIO_PIN_7);``` |
| `RFM69_SetMode` | Choice of working mode  |`RFM69_MODE_SLEEP`<br> `RFM69_MODE_FREQUENCYSYNTHER`  <br> `RFM69_MODE_STDBY` <br> `RFM69_MODE_RX` <br> `RFM69_MODE_TX` |
| `RFM69_SetKeyValues_Sync` | Set Key to link two modules. <br> **Have to be the same key on both module to communicate**  <br> ***Max key lenght 8***  |  ```uint8_t mykey[] = {0x42, 0x24};``` <br> ```RFM69_SetSyncValues(mykey, 2);``` |

| Radio Configuration | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_SetFrequencyTo433` | Set Frequency to 433 MHz  |  |
| `RFM69_SetBitrate` | Set bitrate of communication : <br> 4.8 kbps = 4800 | ```RFM69_SetBitrate(4800);```  |
| `RFM69_AutoSetFdev` | Select automaticly the space au frequency depending of the bitrates  |```RFM69_AutoSetFdev();``` |

| Modulation & Packet Engine | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_SetModulationType` | Choice of Modulation  |`RFM69_MODUL_FSK`<br> `RFM69_MODUL_OOK`  |
| `RFM69_SetDataProcessingMode` | Choice of Data Processing  |`RFM69_PACKET_MODE`<br> `RFM69_CONTINUOUS_SYNC`  <br> `RFM69_CONTINUOUS_RAW` |
| `RFM69_SetDataShaping` | Choice of Data Shaping  |`RFM69_SHAPING_NONE`<br> `RFM69_SHAPING_Gaussianfilter_BT1`<br> `RFM69_SHAPING_Gaussianfilter_BT05`<br> `RFM69_SHAPING_Gaussianfilter_BT03`  |

| Transmission | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_PowerAmplifierSelection` | Choice of Power Amplifier <br> ***Pout = PA_... + OutputPower [dBm]***  |`PA_0 => Start from -18 to +13 dbm `<br> `PA_1 => Start from -18 to +13 dbm`<br> `PA_1_2 => Start from -14 to +17 dbm `<br> `PA_HIGH_POWER => Start from -11 to +20 dbm`<br>  |

| Reception | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_AutoMessageDetectionReceive_Config` | Chose type of syncing between two module  |`RFM69_SYNC_OFF`<br> `RFM69_SYNC_ON`<br>`RFM69_FIFO_FILL_IF_SYNC`<br>`RFM69_FIFO_FILL_ALWAYS`<br>`RFM69_SYNC_TOLERANCE_ERROR_0`<br>`RFM69_SYNC_TOLERANCE_ERROR_1`<br>`RFM69_SYNC_TOLERANCE_ERROR_2`<br>`RFM69_SYNC_TOLERANCE_ERROR_3`<br>`RFM69_SYNC_TOLERANCE_ERROR_4`<br>`RFM69_SYNC_TOLERANCE_ERROR_5`<br>`RFM69_SYNC_TOLERANCE_ERROR_6`<br>`RFM69_SYNC_TOLERANCE_ERROR_7`<br> |
| `RFM69_SetLnaImpedance` | Set LNA Impedance (Z) In  | `LNA_Impedance_50`<br> `LNA_Impedance_200`<br> |
| `RFM69_SetLnaGain` | Set LNA Gain  | `LNA_GAIN_AUTO`<br> `LNA_GAIN_G1`<br>`LNA_GAIN_G2`<br>`LNA_GAIN_G3`<br>`LNA_GAIN_G4`<br>`LNA_GAIN_G4`<br>`LNA_GAIN_G5`<br>`LNA_GAIN_G6`<br> |
| `RFM69_GetLnaStatus` | Get LNA Configuration  | |


| Automatic Frequency Correction AFC | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_StartAFC` | Start Frequency Recalibration AFC  ||
| `RFM69_Status_RecalibrationFrequencyAFC` | Get Status of the Automatic Recalibration Frequency Correction AFC  | |
| `RFM69_GetAFCCorrectionHz` | Read AFC Correction in Hz  | |


| Frequency Error Indicator FEI | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_StartFEI` | Start Measurement Of Error Frequency in Hz FEI  | |
| `RFM69_Status_ReadingErrorFrequency` | Get Status of the Reading Error Frequency FEI  |  |
| `RFM69_GetFrequencyErrorFeiHz` | Read Error Frequency in Hz FEI |  |


| Diagnostics | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_RSSI` | Visualize RSSI in dBm  | Return an uint8_t ; |
| `RFM69_getConfigData` | Get all parameters set in one command  |  |
| `RFM69_SetOverloadCurrentProtection` | Enable or disable the overload current protection for the PA  |  |

---

## 💻 SPI recommanded configuration

The SPI was configured as follows:

- **Frame Format :** Motorola
- **Mode :** Full-Duplex Master  
- **CPOL :** Low   
- **CPHA :** 2 Edge  
- **First Bit :** MSB First  

---
## ⚠️ Important notes
- Key Sync have to be the same on each module
- Bitrates have to be the same on each module
  
## 📂 Structure of the project
- `RFM69HCW.h` : Definitions of register addresses and function prototypes.
- `RFM69HCW.c` : Implémentation function in C.

## Useful Documents
**[Datasheet RFM69HCW](https://cdn.sparkfun.com/datasheets/Wireless/General/RFM69HCW-V1.1.pdf)**

## 📝 Licence
EN : This project was developed for educational purposes. Feel free to use and improve it!
FR : Ce projet est développé à des fins pédagogiques. Libre à vous de l'utiliser et de l'améliorer !
