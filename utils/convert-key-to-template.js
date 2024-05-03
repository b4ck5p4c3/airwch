#!/usr/bin/env node

const keyString = process.argv[2];
if (!keyString)
  throw new Error('Key is empty');

const key = Buffer.from(keyString, 'base64');
if (key.length !== 28)
  throw new Error(`Incorrect key size: decoded ${key.length} bytes, but 28 bytes expected`);

const keyHex = key
  // Convert into hex
  .toString('hex')

  // Split hex string into single byte length string (two char chunks)
  .match(/.{1,2}/g)

  // Append "0x" designator to each chunk
  .map(b => `0x${b}`);

// Find My BLE advertisement payload template
const snippet = `
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
    ${keyHex.slice(6, 14).join(', ')},
    ${keyHex.slice(14, 22).join(', ')},
    ${keyHex.slice(22, 28).join(', ')},
    0x${(key[0] >> 6).toString(16)}, 0x00
};
`;

console.log(snippet);