# STM32 RFM69HCW/SX1231 Driver (Basée sur HAL)

Ce driver, en C, a été développé pour programmer rapidement le module radio **RFM69HCW** avec un microcontrôleur **STM32**.
Ce driver utilise **HAL**.

---

## 🛠️Fonctions implémentées actuellement

**RFM69_Init**  -- Initialisation des pins SPI (Chip Select, SPI#)

**RFM69_WriteReg**  -- Écriture dans un registre  

**RFM69_SetMode**  -- Choix du mode de fonctionnement

**RFM69_SetModulationType**  -- Choix modulation

**RFM69_SetFrequency**  -- Choix Fréquence d'émission et réception

**RFM69_getConfigData**  -- Aperçu de tous les paramètres configurés

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
