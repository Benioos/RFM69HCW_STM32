## 📟 RAW Mode Explanation

## The RAW mode 

![Schéma de la trame AX25](Image/Trame.png)

By default, the RFM69HCW is designed to handle packet processing automatically (adding its own preambles, sync words, and CRC). However, the standard AX.25 frame used in packet radio and APRS has its own strict structure, custom CRC (FCS), and bit-stuffing mechanism that do not match the RFM69 chip's native packet engine.

To bypass the chip's internal packet processing and gain full control over every single bit sent over the air, we use the **RAW Mode** (also known as Continuous Mode).

### How it Works
In RAW mode, the RFM69HCW acts as a "dumb" radio pipe:
* **In Transmission (TX):** The microcontroller manually generates the entire AX.25 frame—including the `0x7E` flags, shifted addresses, control bytes, payload, and the calculated FCS. It then feeds this bitstream directly to the RFM69's `DATA` pin in real-time, matching the desired bitrate (usually 1200 bps AFSK or 9600 bps GFSK).
* **In Reception (RX):** The chip simply demodulates the incoming radio signal and dumps the raw bitstream onto the `DATA` pin. The microcontroller must constantly poll this pin to look for the `0x7E` preamble flag, perform bit-unstuffing, and validate the FCS.

🔒 Data Security & Encryption
Since we have absolute control over the payload in RAW mode, the data inside the Information Field can be fully encrypted before being packed into the AX.25 frame.

By integrating an external encryption library (such as custom AES, ChaCha20, or XOR libraries available on GitHub), you can secure the payload over the air. The receiving end will capture the raw AX.25 frame, extract the payload, and decrypt it using the same library and cryptographic key.
