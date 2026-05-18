# STM32 RFM69HCW / SX1231 Driver (HAL-Based)

This is a C driver for the **RFM69HCW / SX1231** RF transceiver, designed for **STM32** microcontrollers using the **STM32 HAL** library.

This library provides a way of controlling the radio module over SPI.


<img src="https://cdn-shop.adafruit.com/970x728/3070-07.jpg" alt="Adafruit RFM69HCW Transceiver Radio" width="260">

## 💻 SPI recommended configuration

The SPI was configured as follows:

- **Frame Format :** Motorola
- **Mode :** Full-Duplex Master  
- **CPOL :** Low   
- **CPHA :** 2 Edge  
- **First Bit :** MSB First  

## ✨ Implemented Features

### 🔧 Core API 

- `RFM69_WriteReg` → Write a register value
- `RFM69_ReadReg` → Read a register value

<details>
<summary> Full reference </summary>

| Core API | Description | Example |
| :--- | :---: | :--- | 
|  ` RFM69_WriteReg ` | Write a value in a register |  ```RFM69_WriteReg(0x37, 0x90);``` | 
|  `RFM69_ReadReg ` | Read a value in a register |  ```uint8_t currentVal = RFM69_ReadReg(0x02);``` | 

</details>

### ⚙️ Radio Initialisation

- `RFM69_Init` → Initialize SPI interface and Chip Select pin
- `RFM69_SetMode` → Select operating mode type
- `RFM69_GetMode` → Get operating mode
- `RFM69_SetKeyValues_Sync` → Set Key to link two modules.

<details>
<summary> Full reference </summary>

| Radio Initialization| Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_Init` | Initialize SPI interface and Chip Select pin | ```RFM69_Init(&hspi1, GPIOC, GPIO_PIN_7);``` |
| `RFM69_SetMode` | Select operating mode type  |`RFM69_MODE_SLEEP`<br> `RFM69_MODE_FREQUENCYSYNTHER`  <br> `RFM69_MODE_STDBY` <br> `RFM69_MODE_RX` <br> `RFM69_MODE_TX` |
| `RFM69_GetMode` | Get operating mode  | |
| `RFM69_SetKeyValues_Sync` | Set Key to link two modules. <br> **Must be identical on both modules**  <br> ***Max key length 8***  |  ```uint8_t mykey[] = {0x42, 0x24};``` <br> ```RFM69_SetSyncValues(mykey, 2);``` |

</details>

### 📡 Radio Configuration

- `RFM69_SetFrequencyTo433` → Set Frequency to 433 MHz
- `RFM69_SetBitrate` → Set bitrate of communication : 4.8 kbps = 4800
- `RFM69_AutoSetFdev` → Automatically adjusts frequency deviation based on bitrate

<details>
<summary> Full reference </summary>

| Radio Configuration | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_SetFrequencyTo433` | Set Frequency to 433 MHz  |  |
| `RFM69_SetBitrate` | Set bitrate of communication : <br> 4.8 kbps = 4800 | ```RFM69_SetBitrate(4800);```  |
| `RFM69_AutoSetFdev` | Automatically adjusts frequency deviation based on bitrate  |```RFM69_AutoSetFdev();``` |

</details>

### 📶 Modulation & Packet Engine

- `RFM69_SetModulationType` → Select modulation type
- `RFM69_SetDataProcessingMode` → Select Data Processing type
- `RFM69_SetDataShaping` → Select Data Shaping type

<details>
<summary> Full reference </summary>

| Modulation & Packet Engine | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_SetModulationType` | Select Modulation type  |`RFM69_MODUL_FSK`<br> `RFM69_MODUL_OOK`  |
| `RFM69_SetDataProcessingMode` | Select Data Processing type  |`RFM69_PACKET_MODE`<br> `RFM69_CONTINUOUS_SYNC`  <br> `RFM69_CONTINUOUS_RAW` |
| `RFM69_SetDataShaping` | Select Data Shaping type  |`RFM69_SHAPING_NONE`<br> `RFM69_SHAPING_Gaussianfilter_BT1`<br> `RFM69_SHAPING_Gaussianfilter_BT05`<br> `RFM69_SHAPING_Gaussianfilter_BT03`  |

</details>

### 📤 Transmission

- `RFM69_PowerAmplifierSelection` → Select Power Amplifier type ***Pout = PA_... + OutputPower [dBm]***
- `RFM69_FlushFIFO` → Flush FIFO
- `RFM69_SendMessage` →  Send message

<details>
<summary> Full reference </summary>

| Transmission | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_PowerAmplifierSelection` | Select Power Amplifier type <br> ***Pout = PA_... + OutputPower [dBm]***  |`PA_0 => Start from -18 to +13 dbm `<br> `PA_1 => Start from -18 to +13 dbm`<br> `PA_1_2 => Start from -14 to +17 dbm `<br> `PA_HIGH_POWER => Start from -11 to +20 dbm`<br>  |
| `RFM69_FlushFIFO` | Flush FIFO  | |
| `RFM69_SendMessage` | Send message   uint8_t message[] = "SATELLITE_TEST";<br>RFM69_SendMessage( message, 14);  |

</details>

### 📥 Reception

- `RFM69_AutoMessageDetectionReceive_Config` → Choose type of syncing between two module
- `RFM69_SetLnaImpedance` → Set LNA Impedance (Z) In
- `RFM69_SetLnaGain` → Set LNA Gain
- `RFM69_GetLnaStatus` → Get LNA Configuration
- `RFM69_ReceiveMessage` → Read a message received

<details>
<summary> Full reference </summary>

| Reception | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_AutoMessageDetectionReceive_Config` | Choose type of syncing between two module  |`RFM69_SYNC_OFF`<br> `RFM69_SYNC_ON`<br>`RFM69_FIFO_FILL_IF_SYNC`<br>`RFM69_FIFO_FILL_ALWAYS`<br>`RFM69_SYNC_TOLERANCE_ERROR_0`<br>`RFM69_SYNC_TOLERANCE_ERROR_1`<br>`RFM69_SYNC_TOLERANCE_ERROR_2`<br>`RFM69_SYNC_TOLERANCE_ERROR_3`<br>`RFM69_SYNC_TOLERANCE_ERROR_4`<br>`RFM69_SYNC_TOLERANCE_ERROR_5`<br>`RFM69_SYNC_TOLERANCE_ERROR_6`<br>`RFM69_SYNC_TOLERANCE_ERROR_7`<br> |
| `RFM69_SetLnaImpedance` | Set LNA Impedance (Z) In  | `LNA_Impedance_50`<br> `LNA_Impedance_200`<br> |
| `RFM69_SetLnaGain` | Set LNA Gain  | `LNA_GAIN_AUTO`<br> `LNA_GAIN_G1`<br>`LNA_GAIN_G2`<br>`LNA_GAIN_G3`<br>`LNA_GAIN_G4`<br>`LNA_GAIN_G4`<br>`LNA_GAIN_G5`<br>`LNA_GAIN_G6`<br> |
| `RFM69_GetLnaStatus` | Get LNA Configuration  | |
| `RFM69_ReceiveMessage` | Read a message received  | uint8_t receivedLen = RFM69_ReceiveMessage(rxBuffer, 64); |
</details>

### 🔄 Automatic Frequency Correction AFC

- `RFM69_StartAFC` → Start Frequency Recalibration AFC 
- `RFM69_Status_RecalibrationFrequencyAFC` → Get Status of the Automatic Recalibration Frequency Correction AFC
- `RFM69_GetAFCCorrectionHz` → Read AFC Correction in Hz
- `RFM69_ClearAfc` → Clear Frequency Recalibration AFC
- `RFM69_SetAfcAuto` → Activate/Deactive Automatic Frequency Recalibration AFC
- `RFM69_SetAfcAutoclear` → Activate/Deactive Clearing Automatic Frequency Recalibration AFC

<details>
<summary> Full reference </summary>

| Automatic Frequency Correction AFC | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_StartAFC` | Start Frequency Recalibration AFC  ||
| `RFM69_Status_RecalibrationFrequencyAFC` | Get Status of the Automatic Recalibration Frequency Correction AFC  | |
| `RFM69_GetAFCCorrectionHz` | Read AFC Correction in Hz  | |
| `RFM69_ClearAfc` | Clear Frequency Recalibration AFC  | |
| `RFM69_SetAfcAuto` | Activate/Deactive Automatic Frequency Recalibration AFC  |`AFC_AUTO_OFF`<br>`AFC_AUTO_ON`<br> |
| `RFM69_SetAfcAutoclear` | Activate/Deactive Automatic Frequency Recalibration AFC  | `AFC_AUTOCLEAR_OFF`<br>`AFC_AUTOCLEAR_ON`<br>|

</details>


### 📊 Frequency Error Indicator FEI

- `RFM69_StartFEI` → Start Measurement Of Error Frequency in Hz FEI
- `RFM69_Status_ReadingErrorFrequency` → Get Status of the Reading Error Frequency FEI
- `RFM69_GetFrequencyErrorFeiHz` → Read Error Frequency in Hz FEI


<details>
<summary> Full reference </summary>

| Frequency Error Indicator FEI | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_StartFEI` | Start Measurement Of Error Frequency in Hz FEI  | |
| `RFM69_Status_ReadingErrorFrequency` | Get Status of the Reading Error Frequency FEI  |  |
| `RFM69_GetFrequencyErrorFeiHz` | Read Error Frequency in Hz FEI |  |

</details>

### 🔍 Diagnostics

- `RFM69_RSSI` → Visualize RSSI in dBm
- `RFM69_getConfigData` → Get all parameters set in one command
- `RFM69_SetOverloadCurrentProtection` → Enable or disable the overload current protection for the PA 


<details>
<summary> Full reference </summary>

| Diagnostics | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_RSSI` | Visualize RSSI in dBm  | Return an uint8_t ; |
| `RFM69_getConfigData` | Get all parameters set in one command  |  |
| `RFM69_SetOverloadCurrentProtection` | Enable or disable the overload current protection for the PA  |  |

</details>

---


## ⚠️ Important notes
- Key Sync have to be the same on each module
- Bitrates have to be the same on each module
  
## 🔗 Resources & Links
* [Official Website](https://www.adafruit.com/product/3071)
* [Datasheet RFM69HCW](https://cdn.sparkfun.com/datasheets/Wireless/General/RFM69HCW-V1.1.pdf)
* [Guide d'apprentissage Adafruit](https://learn.adafruit.com/adafruit-rfm69hcw-and-rfm96-rfm95-rfm98-lora-packet-padio-breakouts/downloads)

## 📄 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---


## 🧪 Connection Troubleshooting

If the module does not respond correctly, check the SPI communication.

### ✔️ Step 1 — Verify chip detection

Read the register:

- `RegVersion` = `0x10`

Expected value:
- `0x24`

👉 If the value is different, it means:
- SPI wiring issue, or
- incorrect configuration
---

## 🔧 Debug Configuration

To enable or disable all `printf` debug outputs, edit `RFM69HCW.h`.

Simply comment or uncomment the following line:

```c
/*
 * Enable/Disable global DEBUG printf
 * Comment to disable all debug outputs
 */
#define RFM69_DEBUG_ENABLED

#ifdef RFM69_DEBUG_ENABLED
    #define RFM69_printf(...) printf(__VA_ARGS__)
#else
    #define RFM69_printf(...)
#endif
```

