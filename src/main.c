#include "config.h"
#include "HAL.h"

__attribute__((aligned(4))) uint32_t mem_buffer[BLE_MEMHEAP_SIZE / 4];

#define DEFAULT_ADVERTISING_INTERVAL 2000

const uint8_t kMacAddress[6] =
        { 0xc0, 0xe5, 0xcb, 0x59, 0xe9, 0xc3 };

static uint8_t advertisement_data[] = {
        /* // Flags; this sets the device to use limited discoverable
        // mode (advertises for 30 seconds at a time) instead of general
        // discoverable mode (advertises indefinitely)
        0x02, // length of this data
        GAP_ADTYPE_FLAGS,
        GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

        // Broadcast of the data
        0x04,                             // length of this data including the data type byte
        GAP_ADTYPE_MANUFACTURER_SPECIFIC, // manufacturer specific advertisement data type
        'b', 'l', 'e', 0x04,
        GAP_ADTYPE_LOCAL_NAME_SHORT,
        'A', 'B', 'c' */
        30, 0xFF,
        0x4c, 0x00, 0x12, 0x19, 0x00,
        0xe8, 0x66, 0x0b, 0xe0, 0x56, 0xf2, 0xa6, 0x82,
        0x58, 0x85, 0xe2, 0x5a, 0xc5, 0x9e, 0xda, 0x2c,
        0x2b, 0xbf, 0x59, 0x6f, 0x35, 0xc9,
        0x1, 0x00
};

static gapRolesBroadcasterCBs_t broadcaster_callbacks = {NULL, NULL};

void InitBroadcaster() {
    uint8_t initial_advertising_enable = TRUE;
    uint8_t initial_adv_event_type = GAP_ADTYPE_ADV_NONCONN_IND;
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &initial_advertising_enable);
    GAPRole_SetParameter(GAPROLE_ADV_EVENT_TYPE, sizeof(uint8_t), &initial_adv_event_type);
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertisement_data), advertisement_data);
    GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, DEFAULT_ADVERTISING_INTERVAL);
    GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, DEFAULT_ADVERTISING_INTERVAL);
    GAPRole_BroadcasterStartDevice(&broadcaster_callbacks);
}

int main() {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

    CH58X_BLEInit();
    HAL_Init();
    GAPRole_BroadcasterInit();
    InitBroadcaster();
    HAL_SleepInit();

    while (1) {
        TMOS_SystemProcess();
    }
}