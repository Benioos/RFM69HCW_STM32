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

### Example :
```c
  RFM69_printfs("====================================================\r\n");
  RFM69_printfs("================== Programm Begin ==================\r\n");
  RFM69_printfs("====================================================\r\n");

  RFM69_Init(&hspi1, GPIOC, GPIO_PIN_7);
  RFM69_SetMode(RFM69_MODE_STDBY);
  RFM69_SetFrequencyTo433();
  RFM69_SetDataProcessingMode(RFM69_CONTINUOUS_SYNC);
  RFM69_SetModulationType(RFM69_MODUL_FSK);
  RFM69_SetDataShaping(RFM69_SHAPING_Gaussianfilter_BT1);
  RFM69_getConfigData();
  RFM69_SetBitrate(4800);

  RFM69_AutoSetFdev();
  RFM69_PowerAmplifierSelection(PA_1, 13);

  while (1)
  {
	  if (rx_paquet_pret_a_afficher)
	  {
		RFM69_printf(C, "\r\n[RADIO RX] START Operation Message Received", " \r\n");

		uint16_t crc_rx = RFM69_RAW_Show_Trame(rx_longueur_paquet, rx_buffer_paquet);
		uint8_t CRC_CHECK = RFM69_CRC_Check(crc_rx, rx_buffer_paquet);
		uint8_t DataType = RFM69_RAW_DATATYPE(rx_buffer_paquet);
		RFM69_RAW_SHOW_DATA(DataType,rx_longueur_paquet,rx_buffer_paquet);

		rx_paquet_pret_a_afficher = 0;
		RFM69_printf(C, "[RADIO RX]", "END Operation Message Received \r\n");
	  }
  }
```

```c

volatile uint8_t LastBit_Received = 2;
volatile uint8_t Bit_Identical_Counter = 0;
volatile uint8_t Register_Swipe = 0x00;


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == RFM_DCLK_Pin)
    {
        uint8_t Bit_Received = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET) ? 1 : 0;

    	/*
    	 * NOISE FILTER
    	 */
        if (State_Machine_RX == STATE_RECHERCHE_FLAG)
        {

        	if (Bit_Received == LastBit_Received) {
        	    Bit_Identical_Counter++;
        	    if (Bit_Identical_Counter >= 16) {
        	        return; // Ligne figée ou bruit continu, on ignore.
        	    }
        	} else {
        	    Bit_Identical_Counter = 0;
        	    LastBit_Received = Bit_Received;
        	}

        }

    	/*
    	 * FLAG 0x7E Research
    	 */
        if (State_Machine_RX == STATE_RECHERCHE_FLAG)
        {

            Register_Swipe = (Register_Swipe << 1) | Bit_Received;

            if (Register_Swipe == 0x7E)
            {
                State_Machine_RX = STATE_VERIF_ADRESSE;

                Adress_Received = 0;
                Bit_Counter = 7;
                Temporary_Byte = 0x00;
            }
            return;
        }

    	/*
    	 * Byte Reconstruction from bit.
    	 */
        if (Bit_Received) {
            Temporary_Byte |= (1 << Bit_Counter);
        } else {
            Temporary_Byte &= ~(1 << Bit_Counter);
        }

        Bit_Counter--;

        if (Bit_Counter >= 0) {
            return;
        }

        uint8_t octet_valide = Temporary_Byte;

        Bit_Counter = 7;
        Temporary_Byte = 0x00;

    	/*
    	 * Check Adress
    	 */
        if (State_Machine_RX == STATE_VERIF_ADRESSE)
        {
            if (octet_valide == adresse_cible[Adress_Received]) {
                Adress_Received++;
                if (Adress_Received >= 14) {
                    State_Machine_RX = STATE_CAPTURE_DONNEES;
                    rx_idx_octet_paquet = 0;
                }
            } else {
                State_Machine_RX = STATE_RECHERCHE_FLAG;
            }
        }

    	/*
    	 * Stock Data
    	 */
        else if (State_Machine_RX == STATE_CAPTURE_DONNEES)
        {
            if (octet_valide == 0x7E && rx_idx_octet_paquet > 1)
            {
                rx_longueur_paquet = rx_idx_octet_paquet;
                rx_paquet_pret_a_afficher = 1;
                State_Machine_RX = STATE_RECHERCHE_FLAG;
            }
            else
            {
                if (rx_idx_octet_paquet < TAILLE_CAPTE_RAW) {
                    rx_buffer_paquet[rx_idx_octet_paquet++] = octet_valide;
                }

                if (rx_idx_octet_paquet >= TAILLE_CAPTE_RAW)
                {
                    rx_longueur_paquet = rx_idx_octet_paquet;
                    rx_paquet_pret_a_afficher = 1;
                    State_Machine_RX = STATE_RECHERCHE_FLAG;
                }
            }
        }
    }
}
```

