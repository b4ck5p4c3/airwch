#include "HAL.h"
#include "config.h"

#define CHANNEL_MIN 37
#define CHANNEL_MAX 39
#define NUM_PACKETS 3
#define SEND_PACKETS_PERIOD 10000

__attribute__((aligned(4))) uint32_t mem_buffer[BLE_MEMHEAP_SIZE / 4];

const uint8_t kUnusedMacAddress[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

uint8_t kTXData[37] = {0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99,
                       0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1,
                       0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9,
                       0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1,
                       0xb2, 0xb3, 0xb4, 0xb5, 0xb6};

void RFStatusCallback(uint8_t status, uint8_t crc, uint8_t *rx_buffer) {}

void SendPackets() {
  rfConfig_t rfConfig;

  for (int channel = CHANNEL_MIN; channel <= CHANNEL_MAX; channel++) {
    tmos_memset(&rfConfig, 0, sizeof(rfConfig_t));
    rfConfig.accessAddress = 0x8e89bed6;
    rfConfig.CRCInit = 0x555555;
    rfConfig.Channel = channel;
    rfConfig.LLEMode = LLE_MODE_BASIC;
    rfConfig.rfStatusCB = RFStatusCallback;
    rfConfig.RxMaxlen = 251;
    RF_Config(&rfConfig);

    for (int i = 0; i < NUM_PACKETS; i++) {
      RF_Tx(kTXData, sizeof(kTXData), 0x02, 0xFF);
    }
  }
  RF_Shut();
}

#define SBP_SEND_PACKETS_EVT 4

tmosEvents SenderTaskCallback(tmosTaskID task_id, tmosEvents event) {
  if (event & SYS_EVENT_MSG) {
    uint8_t *message;

    if ((message = tmos_msg_receive(task_id)) != NULL) {
      tmos_msg_deallocate(message);
    }

    return event ^ SYS_EVENT_MSG;
  }

  if (event & SBP_SEND_PACKETS_EVT) {
    SendPackets();

    return event ^ SBP_SEND_PACKETS_EVT;
  }

  return 0;
}

int main() {
  SetSysClock(CLK_SOURCE_PLL_60MHz);

  CH58X_BLEInit();
  HAL_Init();
  RF_RoleInit();

  int sender_task_id = TMOS_ProcessEventRegister(SenderTaskCallback);

  tmos_set_event(sender_task_id, SBP_SEND_PACKETS_EVT);
  tmos_start_reload_task(sender_task_id, SBP_SEND_PACKETS_EVT,
                         SEND_PACKETS_PERIOD * 8 / 5);

  while (1) {
    TMOS_SystemProcess();
  }
}