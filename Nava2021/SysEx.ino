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

#define CHECKSUMSIZE 8
/* Convert 8 bit data to 8 bytes containing 7 bits
 * (0 MSB7 MSB6 MSB5 MSB4 MSB3 MSB2 MSB1) 
 * (BYTE1 & 0x7F) (BYTE2 & 0x7F) (BYTE3 & 0x7F) 
 * (BYTE4 & 0x7F) (BYTE5 & 0x7F) (BYTE6 & 0x7F) 
 * (BYTE7 & 0x7F)  
 */

 void EnableSysexMode()
 {
  seq.SysExMode = true;
  Serial.println("Sysex Receive enabled");
  TimerStop();
  initTrigTimer();
  DisconnectMidiHandleNote();                        
  DisconnectMidiHandleRealTime();
//  MIDI.Settings.SysExMaxSize(Serial1,500,MIDI.Platform);
  ConnectMidiSysex();
 }
 
uint16_t data_to_sysex(uint8_t *data, uint8_t *sysex, uint16_t len) {
  uint16_t retlen = 0;
  uint16_t cnt;
  uint16_t cnt7 = 0;

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
uint16_t sysex_to_data(uint8_t *sysex, uint8_t *data, uint16_t len) {
  uint16_t cnt;
  uint16_t cnt2 = 0;
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

uint16_t build_sysex(uint8_t *data, uint16_t datasize, uint8_t sysex_type, uint8_t param)
{     
  char header[]={ START_OF_SYSEX, SYSEX_MANUFACTURER, SYSEX_DEVID_1, SYSEX_DEVID_2, sysex_type, param};
  //uint8_t *SysEx = (uint8_t *)MIDI.getSysExArray();
  uint8_t SysEx[2100];
  // Copy the header to the buffer
  memcpy(SysEx, header, sizeof(header));

  // Convert data for 7 bit transfers
  uint16_t sysexSize=sizeof(header) + data_to_sysex(data,SysEx+sizeof(header),datasize);

  // Calculate checksum for the sysex
  uint32_t checksum = crc.calc(SysEx, sysexSize);

  // Add the crc and end of sysex at the end
  SysEx[sysexSize++] = (checksum >> 28) & 0xf;
  SysEx[sysexSize++] = (checksum >> 24) & 0xf;
  SysEx[sysexSize++] = (checksum >> 20) & 0xf;
  SysEx[sysexSize++] = (checksum >> 16) & 0xf;
  SysEx[sysexSize++] = (checksum >> 12) & 0xf;
  SysEx[sysexSize++] = (checksum >> 8) & 0xf;
  SysEx[sysexSize++] = (checksum >> 4) & 0xf;
  SysEx[sysexSize++] = checksum & 0xf;
  SysEx[sysexSize++]=END_OF_SYSEX;

#if DEBUG == 2
  Serial.println();
  Serial.print("Rawdata Size: ");Serial.println(datasize);
  Serial.print("Header Size: ");Serial.println(HEADERSIZE);
  Serial.println("Checksum: ");Serial.println(checksum,HEX);
  for(int i=0; i<sysexSize + 1; i++)
  {
    if ( i % 8 == 0 ) 
    {
      Serial.println();
      Serial.print(i,HEX);Serial.print(":\t");
    }
    Serial.print("0x");Serial.print(SysEx[i], HEX); Serial.print("\t");
    if ( SysEx[i] == END_OF_SYSEX ) 
    {
      Serial.println();
      Serial.print("Sysex Size: "); Serial.println(i+1);
      break;
    }
  }
  memory("Just before sending sysex");
  Serial.print("Sysex TX size: "); Serial.println(sysexSize);
#endif
  MIDI.sendSysEx(sysexSize,SysEx,true);
  // return system exclusive size
  return(sysexSize);
}

void DumpPattern(byte patternNbr)
{
  byte RawData[PTRN_SIZE];
  int datacount = 0;

  unsigned long adress = (unsigned long)(PTRN_OFFSET + patternNbr * PTRN_SIZE);
  WireBeginTX(adress); 
  Wire.endTransmission();
  Wire.requestFrom(HRDW_ADDRESS,MAX_PAGE_SIZE); //request a 64 bytes page
  //TRIG-----------------------------------------------
  for(int i =0; i<NBR_INST;i++){
    RawData[datacount++] = Wire.read(); 
    RawData[datacount++] = Wire.read();
  }
  //SETUP-----------------------------------------------
  RawData[datacount++] = Wire.read();
  RawData[datacount++] = Wire.read();
  RawData[datacount++] = Wire.read();                                                         // [zabox] [1.027] flam
  RawData[datacount++] = Wire.read();                                                            // [zabox] [1.027] flam
  RawData[datacount++] = Wire.read();
  RawData[datacount++] = Wire.read();
  RawData[datacount++] = Wire.read();
  RawData[datacount++] = Wire.read();
  for(int a = 0; a < 24; a++){
    RawData[datacount++]=Wire.read();
  }
  //EXT INST-----------------------------------------------
  for(int nbrPage = 0; nbrPage < 2; nbrPage++){
    adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_SETUP_OFFSET);
    WireBeginTX(adress);
    Wire.endTransmission();
    Wire.requestFrom(HRDW_ADDRESS,MAX_PAGE_SIZE); //request of  64 bytes

    for (byte j = 0; j < MAX_PAGE_SIZE; j++){
      RawData[datacount++] = Wire.read();
    }
  }
  //VELOCITY-----------------------------------------------
  for(int nbrPage = 0; nbrPage < 4; nbrPage++){
    adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_EXT_OFFSET);
    WireBeginTX(adress);
    Wire.endTransmission();
    Wire.requestFrom(HRDW_ADDRESS,MAX_PAGE_SIZE); //request of  64 bytes
    for (byte i = 0; i < 4; i++){//loop as many instrument for a page
      for (byte j = 0; j < NBR_STEP; j++){
        RawData[datacount++] = (Wire.read() & 0xFF);
      }
    }
  }
  uint16_t transmit_size=build_sysex(RawData, sizeof(RawData), NAVA_PTRN_DMP, patternNbr);
#ifdef DEBUG
  memory("Dump ptr transmit");
#endif
}

void GetPattern(byte * sysex, uint16_t RawSize)
{
  byte NavaData[PTRN_SIZE];
  uint8_t patternNbr = sysex[5];
  RawSize = RawSize - 6; // Size of the header
  RawSize = RawSize - 9; // The checksum at the end + EOX
  uint16_t NavaBytes = sysex_to_data(sysex + 6, NavaData, RawSize); // Transmitted Bank block = 2064 bytes;

  unsigned long adress = (unsigned long)(PTRN_OFFSET + patternNbr * PTRN_SIZE);
  WireBeginTX(adress); 
  //TRIG-----------------------------------------------
  for (uint16_t i = 0; i < PTRN_SIZE; i++){
    if ( (i % MAX_PAGE_SIZE) == 0 && i > 0) 
    {
      Wire.endTransmission();//end page transmission
      delay(DELAY_WR);//delay between each write page
      adress = (unsigned long)(PTRN_OFFSET + patternNbr * PTRN_SIZE) + i;
      WireBeginTX(adress);
    }
    Wire.write((byte)(NavaData[i]));
  }
  Wire.endTransmission();//end page transmission
#if DEBUG  
  Serial.print("Get Pattern");Serial.println(patternNbr);
#endif
}

void DumpBank(byte selectedBank)
{
  byte RawData[BANK_PARTS*PTRN_SIZE]; // Need to do the bank dump in two parts as there isn't enough memory to have the rawdata and the sysex array in memory.
  int datacount = 0;
  
  for ( int BankPart=0; BankPart < BANK_PARTS; BankPart++)
  { 
    int datacount = 0;
    uint8_t patNum = selectedBank * NBR_PATTERN + (BANK_PARTS * BankPart);

    for ( int ptrn = 0 ; ptrn < ( NBR_PATTERN / BANK_PARTS ); ptrn++ )
    {
      uint8_t patternNbr = patNum + ptrn; 
#if DEBUG      
      Serial.print("Pattern Nbr: "); Serial.println(patternNbr);
#endif      
      unsigned long adress = (unsigned long)(PTRN_OFFSET + patternNbr * PTRN_SIZE);
      WireBeginTX(adress); 
      Wire.endTransmission();
      Wire.requestFrom(HRDW_ADDRESS,MAX_PAGE_SIZE); //request a 64 bytes page
      //TRIG-----------------------------------------------
      for(int i =0; i<NBR_INST;i++){
        RawData[datacount++] = Wire.read(); 
        RawData[datacount++] = Wire.read();
      }
      //SETUP-----------------------------------------------
      RawData[datacount++] = Wire.read();
      RawData[datacount++] = Wire.read();
      RawData[datacount++] = Wire.read();                                                         // [zabox] [1.027] flam
      RawData[datacount++] = Wire.read();                                                            // [zabox] [1.027] flam
      RawData[datacount++] = Wire.read();
      RawData[datacount++] = Wire.read();
      RawData[datacount++] = Wire.read();
      RawData[datacount++] = Wire.read();
      for(int a = 0; a < 24; a++){
        RawData[datacount++]=Wire.read();
      }
      //EXT INST-----------------------------------------------
      for(int nbrPage = 0; nbrPage < 2; nbrPage++){
        adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_SETUP_OFFSET);
        WireBeginTX(adress);
        Wire.endTransmission();
        Wire.requestFrom(HRDW_ADDRESS,MAX_PAGE_SIZE); //request of  64 bytes
    
        for (byte j = 0; j < MAX_PAGE_SIZE; j++){
          RawData[datacount++] = Wire.read();
        }
      }
      //VELOCITY-----------------------------------------------
      for(int nbrPage = 0; nbrPage < 4; nbrPage++){
        adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_EXT_OFFSET);
        WireBeginTX(adress);
        Wire.endTransmission();
        Wire.requestFrom(HRDW_ADDRESS,MAX_PAGE_SIZE); //request of  64 bytes
        for (byte i = 0; i < 4; i++){//loop as many instrument for a page
          for (byte j = 0; j < NBR_STEP; j++){
            RawData[datacount++] = (Wire.read() & 0xFF);
          }
        }
      }
    }
    uint16_t transmit_size=build_sysex( RawData, BANK_PARTS*PTRN_SIZE, NAVA_BANK_DMP, selectedBank + BANK_PARTS * BankPart);
  }
}

void GetBank(byte * sysex, uint16_t RawSize)
{
  byte NavaData[BANK_PARTS * PTRN_SIZE];

  uint8_t BankId = sysex[5] & 0xF;
  uint8_t ptrnGrp = (sysex[5] - BankId) / 16;

  RawSize = RawSize - 6; // Size of the header
  RawSize = RawSize - 9; // The checksum at the end + EOX
  uint16_t NavaBytes = sysex_to_data(sysex + 6, NavaData, RawSize); // Transmitted Bank block = 2064 bytes;
#if DEBUG
  Serial.print("BankId: ");Serial.println(BankId);
  Serial.print("ptrnGrp: ");Serial.println(ptrnGrp);
  Serial.print("Nava Bytes: "); Serial.println(NavaBytes);
#endif
  
  unsigned long adress;
  //TRIG-----------------------------------------------
  for (uint16_t nbrPage = 0; nbrPage < (BANK_PARTS * PTRN_SIZE)/MAX_PAGE_SIZE; nbrPage++)
  {
    adress = (unsigned long)(PTRN_OFFSET + ((NBR_PATTERN*BankId + BANK_PARTS * ptrnGrp) * PTRN_SIZE) + nbrPage * MAX_PAGE_SIZE);
    WireBeginTX(adress);
    for ( int i = 0 ; i < MAX_PAGE_SIZE ; i++ )
    {
      Wire.write((byte)(NavaData[i + ( MAX_PAGE_SIZE * nbrPage)]));
    }
    Wire.endTransmission();//end of 64 bytes transfer
    delay(DELAY_WR);//delay between each write page
  }
}

void DumpTrack(byte trackNbr)
{
  byte RawData[TRACK_SIZE];
  
  unsigned long adress;
  for(int nbrPage = 0; nbrPage < TRACK_SIZE/MAX_PAGE_SIZE; nbrPage++){
    adress = (unsigned long)(PTRN_OFFSET + (trackNbr * TRACK_SIZE) + (MAX_PAGE_SIZE * nbrPage) + TRACK_OFFSET);
    WireBeginTX(adress);
    Wire.endTransmission();
    if (adress > 65535) Wire.requestFrom(HRDW_ADDRESS_UP,MAX_PAGE_SIZE); //request of  64 bytes
    else Wire.requestFrom(HRDW_ADDRESS,MAX_PAGE_SIZE); //request of  64 bytes
    for (byte i = 0; i < MAX_PAGE_SIZE; i++){//loop as many instrument for a page
      RawData[i + (MAX_PAGE_SIZE * nbrPage)] = (Wire.read() & 0xFF); 
    }
  }

  uint16_t transmit_size=build_sysex(RawData, TRACK_SIZE, NAVA_TRACK_DMP, trackNbr);
}

void GetTrack(byte *sysex, uint16_t RawSize)
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

  uint16_t transmit_size=build_sysex( RawData, SETUP_SIZE, NAVA_CONFIG_DMP,0);
}

void GetConfig(byte *sysex)
{
  byte RawData[SETUP_SIZE];

  sysex_to_data(sysex + 6, RawData, 74);
  seq.sync = RawData[0];
  seq.defaultBpm = RawData[1];
  seq.TXchannel = RawData[2];
  seq.RXchannel = RawData[3];
  seq.ptrnChangeSync = RawData[4];
  seq.muteModeHH = RawData[5];
  seq.EXTchannel = RawData[6];
  seq.BootMode  = (SeqMode)RawData[7];

  SaveSeqSetup();
//  SetSeqSync();
  needLcdUpdate = true;
}

void MidiSendSysex(byte Type, byte Param)
{
  switch(Type)
  {
    case NAVA_BANK_DMP: // Bank
        DumpBank(Param);
        break;
    case NAVA_PTRN_DMP: // Pattern
        DumpPattern(Param);
        break;
    case NAVA_TRACK_DMP: // Track
        DumpTrack(Param);
        break;
    case NAVA_CONFIG_DMP: // Config
        DumpConfig();
    case NAVA_FULL_DMP: // Full
      break; 
  }
}

#if DEBUG
void PrintSysex(byte *sysex, unsigned size)
{
  Serial.print("Print Sysex size: "); Serial.println(size);
  for(int i=0; i<size; i++)
  {
    if ( i % 8 == 0 ) 
    {
      Serial.println();
      Serial.print(i,HEX);Serial.print(":\t");
    }
    Serial.print("0x");Serial.print(sysex[i], HEX); Serial.print("\t");
  }
  Serial.println();
  Serial.print("Sysex Size: "); Serial.println(size);

}

#endif
void HandleSystemExclusive(byte * RawSysEx, unsigned RawSize)
{
//  Serial.println(MIDI:)
  char header[]={ START_OF_SYSEX, SYSEX_MANUFACTURER, SYSEX_DEVID_1, SYSEX_DEVID_2 };
  int16_t DataPointer=6;
  // Check if the sysex is for us.
  if ( memcmp(header, RawSysEx, sizeof(header)) != 0 ) return;

  RawSysEx[RawSize -1]= END_OF_SYSEX;

  // Get type and parameter
  byte Type=RawSysEx[4];
  byte Param=RawSysEx[5];
  
  // Do calculate checksum and compare to received checksum
  uint32_t checksum = crc.calc(RawSysEx, RawSize - CHECKSUMSIZE -1);
  uint32_t RxChecksum = 0;

  // Reconstruct received checksum
  for(int i=RawSize - CHECKSUMSIZE; i < RawSize; i++)
  {
    RxChecksum = ( RxChecksum << 4 ) | RawSysEx[i-1];
  }

#if DEBUG
  Serial.print("Calculated checksum: "); Serial.println(checksum, HEX);
  Serial.print("Received checksum: "); Serial.println(RxChecksum, HEX);
//  PrintSysex(RawSysEx, RawSize);
#endif  

  if ( checksum != RxChecksum )
  {
//    Serial.print("CREATE ERROR HANDLING");
    return;
  }

  seq.configMode = true;
  seq.configPage = 3;
  needLcdUpdate = TRUE;
 // EnableSysexMode();

#if DEBUG
  Serial.print("Type = "); Serial.println(Type, HEX);
Serial.print("Rawsize: "); Serial.println(RawSize);  
#endif
  switch(Type)
  {
  case NAVA_BANK_REQ: 
    {
      if ( Param < 8 )
      { 
        sysExParam = Param;
        sysExDump = NAVA_BANK_DMP;
        DumpBank(Param);
      }
      break; 
    }
  case NAVA_BANK_DMP:
    {
      if ( RawSize != 2063 ) return;
      GetBank(RawSysEx, RawSize);
      break;     
    }
  case NAVA_PTRN_REQ:
    {
      if ( Param < 128 )
      {
        sysExDump = NAVA_PTRN_DMP;
        sysExParam = Param;
        DumpPattern(Param);
      }
      break;  
    }
   case NAVA_PTRN_DMP:
    {
      if ( Param < 128 )
      {
        if ( RawSize != 527 ) return;
        GetPattern(RawSysEx, RawSize);
      }
      break;  
    }
  case NAVA_TRACK_REQ:
    {
      if ( Param < 16 )
      {
        sysExDump = NAVA_TRACK_DMP;
        sysExParam = Param;
        DumpTrack(Param);
      }
      break;
    }
  case NAVA_TRACK_DMP:
    {
      if ( Param < 16 )
      {
        if ( RawSize != 1186 ) return;
        GetTrack(RawSysEx, RawSize);
      }
    }
  case NAVA_CONFIG_REQ:
    { 
      Serial.println("Dumping Config");
      sysExDump = NAVA_CONFIG_DMP;
      DumpConfig();
      break;
    }
   case NAVA_CONFIG_DMP:
    {
      sysExDump = NAVA_CONFIG_DMP;
      GetConfig(RawSysEx);
      break; 
    }
  }
}

#endif // MIDI_HAS_SYSEX
