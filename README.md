# STM32 RFM69HCW / SX1231 Driver (HAL-Based)

This is a C driver for the **RFM69HCW / SX1231** RF transceiver, designed for **STM32** microcontrollers using the **STM32 HAL** library.

This library provides a way of controlling the radio module over SPI. **NEW FUNCTION COMING THIS WEEK**


<img src="https://cdn-shop.adafruit.com/970x728/3070-07.jpg" alt="Adafruit RFM69HCW Transceiver Radio" width="260">


## 📖 Documentation
To help you understand or modify the project, the documentation has been split into detailed sections:
*   💻 [RFM69HCW Packet Mode Details](docs/Packet_Mode.md) — Understand how to make a simple communication using Packet Mode
*   📊 [RFM69HCW RAW Mode Details](docs/RAW_Modes.md) — Understand how to use RAW Mode
*   🔒 [Payload Encryption Guide](docs/AES.md) — How to secure your data using external cryptographic libraries.
*   🪲 [Debug](docs/debug.md) — Simple Debug Implementation
*   🚨 [TroubleShooting](docs/troubleshooting.md) — Many response to a lot of error...


## ✨ Implemented Features

### 🔧 Core API 

- `RFM69_WriteReg` → Write a value in a register
- `RFM69_ReadReg` → Read a register value

<details>
<summary> Full reference </summary>

| Core API | Description | Example |
| :--- | :--- | :--- | 
|  `RFM69_WriteReg ` | Write a value in a register |  ```RFM69_WriteReg(0x37, 0x90);``` | 
|  `RFM69_ReadReg ` | Read a value in a register |  ```uint8_t currentVal = RFM69_ReadReg(0x02);``` | 

</details>

### ⚙️ Radio Initialisation

- `RFM69_Init` → Initialize SPI interface and Chip Select pin
- `RFM69_SetMode` → Select operating mode type
- `RFM69_GetMode` → Get operating mode

<details>
<summary> Full reference </summary>

| Radio Initialization| Description | Example/Available Modes |
| :--- | :--- | :--- |
| `RFM69_Init` | Initialize SPI interface and Chip Select pin | ```RFM69_Init(&hspi1, GPIOC, GPIO_PIN_7);``` |
| `RFM69_SetMode` | Select operating mode type  |`RFM69_MODE_SLEEP`<br> `RFM69_MODE_STDBY`  <br> `RFM69_MODE_FS` <br> `RFM69_MODE_TX` <br> `RFM69_MODE_RX` |
| `RFM69_GetMode` | Get operating mode  | Return `RFM69_Mode_t` :<br>  `RFM69_MODE_SLEEP`<br> `RFM69_MODE_STDBY`  <br> `RFM69_MODE_FS` <br> `RFM69_MODE_TX` <br> `RFM69_MODE_RX` |

</details>

### 📡 Radio Configuration

- `RFM69_SetFrequencyTo433` → Set Frequency to 433 MHz
- `RFM69_SetBitrate` → Set bitrate of communication
- `RFM69_GetBitrate` → Get bitrate of communication
- `RFM69_AutoSetFdev` → Automatically adjusts frequency deviation based on bitrate for FSK Modulation
- `RFM69_GetFdev` → Get frequency deviation based on bitrate with FSK Modulation
- `RFM69_SetModulationType` → Set modulation type
- `RFM69_GetModulationType` → Get modulation type
- `RFM69_SetDataProcessingMode` → Set Data Processing type
- `RFM69_GetDataProcessingMode` → Get Data Processing type
- `RFM69_SetDataShaping` → Set Data Shaping type
- `RFM69_GetDataShaping` → Get Data Shaping type
- `RFM69_WaitForPLLLock` → Wait For PLL Lock
- `RFM69_WaitForTxReady` → Wait For Tx Mode Ready

<details>
<summary> Full reference </summary>

| Radio Configuration | Description | Example/Available Modes |
| :--- | :--- | :--- |
| `RFM69_SetFrequencyTo433` | Set Frequency to 433 MHz  |  |
| `RFM69_SetBitrate` | Set bitrate of communication : <br> 4.8 kbps = 4800 | ```RFM69_SetBitrate(4800);```  |
| `RFM69_GetBitrate` | Get bitrate of communication | ```uint32_t Bitrate = RFM69_GetBitrate();```  |
| `RFM69_AutoSetFdev` | Automatically adjusts frequency deviation based on bitrate for FSK Modulation |```RFM69_AutoSetFdev();``` |
| `RFM69_GetFdev` | Get frequency deviation based on bitrate  |```uint32_t Fdev = RFM69_GetFdev();``` |
| `RFM69_SetModulationType` | Set Modulation type  |`RFM69_MODUL_FSK`<br> `RFM69_MODUL_OOK`  |
| `RFM69_GetModulationType` | Get Modulation type  |`RFM69_Modulation_t RFM69_GetModulationType(void)`  |
| `RFM69_SetDataProcessingMode` | Select Data Processing type  |`RFM69_PACKET_MODE`<br> `RFM69_CONTINUOUS_SYNC`  <br> `RFM69_CONTINUOUS_RAW` |
| `RFM69_GetDataProcessingMode` | Get Data Processing type  | RFM69_DataProcessingMode_t RFM69_GetDataProcessingMode(void) |
| `RFM69_SetDataShaping` | Select Data Shaping type  |`RFM69_SHAPING_NONE`<br> `RFM69_SHAPING_Gaussianfilter_BT1`<br> `RFM69_SHAPING_Gaussianfilter_BT05`<br> `RFM69_SHAPING_Gaussianfilter_BT03`  |
| `RFM69_GetDataShaping` | Get Data Shaping type  | RFM69_Data_Shaping_t RFM69_GetDataShaping(void) |
| `RFM69_WaitForPLLLock` | Wait For PLL Lock  |  |
| `RFM69_WaitForTxReady` | Wait For Tx Mode Ready  |  |

</details>

### 📶 Packet Engine

- `RFM69_SetKeyValues_Sync` → Set Key to link two modules.
- `RFM69_SetPacketConfig` → Select Packet Configuration
- `RFM69_AutoMessageDetectionReceive_Config` → Select Automatic Receive Packet Configuration
- `RFM69_FlushFIFO` → Flush FIFO
- `RFM69_SendMessage_Packet_Mode` →  Send message
- `RFM69_ReceiveMessage_Packet_Mode` → Read a message received

<details>
<summary> Full reference </summary>

| Modulation & Packet Engine | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_SetKeyValues_Sync` | Set Key to link two modules. <br> **Must be identical on both modules**  <br> ***Max key length 8***  |  ```uint8_t mykey[] = {0x42, 0x24};``` <br> ```RFM69_SetSyncValues(mykey, 2);``` |
| `RFM69_SetPacketConfig` | Select Packet Configuration  |`RFM69_PACKET_FORMAT_FIXED`<br> `RFM69_PACKET__FORMAT_VARIABLE`<br> `RFM69_DC_FREE_NONE`<br> `RFM69_DC_FREE_MANCHESTER`<br> `RFM69_DC_FREE_WHITENING`<br> `RFM69_CRC_OFF`<br> `RFM69_CRC_ON`<br> `RFM69_CRC_AUTOCLEAR_ON`<br> `RFM69_CRC_AUTOCLEAR_OFF`<br> `RFM69_FILTER_NONE`<br> `RFM69_FILTER_NODE`<br> `RFM69_FILTER_NODE_BROADCAST`  |
| `RFM69_AutoMessageDetectionReceive_Config` | Choose type of syncing between two module  |`RFM69_SYNC_OFF`<br> `RFM69_SYNC_ON`<br>`RFM69_FIFO_FILL_IF_SYNC`<br>`RFM69_FIFO_FILL_ALWAYS`<br>`RFM69_SYNC_TOLERANCE_ERROR_0`<br>`RFM69_SYNC_TOLERANCE_ERROR_1`<br>`RFM69_SYNC_TOLERANCE_ERROR_2`<br>`RFM69_SYNC_TOLERANCE_ERROR_3`<br>`RFM69_SYNC_TOLERANCE_ERROR_4`<br>`RFM69_SYNC_TOLERANCE_ERROR_5`<br>`RFM69_SYNC_TOLERANCE_ERROR_6`<br>`RFM69_SYNC_TOLERANCE_ERROR_7`<br> |
| `RFM69_FlushFIFO` | Flush FIFO  | |
| `RFM69_SendMessage_Packet_Mode` | Send message   uint8_t message[] = "SATELLITE_TEST";<br>RFM69_SendMessage( message, 14);  |
| `RFM69_ReceiveMessage_Packet_Mode` | Read a message received  | uint8_t receivedLen = RFM69_ReceiveMessage_Packet_Mode(rxBuffer, 64); |

</details>

### 📶 RAW Mode

- `RFM69_SetDefaultDioMapping` → Set DIO Mapping Default (Important if you use RAW Mode)
- `RFM69_Text_To_Binary_Payload` → Transform Text to Binary format 
- `RFM69_RAW_FillUp_Playload` → Fill Up Playload before sending
- `RFM69_RAW_DATA_SEND` → Automatic Send Message in RAW Mode
- `RFM69_RAW_Transmit_Byte` → Transmit byte over radio with bit stuffing
- `RFM69_RAW_CRC16_Calculation_Buffer` → Calculate CRC16 for error check
- `RFM69_Pad_To_256` → Complete Data buffer to be up to 256 bytes  
- `CRC_CHECK` → CRC Check


<details>
<summary> Full reference </summary>

| Function| Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_SetDefaultDioMapping` | Set DIO Mapping Default (Important if you use RAW Mode) | By default DIO1 is DCLK (Clock) |
| `RFM69_Text_To_Binary_Payload` | Transform Text to Binary format   |  |
| `RFM69_RAW_FillUp_Payload` | Fill Up Playload before sending  | |
| `RFM69_RAW_DATA_SEND` | Automatic Send Message in RAW Mode  | |
| `RFM69_RAW_Transmit_Byte` | Transmit byte over radio with bit stuffing | void RFM69_RAW_Transmit_Byte(uint8_t data, uint8_t enable_stuffing) |
| `RFM69_RAW_CRC16_Calculation_Buffer` | Calculate CRC16 for error check | |
| `RFM69_Pad_To_256` | Complete Data buffer to be up to 256 bytes | |
| `CRC_CHECK` | CRC Check | |

</details>

### 📤 Transmission Parameters

- `RFM69_PowerAmplifierSelection` → Select Power Amplifier type ***Pout = PA_... + OutputPower [dBm]***
- `RFM69_GetPowerAmplifier` → Get Power Amplifier type

<details>
<summary> Full reference </summary>

| Transmission | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_PowerAmplifierSelection` | Select Power Amplifier type <br> ***Pout = PA_... + OutputPower [dBm]***  |`PA_0 => Start from -18 to +13 dbm `<br> `PA_1 => Start from -18 to +13 dbm`<br> `PA_1_2 => Start from -14 to +17 dbm `<br> `PA_HIGH_POWER => Start from -11 to +20 dbm`<br>  |
| `RFM69_GetPowerAmplifier` | Get Power Amplifier type  | RFM69_PA_Select_t RFM69_GetPowerAmplifier(void) |


</details>

### 📥 Reception Parameters

- `RFM69_RSSI` → Visualize RSSI in dBm
- `RFM69_SetLnaImpedance` → Set LNA Impedance (Z) In
- `RFM69_SetLnaGain` → Set LNA Gain
- `RFM69_GetLnaStatus` → Get LNA Configuration
- `RFM69_StartAFC` → Start Frequency Recalibration AFC 
- `RFM69_Status_RecalibrationFrequencyAFC` → Get Status of the Automatic Recalibration Frequency Correction AFC
- `RFM69_GetAFCCorrectionHz` → Read AFC Correction in Hz
- `RFM69_ClearAfc` → Clear Frequency Recalibration AFC
- `RFM69_SetAfcAuto` → Activate/Deactive Automatic Frequency Recalibration AFC
- `RFM69_SetAfcAutoclear` → Activate/Deactive Clearing Automatic Frequency Recalibration AFC
- `RFM69_StartFEI` → Start Measurement Of Error Frequency in Hz FEI
- `RFM69_Status_ReadingErrorFrequency` → Get Status of the Reading Error Frequency FEI
- `RFM69_GetFrequencyErrorFeiHz` → Read Error Frequency in Hz FEI
  
<details>
<summary> Full reference </summary>

| Reception | Description | Example/Available Modes |
| :--- | :---: | :--- |
| `RFM69_RSSI` | Visualize RSSI in dBm  | Return an uint8_t ; |
| `RFM69_SetLnaImpedance` | Set LNA Impedance (Z) In  | `LNA_Impedance_50`<br> `LNA_Impedance_200`<br> |
| `RFM69_SetLnaGain` | Set LNA Gain  | `LNA_GAIN_AUTO`<br> `LNA_GAIN_G1`<br>`LNA_GAIN_G2`<br>`LNA_GAIN_G3`<br>`LNA_GAIN_G4`<br>`LNA_GAIN_G4`<br>`LNA_GAIN_G5`<br>`LNA_GAIN_G6`<br> |
| `RFM69_GetLnaStatus` | Get LNA Configuration  | |
| `RFM69_StartAFC` | Start Frequency Recalibration AFC  ||
| `RFM69_Status_RecalibrationFrequencyAFC` | Get Status of the Automatic Recalibration Frequency Correction AFC  | |
| `RFM69_GetAFCCorrectionHz` | Read AFC Correction in Hz  | |
| `RFM69_ClearAfc` | Clear Frequency Recalibration AFC  | |
| `RFM69_SetAfcAuto` | Activate/Deactive Automatic Frequency Recalibration AFC  |`AFC_AUTO_OFF`<br>`AFC_AUTO_ON`<br> |
| `RFM69_SetAfcAutoclear` | Activate/Deactive Automatic Frequency Recalibration AFC  | `AFC_AUTOCLEAR_OFF`<br>`AFC_AUTOCLEAR_ON`<br>|
| `RFM69_StartFEI` | Start Measurement Of Error Frequency in Hz FEI  | |
| `RFM69_Status_ReadingErrorFrequency` | Get Status of the Reading Error Frequency FEI  |  |
| `RFM69_GetFrequencyErrorFeiHz` | Read Error Frequency in Hz FEI |  |

</details>

---
## 💡 SPI Tested configuration

The SPI was configured as follows:

- **Frame Format :** Motorola
- **Mode :** Full-Duplex Master  
- **CPOL :** Low   
- **CPHA :** 2 Edge  
- **First Bit :** MSB First
  
## 🔗 Resources & Links
* [Official Website](https://www.adafruit.com/product/3071)
* [Datasheet RFM69HCW](https://cdn.sparkfun.com/datasheets/Wireless/General/RFM69HCW-V1.1.pdf)
* [Adafruit Learning Guide](https://learn.adafruit.com/adafruit-rfm69hcw-and-rfm96-rfm95-rfm98-lora-packet-padio-breakouts/downloads)

## 📄 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

### Note

To use the `printf` debug feature on STM32, make sure you have redirected `stdout` to your UART peripheral (by overriding the `_write` function).


