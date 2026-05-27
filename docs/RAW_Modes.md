# 📻 RAW Mode

The `RFM69HCW` normally handles:
- preamble generation,
- synchronization,
- CRC generation,
- packet framing.

However, protocols like **AX.25** require:
- custom frame formatting,
- custom CRC,
- bit stuffing,

To fully control the transmitted radio stream, the module must use **RAW Mode** (Continuous Mode). <br><br>
***Find below an ilustration of the payload :***

![AX25 Frame](https://raw.githubusercontent.com/Benioos/RFM69HCW_STM32/main/Image/Trame.png)

### ✔️ Activate

#### Transmission (TX)

```c
RFM69_SetDataProcessingMode(RFM69_CONTINUOUS_RAW);
```

#### Reception (RX)

```c
RFM69_SetDataProcessingMode(RFM69_CONTINUOUS_SYNC);
```

> [!TIP]
> `RFM69_CONTINUOUS_SYNC` is used on RX to let the hardware automatically recover the clock signal.  
> This greatly simplifies bit synchronization and reduces MCU processing load.

# ✔️ Example

> Example for **transmitter (TX)** :

```c

  RFM69_Init(&hspi1, GPIOC, GPIO_PIN_7);
  RFM69_SetMode(RFM69_MODE_STDBY);
  RFM69_SetFrequencyTo433();
  RFM69_SetDataProcessingMode(RFM69_CONTINUOUS_RAW);
  RFM69_SetModulationType(RFM69_MODUL_FSK);
  RFM69_SetDataShaping(RFM69_SHAPING_Gaussianfilter_BT1);
  RFM69_getConfigData();
  RFM69_SetBitrate(4800);

  RFM69_AutoSetFdev();
  RFM69_PowerAmplifierSelection(PA_1, 13);
  RFM69_SetDefaultDioMapping();

  RFM69_WriteReg(0x18, 0x08); // LNA Gain Max (pour capter de loin)

  RFM69_WriteReg(0x19, 0x42); // Filtre adapté au bitrate 4800
  RFM69_WriteReg(0x3C, 0x01); // Seuil FIFO bas (réception immédiate)

  RFM69_WriteReg(0x13, 0x1A); // Protection courant 95mA
  RFM69_WriteReg(0x12, 0x05); // Rampe de montée douce (PaRamp)

  RFM69_WriteReg(0x6F, 0x30);
  RFM69_WriteReg(0x3C, 0x8F);//When send message

  const char* mon_message = "Hello World !";

  uint8_t adresse[14] =
  {
      0x14, 0x14, 0x14, 0x14,
	  0x14, 0x14, 0x14, 0x14,
	  0x14, 0x14, 0x14, 0x14,
	  0x14, 0x14
  };

  uint8_t buffer[254];
  while (1)
  {
    ptr_trame = (const uint8_t *)&trame;
	uint16_t len = RFM69_Text_To_Binary_Payload(mon_message, buffer);
	RFM69_Pad_To_256(buffer, len);
	uint16_t mon_CRC = RFM69_RAW_CRC16_Calculation_Buffer(buffer, 254);

	uint8_t control = 0x03;
	uint8_t Type_Data = 0x02;

    RFM69_RAW_FillUp_Payload(&trame, adresse, control, Type_Data, buffer, len, mon_CRC);
    // ENCRYPTION GOES HERE
    RFM69_RAW_DATA_SEND(ptr_trame, sizeof(TrameAX));
  }
```

> Example for **receiver (RX)** :

```c
  RFM69_Init(&hspi1, GPIOC, GPIO_PIN_7);
  RFM69_SetMode(RFM69_MODE_STDBY);
  RFM69_SetFrequencyTo433();
  RFM69_SetDataProcessingMode(RFM69_CONTINUOUS_SYNC);
  RFM69_SetModulationType(RFM69_MODUL_FSK);
  RFM69_SetDataShaping(RFM69_SHAPING_Gaussianfilter_BT1);
  RFM69_SetBitrate(4800);

  RFM69_AutoSetFdev();
  RFM69_PowerAmplifierSelection(PA_1, 13);

  RFM69_WriteReg(0x18, 0x08); // LNA Gain Max (pour capter de loin)

  RFM69_WriteReg(0x19, 0x42); // Filtre adapté au bitrate 4800
  RFM69_WriteReg(0x3C, 0x01); // Seuil FIFO bas (réception immédiate)

  RFM69_WriteReg(0x13, 0x1A); // Protection courant 95mA
  RFM69_WriteReg(0x12, 0x05); // Rampe de montée douce (PaRamp)

  RFM69_WriteReg(0x6F, 0x30);
  RFM69_WriteReg(0x3C, 0x8F);//When send message

  RFM69_SetMode(RFM69_MODE_RX);

  while (1)
  {

	  if (rx_paquet_pret_a_afficher)
	  {
		  rx_paquet_pret_a_afficher = 0;
		  GET_DASHBOARD((const TrameAX *)&rx_Trame);
          // DECRYPTION GOES HERE
		  CRC_CHECK((const TrameAX *)&rx_Trame);

	  }
    GET_DASHBOARD((const TrameAX *)&rx_Trame);
  }

```
