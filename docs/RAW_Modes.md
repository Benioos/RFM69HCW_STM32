# 📟 RFM69HCW RAW Mode Documentation & Implementation

By default, the RFM69HCW handles packet processing automatically (adding its own preambles, sync words, and CRC). 

However, the standard AX.25 frame used in packet radio has its own strict structure, custom CRC (FCS), and bit-stuffing mechanism that do not match the RFM69 chip's native packet engine. To bypass the chip's internal packet processing and gain full control over every single bit sent over the air, we use the **RAW Mode** (also known as Continuous Mode).

![Schéma de la trame AX25](https://raw.githubusercontent.com/Benioos/RFM69HCW_STM32/main/Image/Trame.png)

### How it Works
In RAW mode, the RFM69HCW acts as a "dumb" radio pipe:
* **In Transmission (TX):** The microcontroller manually generates the entire AX.25 frame including the `0x7E` flags, shifted addresses, control bytes, payload, and the calculated FCS. It then feeds this bitstream directly to the RFM69's `DATA` pin in real-time, matching the desired bitrate.
* **In Reception (RX):** The chip simply demodulates the incoming radio signal and dumps the raw bitstream onto the `DATA` pin. The microcontroller must constantly poll this pin or use interrupts to look for the `0x7E` preamble flag, perform bit-unstuffing, and validate the FCS.

---

## ⚙️ Configuration & Hardware Sync

* **Transmission (TX):** Use `RFM69_SetDataProcessingMode(RFM69_CONTINUOUS_RAW)` to use the module in pure RAW Mode.
* **Reception (RX):** Use `RFM69_SetDataProcessingMode(RFM69_CONTINUOUS_SYNC)`.

> 💡 **Synchronization Trick:** We use `RFM69_CONTINUOUS_SYNC` mode in RX instead of pure RAW mode. This allows the RFM69 hardware to automatically handle the clock recovery, simplifying the bit-synchronization between both modules and saving precious MCU processing time.

---

## 🛠️ Implemented Features

* **Flexible Configuration:** Easily change the module's destination/source address.
* **Error Detection:** Complete CRC-16 (AX.25) error detection is fully implemented to ensure packet integrity.
* **Security Validation:** Integrated payload encryption support (AES-128) to protect control tokens and avoid unauthorized or spoofed "pirate" messages.

---

## 💻 Code Examples

### 1. Initialization and Core Setup
This common initialization block configures the RFM69 transceiver parameters (Frequency, Modulation, Bitrate, and Power) before entering the processing loop.

```c
void Radio_Init_Sequence(void)
{
    RFM69_printfs("====================================================\r\n");
    RFM69_printfs("================== Program Begin ===================\r\n");
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
}
```
### 2. Transmission (TX) Application Code
This snippet demonstrates a clean, non-blocking transmission loop. It handles background processing, updates a user interface dashboard, encodes, pads, encrypts, and transmits data every 5 seconds.
```c
void Radio_TX_Process(void)
{
    static uint32_t last_tx_time = 0;
    const uint32_t tx_interval = 5000;
    uint32_t current_time = HAL_GetTick();

    // 1. Non-blocking UI / Status Blink Process (Every 500ms)
    static uint32_t last_blink_time = 0;
    if (current_time - last_blink_time >= 500)
    {
        last_blink_time = current_time;
        RFM69_REFRESH_SCREEN();
        RFM69_printfs(C "=================== TRANSMISSION RADIO ===================\r\n" X);

        if (tx_toggle == 0) {
            RFM69_printf(J, "[STATUS    ]", " Émission imminente... ⚡⚡⚡ \r\n");
            tx_toggle = 1;
        } else {
            uint32_t temps_restant = (tx_interval - (current_time - last_tx_time)) / 1000 + 1;
            if (temps_restant > 5) temps_restant = 5;
            RFM69_printf(C, "[STATUS    ]", " En pause radio (Envoi dans %ds)...   \r\n", temps_restant);
            tx_toggle = 0;
        }
        RFM69_printfs("----------------------------------------------------------\r\n" X);
    }

    // 2. Synchronous Transmission Trigger (Every 5 seconds)
    if (current_time - last_tx_time >= tx_interval)
    {
        last_tx_time = current_time;
        const uint8_t *ptr_trame = (const uint8_t *)&trame;

        RFM69_printf(G, "[TEXT DATA ]", " \"%s\"\r\n", mon_message);

        // Payload building and raw data padding
        uint16_t len = RFM69_Text_To_Binary_Payload(mon_message, buffer);
        RFM69_Pad_To_256(buffer, len);
        
        // Calculate CRC on plain text data layout
        uint16_t mon_CRC = RFM69_RAW_CRC16_Calculation_Buffer(buffer, 254);
        RFM69_printf(W, "[CRC CLAIR ]", " 0x%04X\r\n", mon_CRC);

        // Fill structure and secure via AES-128 encryption
        uint8_t control = 0x03;
        uint8_t Type_Data = 0x02;
        RFM69_RAW_FillUp_Payload(&trame, adresse, control, Type_Data, buffer, len, mon_CRC);

        AES128_Encrypt_Buffer256((uint8_t *)&trame.payload, RoundKey);
        RFM69_printf(M, "[CRYPTO    ]", " Payload sécurisée par AES-128\r\n");

        // Physical transmission
        RFM69_RAW_DATA_SEND(ptr_trame, sizeof(TrameAX));
        
        RFM69_printf(G, "[RADIO TX  ]", " Envoi terminé avec succès !\r\n");
        RFM69_printfs("----------------------------------------------------------\r\n");
    }
}
```
### 3. Reception (RX) Application Loop
The receiving interface polls the data handlers, verifies frame validity through structural CRC checks, decrypts the ciphered payload, and dumps the clear text payload to the user console.

```c
void Radio_RX_Process(void)
{
    // Check background hardware buffers
    GET_DASHBOARD((const TrameAX *)&rx_Trame);
    
    if (rx_paquet_pret_a_afficher)
    {
        rx_paquet_pret_a_afficher = 0; // Clear flag instantly
        
        // Step A: Check external clear text CRC on the encrypted layout
        CRC_CHECK((const TrameAX *)&rx_Trame);
        
        // Step B: Decrypt 256 bytes block [Control + Type_Data + Data]
        AES128_Decrypt_Buffer256((uint8_t *)&rx_Trame.payload, RoundKey);
        
        // Step C: Render clean data
        RFM69_printfs(G "\r\n=================== PAQUET RECU SÉCURISÉ ===================\r\n" X);
        RFM69_printf(G, "[DECRYPTED ]", " Payload déchiffrée avec succès !\r\n");
        RFM69_printf(W, "[CONTROL   ]", " 0x%02X\r\n", rx_Trame.payload.control);
        RFM69_printf(W, "[TYPE DATA ]", " 0x%02X\r\n", rx_Trame.payload.Type_Data);
        RFM69_printf(C, "[MESSAGE   ]", " \"%s\"\r\n", rx_Trame.payload.data);
        RFM69_printfs(G "============================================================\r\n\r\n" X);
    }
    HAL_Delay(1); // Small safety yield to prevent CPU choking
}
```
### 4. Low-Level Bit-Slicing: RX EXTI Callback
This hardware interrupt handles bit-by-bit extraction from the radio data line using a 3-state state machine: STATE_RECHERCHE_FLAG, STATE_VERIF_ADRESSE, and STATE_CAPTURE_DONNEES.
```c
volatile uint8_t LastBit_Received = 2;
volatile uint8_t Bit_Identical_Counter = 0;
volatile uint8_t Register_Swipe = 0x00;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == RFM_DCLK_Pin)
    {
        uint8_t Bit_Received = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET) ? 1 : 0;

        /* --- 1. NOISE FILTER --- */
        if (State_Machine_RX == STATE_RECHERCHE_FLAG)
        {
            if (Bit_Received == LastBit_Received) {
                Bit_Identical_Counter++;
                if (Bit_Identical_Counter >= 16) {
                    return; // Continuous line or noise, ignore bit.
                }
            } else {
                Bit_Identical_Counter = 0;
                LastBit_Received = Bit_Received;
            }
        }

        /* --- 2. FLAG 0x7E SEARCH --- */
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

        /* --- 3. SERIAL TO PARALLEL BYTE RECONSTRUCTION --- */
        if (Bit_Received) {
            Temporary_Byte |= (1 << Bit_Counter);
        } else {
            Temporary_Byte &= ~(1 << Bit_Counter);
        }

        Bit_Counter--;
        if (Bit_Counter >= 0) {
            return; // Wait until full byte is parsed
        }

        uint8_t octet_valide = Temporary_Byte;
        Bit_Counter = 7;
        Temporary_Byte = 0x00;

        /* --- 4. DESTINATION ADDRESS FILTER --- */
        if (State_Machine_RX == STATE_VERIF_ADRESSE)
        {
            if (octet_valide == adresse_cible[Adress_Received]) {
                Adress_Received++;
                if (Adress_Received >= 14) {
                    State_Machine_RX = STATE_CAPTURE_DONNEES;
                    rx_idx_octet_paquet = 0;
                }
            } else {
                State_Machine_RX = STATE_RECHERCHE_FLAG; // Mismatch, reset pipeline
            }
        }

        /* --- 5. DATA CAPTURE AND FRAME LIMIT PATTERN --- */
        else if (State_Machine_RX == STATE_CAPTURE_DONNEES)
        {
            // If trailing 0x7E flag is caught, close packet
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
## 🔍 Callback Filter Breakdown

### Noise & Preamble Check
Filters data ripples. It checks for deadlocks or non-modulated waves by evaluating continuous matching streams.

```c
if (State_Machine_RX == STATE_RECHERCHE_FLAG) {
    if (Bit_Received == LastBit_Received) {
        Bit_Identical_Counter++;
        if (Bit_Identical_Counter >= 16) return;
    } else {
        Bit_Identical_Counter = 0;
        LastBit_Received = Bit_Received;
    }
}
```
### AX.25 Opening Flag Detection
Shifts incoming bitstreams through a sliding window register to trap the standard AX.25 frame starter marker (0x7E / 01111110).

```c
if (State_Machine_RX == STATE_RECHERCHE_FLAG) {
    Register_Swipe = (Register_Swipe << 1) | Bit_Received;
    if (Register_Swipe == 0x7E) {
        State_Machine_RX = STATE_VERIF_ADRESSE;
        Adress_Received = 0;
        Bit_Counter = 7;
        Temporary_Byte = 0x00;
    }
    return;
}
```

### Hardware Address Matching
Secures MCU stack allocation space. It drops unneeded processing steps early if the payload routing index doesn't match the localized token.

```c
if (State_Machine_RX == STATE_VERIF_ADRESSE) {
    if (octet_valide == adresse_cible[Adress_Received]) {
        rx_trame.adresse[Adress_Received] = octet_valide;
        Adress_Received++;
        if (Adress_Received >= 14) {
            State_Machine_RX = STATE_CAPTURE_DONNEES;
            rx_idx_octet_paquet = 0;
        }
    } else {
        State_Machine_RX = STATE_RECHERCHE_FLAG;
    }
}
```
