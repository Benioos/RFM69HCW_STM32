# 📬 Packet Mode

The `RFM69HCW` features a hardware **Packet Mode** that automatically handles:
- packet framing,
- synchronization,
- CRC verification,
- FIFO management.

Compared to raw continuous transmission, Packet Mode improves reliability while simplifying software implementation.


### ✔️ Activate 

Both the **transmitter (TX)** and **receiver (RX)** must use Packet Mode.

Enable it with:
```c
RFM69_SetDataProcessingMode(RFM69_PACKET_MODE);
```
### ✔️ Take a look 

> [!TIP]
> * Sync Key, Bitrate, Frequency have to be the same on each module. <br>
> * The internal FIFO is limited to **66 bytes**.

### ✔️ Example 

> Example for **transmitter (TX)** :

```c

int main(void)
{
  RFM69_Init(&hspi1, GPIOC, GPIO_PIN_7); 

  RFM69_SetMode(RFM69_MODE_STDBY); //Exit Sleep Mode to Configure

  /*
   * Basic radio configuration
  /* 
  RFM69_SetFrequencyTo433();
  RFM69_SetBitrate(4800);

  /*
   * Packet mode configuration
  /* 
  RFM69_SetDataProcessingMode(RFM69_PACKET_MODE);
  RFM69_SetModulationType(RFM69_MODUL_FSK);
  RFM69_SetDataShaping(RFM69_SHAPING_Gaussianfilter_BT1);

  /*
   * Automatically configure frequency deviation
  /* 
  RFM69_AutoSetFdev();

  RFM69_PowerAmplifierSelection(PA_1, 13); // Enable PA if antenna is connected

  // Configure synchronization system
  RFM69_AutoMessageDetectionReceive_Config(RFM69_SYNC_ON, RFM69_FIFO_FILL_IF_SYNC, 2,  RFM69_SYNC_TOLERANCE_ERROR_2);

  // Configure packet engine
  RFM69_SetPacketConfig(RFM69_PACKET_VARIABLE, RFM69_DC_FREE_NONE, RFM69_CRC_ON, RFM69_CRC_AUTOCLEAR_ON, RFM69_FILTER_NONE );

  /*
   * Configure sync key
  /* 
  uint8_t mykey[] = {0x42, 0x24};
  RFM69_SetKeyValues_Sync(mykey, 2);

  // Payload
  uint8_t message[] = "SATELLITE_TEST";

  while (1)
  {
      // Send packet
      RFM69_SendMessage(message, size of message[]);

      HAL_Delay(5000);
  }
}
```
