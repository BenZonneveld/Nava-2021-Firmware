#include <Arduino.h>
#include "src\WireN\WireN.h"

#include "define.h"
#include "Pattern.h"

CPattern::CPattern()
{
  length = 15; // steps 0 - 15
  scale = 0; // 16th notes
  shuffle = 0;
  flam = 0;
  extLength = 0;
  groupPos = 0;
  groupLength = 0;
  totalAcc = 0;
  for (int i = 0; i < NBR_INST; i++)
  {
    inst[i] = 0;
    for (int j = 0; j < NBR_STEP; j++)
    {
      Velocity[i][j] = Velo::off;
    }
  }
  for (int i = 0; i < NBR_STEP; i++)
  {
    step[i] = 0;
  }
  for (int i = 0; i < MAX_EXT_INST_NOTE; i++ )
  {
     extNote[i] = 0;
  }
}

void CPattern::SetHH(void)
{
  
}

void CPattern::printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char by;
    int i, j;
    
    for (i = 0; i < size; i++) {
        for (j = 0; j <= 7; j++) {
            by = (b[i] >> j) & 1;
            Serial.print(by);
        }
    }
    Serial.println();
}

void CPattern::Show()
{
  char instName[4];
  Serial.println("inst[16]");
  for(int i=0; i < NBR_INST; i++ )
  {
//    strcpy_P(instName, (char*)pgm_read_word(&(selectInstString[i])));
    Serial.print(i);
    Serial.print(" = ");
    printBits(sizeof(inst[i]),&inst[i]);
  }

  Serial.println("step[i]");
  for(int i=0; i <= length; i++ )
  {
    Serial.print(i+1);
    Serial.print(" = ");
    printBits(sizeof(step[i]), &step[i]);
  }
//    for (byte i = 0; i < NBR_INST; i++){
//      char hex[4];
//      sprintf(hex,"%02X ",[i + 4*nbrPage][j]);
//      Serial.print(hex);
//    }
    Serial.println();
}

void CPattern::Load(byte patternNbr)
{
  Serial.println("CPattern::Load()");
  unsigned long adress = (unsigned long)(PTRN_OFFSET + patternNbr * PTRN_SIZE);
  WireBeginTX(adress); 
  Wire.endTransmission();
  Wire.requestFrom(HRDW_ADDRESS,MAX_PAGE_SIZE); //request a 64 bytes page
  //TRIG-----------------------------------------------
  for(int i =0; i<NBR_INST;i++){
    inst[i] = (unsigned long)((Wire.read() & 0xFF) | (( Wire.read() << 8) & 0xFF00));
    // Serial.println(Wire.read());
  }
  //SETUP-----------------------------------------------
  length = Wire.read();
  scale = Wire.read();
  shuffle = Wire.read();
  flam = Wire.read();
  extLength = Wire.read();
  groupPos = Wire.read();
  groupLength = Wire.read();
  totalAcc = Wire.read();
  
  for(int a = 0; a < 24; a++){
    Wire.read();
  }
  //EXT INST-----------------------------------------------
  for(int nbrPage = 0; nbrPage < 2; nbrPage++){
    adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_SETUP_OFFSET);
    WireBeginTX(adress);
    Wire.endTransmission();
    Wire.requestFrom(HRDW_ADDRESS,MAX_PAGE_SIZE); //request of  64 bytes

    for (byte j = 0; j < MAX_PAGE_SIZE; j++){
      extNote[j + (MAX_PAGE_SIZE * nbrPage) ] = Wire.read();
    }
  }
  Serial.println();
  InstToStepWord();
  Show();
  //VELOCITY-----------------------------------------------
/*  for(int nbrPage = 0; nbrPage < 4; nbrPage++){
    adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_EXT_OFFSET);
    WireBeginTX(adress);
    Wire.endTransmission();
    Wire.requestFrom(HRDW_ADDRESS,MAX_PAGE_SIZE); //request of  64 bytes
    for (byte i = 0; i < 4; i++){//loop as many instrument for a page
      char instName[4];
      strcpy_P(instName, (char*)pgm_read_word(&(selectInstString[i + 4*nbrPage])));
      Serial.print(instName);
      Serial.print(" = ");
      for (byte j = 0; j < NBR_STEP; j++){
        pattern[!ptrnBuffer].velocity[i + 4*nbrPage][j] = (Wire.read() & 0xFF);
        char hex[4];
        sprintf(hex,"%02X ",pattern[!ptrnBuffer].velocity[i + 4*nbrPage][j]);
        Serial.print(hex);
      }
      Serial.println();
    }
  }*/  
}

void CPattern::Save(byte patternNbr)
{
  
}

void CPatternCopyToBuffer(byte patternNbr)
{
  
}

void CPattern::PasteFromBuffer(byte patternNbr)
{
  
}

void CPattern::ShiftLeft(void)
{
  
}

void CPattern::ShiftRight(void)
{
  
}

void CPattern::Group(void)
{
  
}

void CPattern::InstToStepWord()
{
  for (int a = 0; a < NBR_STEP; a++){
    step[a] = 0;
    for (int b = 0; b < NBR_INST; b++){
      if (bitRead(inst[b],a)) bitSet(step[a],b);
    }
  }
}

//Combine OH and CH pattern to trig HH
void CPattern::SetHHPattern()
{
  inst[HH] = inst[CH] | inst[OH];
  for (int a = 0; a < NBR_STEP; a++){
    if (bitRead(inst[CH],a)) bitClear(inst[OH],a);
    if (bitRead(inst[OH],a)){
      bitClear(inst[CH],a);
      Velocity[CH][a]=Velo::high;
//      velocity[CH][a] = instVelHigh[HH];
    }
  }
}
