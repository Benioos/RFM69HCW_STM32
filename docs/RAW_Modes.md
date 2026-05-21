## 📟 RAW Mode Explanation

By default, the RFM69HCW is designed to handle packet processing automatically (adding its own preambles, sync words, and CRC). 

However, the standard AX.25 frame used in packet radio has its own strict structure, custom CRC (FCS), and bit-stuffing mechanism that do not match the RFM69 chip's native packet engine.

To bypass the chip's internal packet processing and gain full control over every single bit sent over the air, we use the **RAW Mode** (also known as Continuous Mode).

![Schéma de la trame AX25](https://raw.githubusercontent.com/Benioos/RFM69HCW_STM32/main/Image/Trame.png)

### How it Works
In RAW mode, the RFM69HCW acts as a "dumb" radio pipe:
* **In Transmission (TX):** The microcontroller manually generates the entire AX.25 frame including the `0x7E` flags, shifted addresses, control bytes, payload, and the calculated FCS. It then feeds this bitstream directly to the RFM69's `DATA` pin in real-time, matching the desired bitrate.
* **In Reception (RX):** The chip simply demodulates the incoming radio signal and dumps the raw bitstream onto the `DATA` pin. The microcontroller must constantly poll this pin to look for the `0x7E` preamble flag, perform bit-unstuffing, and validate the FCS.

### Configuration

**In Transmission (TX):**
Use `RFM69_SetDataProcessingMode` with the parameter `RFM69_CONTINUOUS_RAW` to use the module in pure RAW Mode.

**In Reception (RX):**
Use `RFM69_SetDataProcessingMode` with the parameter `RFM69_CONTINUOUS_SYNC`.

> 💡 **Synchronization Trick:** We use `RFM69_CONTINUOUS_SYNC` mode in RX instead of pure RAW mode. This allows the RFM69 hardware to automatically sync, simplifying the bit-synchronization between both modules and saving precious MCU processing time.

### Implemented Features

* **Flexible Configuration:** Easily change the module's destination/source address.
* **Error Detection:** Complete CRC-16 (AX.25) error detection is fully implemented to ensure packet integrity.
* **Security Validation:** You can easily customize the validation logic to filter out unauthorized or spoofed "pirate" messages.
