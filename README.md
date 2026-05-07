# STM32 RFM69HCW/SX1231 Driver (Basée sur HAL)

Ce driver, en C, a été développé pour programmer rapidement le module radio **RFM69HCW** avec un microcontrôleur **STM32**.
Ce driver utilise **HAL**.

---

## 🛠️Fonctions implémentées actuellement

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
| **RFM69_SetBitrate** | Set bitrate of communication  | RFM69_SetBitrate(4800); |
| **RFM69_getConfigData** | Get all parameters set in one command  | Nothing (void) |


---

## 💻 Configuration SPI recommandée

Le SPI a été configurée de la façon suivant:

- **Frame Format :** Motorola
- **Mode :** Full-Duplex Master  
- **CPOL :** Low   
- **CPHA :** 2 Edge  
- **First Bit :** MSB First  

---

## 🚀 Intégration dans un nouveau projet

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
## 📂 Structure du projet
- `RFM69HCW.h` : Définitions des adresses des registres et prototypes des fonctions.
- `RFM69HCW.c` : Implémentation de la logique de communication SPI.

## Document utilisé
Datasheet RFM69HCW : https://cdn.sparkfun.com/datasheets/Wireless/General/RFM69HCW-V1.1.pdf

## 📝 Licence
Ce projet est développé à des fins pédagogiques. Libre à vous de l'utiliser et de l'améliorer !
