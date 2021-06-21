/* 
 *  
 * MIDI System Exclusive Implementation for the Nava
 * Manufacturer ID: 0x7D
 * DeviceID: 0x07, 0x1A (909 shifted 1 bit to the left to fit into the 7 data bits.
 * CommandID: See Sysex.h
 * CommandParameter: byte indicating pattern bank / track/ 0 if not used 
 * 
 */

#include <Arduino_CRC32.h>
#include "Sysex.h"

Arduino_CRC32 crc; // See https://github.com/bakercp/CRC32 for info

/* Convert 8 bit data to 8 bytes containing 7 bits
 * (0 MSB7 MSB6 MSB5 MSB4 MSB3 MSB2 MSB1) 
 * (BYTE1 & 0x7F) (BYTE2 & 0x7F) (BYTE3 & 0x7F) 
 * (BYTE4 & 0x7F) (BYTE5 & 0x7F) (BYTE6 & 0x7F) 
 * (BYTE7 & 0x7F)  
 */
uint8_t data_to_sysex(uint8_t *data, uint8_t *sysex, uint8_t len) {
  uint8_t retlen = 0;
  uint8_t cnt;
  uint8_t cnt7 = 0;

  sysex[0] = 0;
  for (cnt = 0; cnt < len; cnt++) {
    uint8_t c = data[cnt] & 0x7F;
    uint8_t msb = data[cnt] >> 7;
    sysex[0] |= msb << cnt7;
    sysex[1 + cnt7] = c;

    if (cnt7++ == 6) {
      sysex += 8;
      retlen += 8;
      sysex[0] = 0;
      cnt7 = 0;
    }
  }
  return retlen + cnt7 + (cnt7 != 0 ? 1 : 0);
}

/* Convert the sysex data back to normal data */
uint8_t sysex_to_data(uint8_t *sysex, uint8_t *data, uint8_t len) {
  uint8_t cnt;
  uint8_t cnt2 = 0;
  uint8_t bits = 0;
  for (cnt = 0; cnt < len; cnt++) {
    if ((cnt % 8) == 0) {
      bits = sysex[cnt];
    } else {
      data[cnt2++] = sysex[cnt] | ((bits & 1) << 7);
      bits >>= 1;
    }
  }
  return cnt2;
}
