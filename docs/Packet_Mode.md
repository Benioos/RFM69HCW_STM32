## 📦 Packet Mode Explanation

The RFM69HCW features a powerful, built-in **Packet Engine**. When using the **Packet Mode**, the chip takes care of the heavy lifting regarding packet formatting, synchronization, and error checking, heavily reducing the microcontroller's CPU overhead.

### How it Works
In Packet Mode, the RFM69HCW operates as an intelligent radio transceiver:
* **In Transmission (TX):** The microcontroller simply writes the raw payload data (your message) into the chip's internal 66-byte FIFO memory. Once triggered, the RFM69 hardware automatically appends the preamble, the programmable Sync Word, the optional packet length byte, and computes the hardware CRC before transmitting everything.
* **In Reception (RX):** The chip constantly listens to the air. When it detects a valid preamble and matches the Sync Word, it automatically locks onto the signal, receives the payload, verifies the CRC, and fills its FIFO. It then triggers an interrupt (e.g., `PayloadReady`) to signal the microcontroller that a clean, error-free packet is ready to be read.

### Configuration

**In both Transmission (TX) and Reception (RX):**
Use `RFM69_SetDataProcessingMode` with the parameter `RFM69_PACKET_MODE` to enable the hardware packet engine.

## ⚠️ Important notes
- Key Sync have to be the same on each module
- Bitrates have to be the same on each module

