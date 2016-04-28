#ifndef _PATTERN_H_
#define _PATTERN_H_

#include <inttypes.h>
#include "define.h"
#include "EEProm.h"

//template< class T>
class CPattern {
  typedef enum Velo { off, low, high} Velo;

private:
  unsigned char nmbr;
  byte length;//0 to 15 steps
  byte scale;
  byte dir;//0=>forward, 1=>backword, 2=>ping-pong, 3=>random
  byte shuffle;
  byte flam; // 10, 14, 18, 22,26,30,34,38 ms, not affected by tempo
  unsigned int inst[NBR_INST]; // The binary pattern for each instrument 
  unsigned int step[NBR_STEP];
  Velo Velocity[NBR_INST][NBR_STEP]; // off, low, high
//  byte velocity[NBR_INST][NBR_STEP]; // Is this really needed ?
  byte extNote[MAX_EXT_INST_NOTE];// DUE to EEPROM 64 bytes PAGE WRITE
  byte extLength;
  byte groupPos;
  byte groupLength;
  byte totalAcc;

public:
  CPattern();
  ~CPattern() {};
  void SetHH(void);
  void Load(byte patternNbr);
  void Save(byte patternNbr);
  void CopyToBuffer(byte patternNbr);
  void PasteFromBuffer(byte patternNbr);
  void ShiftLeft(void);
  void ShiftRight(void);
  void Group(void);

private:
#ifdef DEBUG
  void Show();
  void printBits(size_t const size, void const * const ptr);
#endif
  void InstToStepWord();
  void SetHHPattern();
  void WireBeginTX(unsigned long address)
  {
    byte hardwareAddress;
    if (address > 65535) hardwareAddress = HRDW_ADDRESS_UP;
    else hardwareAddress = HRDW_ADDRESS;
    Wire.beginTransmission(hardwareAddress);
    Wire.write((byte)(address >> 8));
    Wire.write((byte)(address & 0xFF));
  }
};
#endif // _PATTERN_H_ 
