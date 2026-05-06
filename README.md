# STM32 RFM69HCW Driver (HAL Based)

Ce driver en C a été développé pour interfacer un module radio **RFM69HCW** avec un microcontrôleur **STM32** en utilisant les drivers **HAL**. 

## 🚀 Caractéristiques
- **Lecture/Écriture de registres** pour le bus SPI.
- **Code documenté** : Basé directement sur la datasheet officielle.

## 🛠️ Configuration SPI recommandée
Pour une communication stable, les paramètres suivants ont été validés :
- **Mode :** Full-Duplex Master
- **Baudrate :** < 4 MBits/s
- **CPOL :** Low (0)
- **CPHA :** 2 Edge (1) 
- **First Bit :** MSB First

## 💻 Exemple d'utilisation

```c
#include "RFM69HCW.h"

// 1. Initialisation du driver avec l'instance SPI et le port du CS
RFM69_Init(&hspi1, GPIOC, GPIO_PIN_7);

// 2. Vérification de la communication (Lecture du registre de version)
// La valeur attendue pour un RFM69HCW est 0x24
uint8_t version = RFM69_ReadReg(REG_VERSION);

if (version == 0x24) {
    // Communication établie avec succès !
} else {
    // Erreur de câblage ou de configuration SPI
}
```

## 📂 Structure du projet
- `RFM69HCW.h` : Définitions des adresses des registres et prototypes des fonctions.
- `RFM69HCW.c` : Implémentation de la logique de communication SPI.

## 📝 Licence
Ce projet est développé à des fins pédagogiques. Libre à vous de l'utiliser et de l'améliorer !
