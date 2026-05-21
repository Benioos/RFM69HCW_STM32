## 📦 Packet Mode Explanation

The RFM69HCW features a powerful, built-in **Packet Engine**. When using the **Packet Mode**, the chip takes care of the heavy lifting regarding packet formatting, synchronization, and error checking, heavily reducing the microcontroller's CPU overhead.

### How it Works
In Packet Mode, the RFM69HCW operates as an intelligent radio transceiver:
* **In Transmission (TX):** The microcontroller simply writes the raw payload data (your message) into the chip's internal 66-byte FIFO memory. Once triggered, the RFM69 hardware automatically appends the preamble, the programmable Sync Word, the optional packet length byte, and computes the hardware CRC before transmitting everything.
* **In Reception (RX):** The chip constantly listens to the air. When it detects a valid preamble and matches the Sync Word, it automatically locks onto the signal, receives the payload, verifies the CRC, and fills its FIFO. It then triggers an interrupt (e.g., `PayloadReady`) to signal the microcontroller that a clean, error-free packet is ready to be read.

### Configuration

**In both Transmission (TX) and Reception (RX):**
Use `RFM69_SetDataProcessingMode` with the parameter `RFM69_PACKET_MODE` to enable the hardware packet engine.

## Example
```c
int main(void)
{
  RFM69_Init(&hspi1, GPIOC, GPIO_PIN_7);

  // Get Module Out Of Sleep Mode To Configure
  RFM69_SetMode(RFM69_MODE_STDBY);

  // Module Configuration 
  RFM69_SetFrequencyTo433();
  RFM69_SetBitrate(4800);

  // More Complexe Configuration (For beginner, you can copy and paste)
  RFM69_SetDataProcessingMode(RFM69_PACKET_MODE);
  RFM69_SetModulationType(RFM69_MODUL_FSK);
  RFM69_SetDataShaping(RFM69_SHAPING_Gaussianfilter_BT1);
  RFM69_AutoSetFdev();

  // If you have an antenna :
  RFM69_PowerAmplifierSelection(PA_1, 13);
  
  //Sync Module For A Good Reception
  RFM69_AutoMessageDetectionReceive_Config(RFM69_SYNC_ON, RFM69_FIFO_FILL_IF_SYNC, 2, RFM69_SYNC_TOLERANCE_ERROR_2);
  RFM69_SetPacketConfig(RFM69_PACKET_VARIABLE, RFM69_DC_FREE_NONE, RFM69_CRC_ON, RFM69_CRC_AUTOCLEAR_ON, RFM69_FILTER_NONE);
  uint8_t mykey[] = {0x42, 0x24};
  RFM69_SetKeyValues_Sync(mykey, 2);

  // The Message You Want To Send
  uint8_t message[] = "SATELLITE_TEST";
}

while (1)
{
  // Sending Message In Packet Mode Automatically 
  RFM69_SendMessage( message, 14);
  HAL_Delay(5000);
}
```

## ⚠️ Important notes
- Key Sync have to be the same on each module
- Bitrates have to be the same on each module
