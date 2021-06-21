/* 
 *  
 * MIDI System Exclusive Implementation for the Nava
 * Manufacturer ID: 0x7D
 * DeviceID: 0x07, 0x1A (909 shifted 1 bit to the left to fit into the 7 data bits.
 * CommandID: See Sysex.h
 * CommandParameter: byte indicating pattern bank / track/ 0 if not used 
 * 
 */

#if MIDI_HAS_SYSEX
#include <Arduino_CRC32.h>
#include "Sysex.h"

Arduino_CRC32 crc; // See https://github.com/bakercp/CRC32 for info

#define CHECKSUMSIZE 5
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

uint8_t build_sysex(uint8_t *sysex, uint8_t *data, uint16_t datasize, uint8_t sysex_type, uint8_t param)
{     
  char header[]={ START_OF_SYSEX, SYSEX_MANUFACTURER, SYSEX_DEVID_1, SYSEX_DEVID_2, sysex_type, param};

  // Copy the header to the buffer
  memcpy(sysex, header, sizeof(header));

  // Convert data for 7 bit transfers
  uint16_t sysexSize=sizeof(header)+data_to_sysex(data,sysex+sizeof(header),datasize);

  // Calculate checksum for the sysex
  uint32_t checksum = crc.calc(sysex, sysexSize);


  // Add the crc and end of sysex at the end
  sysexSize=sysexSize+data_to_sysex((uint8_t *)&checksum, sysex+sysexSize,4);
  sysex[sysexSize]=END_OF_SYSEX;

#ifdef DEBUG
  Serial.println();
  Serial.print("Rawdata Size: ");Serial.println(datasize);
  Serial.print("Header Size: ");Serial.println(sizeof(header));
  Serial.println("Checksum: ");Serial.println(checksum,HEX);
  for(int i=0; i<SYSEX_BUFFER_SIZE; i++)
  {
    if ( i % 8 == 0 ) 
    {
      Serial.println();
      Serial.print(i,HEX);Serial.print(":\t");
    }
    Serial.print("0x");Serial.print(sysex[i], HEX); Serial.print("\t");
    if ( sysex[i] == END_OF_SYSEX ) 
    {
      Serial.println();
      Serial.print("Sysex Size: "); Serial.println(i+1);
      break;
    }
  }
#endif
  // return system exclusive size
  return(sysexSize+1);
}

void DumpBank(byte selectedBank)
{
  
}

void DumpTrack(byte selectedTrack)
{
  
}

void DumpConfig()
{
  byte RawData[SETUP_SIZE];
  RawData[0]=(byte)(seq.sync); 
  RawData[1]=(byte)(seq.defaultBpm);
  RawData[2]=(byte)(seq.TXchannel);
  RawData[3]=(byte)(seq.RXchannel);
  RawData[4]=(byte)(seq.ptrnChangeSync);
  RawData[5]=(byte)(seq.muteModeHH);                                  // [zabox]
#if MIDI_EXT_CHANNEL
  RawData[6]=(byte)(seq.EXTchannel);  // [Neuromancer]
#endif
#if CONFIG_BOOTMODE
  RawData[7]=(byte)(seq.BootMode); // [Neuromancer]
#endif
  byte *SysEx = (byte *)calloc(SYSEX_BUFFER_SIZE, sizeof(byte));

  uint16_t transmit_size=build_sysex(SysEx, RawData, sizeof(RawData), NAVA_CONFIG_DMP,0);

  MIDI.sendSysEx(transmit_size,SysEx,false);
  free(SysEx);
}

void MidiSendSysex(byte Type, byte Param)
{
  switch(Type)
  {
    case 0: // Bank
        DumpBank(Param);
        break;
    case 1: // Track
        DumpTrack(Param);
        break;
    case 2: // Config
        DumpConfig();
    case 3: // Full
      break; 
  }
}

void HandleSystemExclusive(byte * RawSysEx, byte RawSize)
{
  
}
#endif // MIDI_HAS_SYSEX
