## 🔒 Payload Encryption Guide

By default, standard AX.25 packets transmit payloads in clear text. If you are using the RFM69HCW on open ISM bands (like 868 MHz or 915 MHz) for private telemetry or IoT applications, anyone with a software-defined radio (SDR) can read your data. 

To secure your communications, you can encrypt the data **before** encapsulating it into the AX.25 frame.

---

### 🗺️ Data Flow Architecture

```text
[ Clear Text Data ] 
       │
       ▼  (External Crypto Library: AES-128 / ChaCha20)
[ Encrypted Bytes ]
       │
       ▼  (Your AX.25 Stack)
[ AX.25 Frame Construction ] -> [ Info Field = Encrypted Bytes ]
       │
       ▼  (RFM69 Driver)
[ RFM69HCW RAW Mode ] -> Transmitted over the air 📡
