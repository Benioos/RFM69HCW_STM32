# 🔒 Payload Encryption Guide

By default, AX.25 payloads are transmitted in clear text.

Anyone using:
- SDR hardware,
- packet sniffers,
- or another RFM69 module,

can potentially read transmitted data over open ISM bands such as:
- 433 MHz
- 868 MHz
- 915 MHz

To secure communications, encrypt the payload **before** transmitting the AX.25 frame.

### ✔️ Take a look

> [!WARNING]
> The encrypted buffer must not exceed **256 bytes**.

> [!IMPORTANT]
> Both TX and RX must use:
> - the exact same AES key,
> - the exact same encryption library,
> - the exact same payload structure.

> [!TIP]
> You can use your own AES implementation instead of the provided one.

###  ✔️ Encryption (TX)

```c
uint8_t Cipher_Key[16] = {
    0x2B,0x7E,0x15,0x16,
    0x28,0xAE,0xD2,0xA6,
    0xAB,0xF7,0x15,0x88,
    0x09,0xCF,0x4F,0x3C
};

uint8_t RoundKey[176];

KeyExpansion(RoundKey, Cipher_Key);

while (1)
{
       AES128_Encrypt_Buffer256((uint8_t *)&trame.payload, RoundKey);
}


```

###  ✔️ Decryption (RX)

```c
uint8_t Cipher_Key[16] = {
    0x2B,0x7E,0x15,0x16,
    0x28,0xAE,0xD2,0xA6,
    0xAB,0xF7,0x15,0x88,
    0x09,0xCF,0x4F,0x3C
};

uint8_t RoundKey[176];

KeyExpansion(RoundKey, Cipher_Key);

while (1)
{
       AES128_Decrypt_Buffer256((uint8_t *)&rx_Trame.payload,RoundKey);
}


```



