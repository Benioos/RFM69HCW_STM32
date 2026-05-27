## 🧪 Troubleshooting Guide

If you encounter any issues while setting up or running the project, find your symptom below to see how to fix it.

---

### 🪲 Symptom: No SPI Connection, RFM69_getConfigData() return 0 data.

> [!CAUTION]
> **What is happening:** `RegVersion` returns `0x00` or `0xFF` instead of `0x24`.

> [!WARNING]
> **Possible Causes:**
> * SPI wiring issue (MISO/MOSI inverted or loose wires).
> * Incorrect Slave Select (CS) pin configuration in STM32 CubeMX.
> * The module is not powered properly or missing a common Ground.

> [!NOTE]
> **How to fix:**
> 1. Double-check your SPI pins against your STM32 microcontroller pinout.
> 2. Ensure the module is properly powered with a stable **3.3V** source.

---

### 🪲 Symptom: PLL Never Locked

> [!CAUTION]
> **What is happening:** The PLL Never Locked.

> [!WARNING]
> **Possible Causes:**
> * A wire is not well fixed.

> [!NOTE]
> **How to fix:**
> 1. Check all the wires.

### 🪲 Symptom: OOK modulation

> [!CAUTION]
> **What is happening:** Something wrong with OOK modulation.

> [!WARNING]
> **Possible Causes:**
> * Code haven't been tested with OOK modulation yet.

> [!NOTE]
> **How to fix:**
> 1. Change to FSK
