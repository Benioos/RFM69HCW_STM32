## 🧪 Connection Troubleshooting

If the module does not respond correctly, check the SPI communication.

### ✔️ Step 1 — Verify chip detection

Read the register:

- `RegVersion` = `0x10`

Expected value:
- `0x24`

👉 If the value is different, it means:
- SPI wiring issue, or
- incorrect configuration
