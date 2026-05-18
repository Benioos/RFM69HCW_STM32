#include <stdio.h>
#include <RFM69HCW.h>

int main(void)
{
  RFM69_Init(&hspi1, GPIOC, GPIO_PIN_7);
  RFM69_SetMode(RFM69_MODE_STDBY);

  RFM69_SetFrequencyTo433();
  RFM69_SetBitrate(4800);
  
  RFM69_SetDataProcessingMode(RFM69_CONTINUOUS_SYNC);
  RFM69_SetModulationType(RFM69_MODUL_FSK);
  RFM69_SetDataShaping(RFM69_SHAPING_Gaussianfilter_BT1);
  
  RFM69_AutoMessageDetectionReceive_Config(RFM69_SYNC_ON, RFM69_FIFO_FILL_IF_SYNC, 2, RFM69_SYNC_TOLERANCE_ERROR_2);
  uint8_t mykey[] = {0x42, 0x24};
  RFM69_SetKeyValues_Sync(mykey, 2);

  RFM69_AutoSetFdev();
  RFM69_PowerAmplifierSelection(PA_1, 13);

  uint8_t message[] = "SATELLITE_TEST";
  RFM69_SetPacketConfig(RFM69_PACKET_VARIABLE, RFM69_DC_FREE_NONE, RFM69_CRC_ON, RFM69_CRC_AUTOCLEAR_ON, RFM69_FILTER_NONE);
}

while (1)
{
  RFM69_SendMessage( message, 14);
  HAL_Delay(5000);
}
