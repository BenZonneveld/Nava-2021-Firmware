#include "Pattern.h"

enum SeqMode
{
  TRACK_PLAY, TRACK_WRITE, PTRN_PLAY, PTRN_STEP, PTRN_TAP, MUTE
};

//////////////////////////////variable////////////////////////////////
//Dio---------------------------------------------
byte dinSr[5]={
  0};//Store Din value of the five shift register
byte tempDin[5][2];//Used for the bounce
unsigned long debounceTimer;

//Buttons-----------------------------------------
typedef struct Button Button;
struct Button
{
  boolean curState;//1 is ON, 0 is OFF
  boolean prevState;
  boolean pressed;
  boolean justPressed;
  boolean justRelease;
  boolean hold;// 1 is holding
  byte counter;//count number of push on the button
  unsigned long curTime;
};
Button stepBtn[NBR_STEP_BTN];
Button muteStepBtn[NBR_STEP_BTN];
Button playBtn;
Button stopBtn;
Button encBtn;
Button guideBtn;
Button scaleBtn;
Button tapBtn;
Button dirBtn;
Button tempoBtn;
Button shufBtn;
Button enterBtn, numBtn, lastStepBtn, backBtn, fwdBtn, muteBtn, bankBtn, stepsBtn;

boolean clearBtn;
boolean instBtn;
boolean shiftBtn;
boolean trkBtn;
Button ptrnBtn;
boolean doublePush = 0; //flag that CH and CH_LOW button are pressed together
byte instOut[NBR_INST]=  { 
  BD, BD, SD, SD, LT, LT, MT, MT, HT, HT, RM, HC, CH, CH, CRASH, RIDE};
unsigned int readButtonState;

//Led-----------------------------------------------
unsigned int stepLeds;
unsigned int stepLedsHigh;
unsigned int stepLedsLow;
byte menuLed;
unsigned int configLed;
volatile boolean blinkTempo = 1;
volatile boolean blinkFast = 1;
volatile boolean blinkVeryFast;
boolean stopLed;
boolean trackLed;
boolean backLed;
boolean fwdLed;
boolean numLed;
boolean ptrnLed;
boolean tapLed;
boolean dirLed;
boolean guideLed;
boolean bankLed;
boolean muteLed;
boolean tempoLed;
boolean instLed;
boolean shiftLed;
boolean clearLed;
boolean shufLed;
boolean scaleLed;
boolean lastStepLed;
/*boolean eighttLed;//tempo scale Leds
 boolean sixteentLed;
 boolean sixteenLed;
 boolean threetwoLed;*/
byte scaleLeds;
boolean enterLed;
unsigned int instSlctLed;//[NBR_INST]={0x00, 0x00, 0x300, 0x400, 0x800, 
byte flagLedIntensity;
unsigned int muteLedsOrder[NBR_STEP_BTN]=  { 
  0x03, 0x03, 0x0C, 0x0C, 0x30, 0x30, 0xC0, 0xC0, 0x300, 0x300, 0x400, 0X800, 0x1000, 0x2000, 0x4000, 0x8000};
unsigned int muteLeds = 0;
/*unsigned int muteLedsTrig[NBR_STEP_BTN]=  { 
 0, 0, 0x30, 0x400, 0x800, 0x3000, 0x4000, 0x8000, 0x03, 0x0C, 0x30, 0XC0, 0, 0, 0, 0};*/

//Sequencer-----------------------------------------
struct SeqConfig {
  boolean sync;//0 = MASTER  1 = SLAVE
  boolean syncChanged;
  byte TXchannel;//MIDI transmit channel
  byte RXchannel;
  byte EXTchannel; // MIDI transmit channel for external instrument
  unsigned int bpm;
  boolean patternSync;
  unsigned int defaultBpm;// stored in the eeprom
  byte dir;
  byte configPage;
  byte runMode;
  boolean configMode;
  boolean setupNeedSaved;
}
seq;

volatile byte ppqn = 0;
volatile byte curStep = 0;
volatile int stepCount = -1;
volatile byte tapStepCount;//this counter is used to get a better tap response
volatile boolean stepChanged = FALSE;
volatile byte noteIndexCpt = 0;
boolean isRunning = FALSE;
boolean isStop = TRUE;
SeqMode curSeqMode = TRACK_PLAY;
SeqMode prevSeqMode;
byte curInst = BD;//8 is BD trig out shift register
byte curBank = 0;//0 to 7 banks
byte curPattern = 0;// 0 to 255
byte nextPattern;
//byte seqDirMode;
boolean changeDir; //use to PING PONG change dir
volatile boolean endMeasure;
/*byte seqDir[MAX_SEQ_DIR][NBR_STEP]={//To do
 {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}
 {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0}
 {0, 2, 1, 3, 2, 4, 3, 5, 4, 6, 5, 7, 6, 8, 7, 9, 8, 10, 9, 11, 10, 12, 11, 13, 12, 14, 13, 15, 14}*/
unsigned int metronome;//value of metronome trig
int shuffle[MAX_SHUF_TYPE][2]={
  {
    0          }
  ,{
    0,-1        }
  ,{
    0,-2        }
  ,{
    0,-3        }
  ,{
    0,-4        }
  ,{
    0,-5        }
  ,{
    0,-6        }
};

volatile boolean shufPolarity;

//Pattern-------------------------------------------
typedef struct Pattern Pattern;
struct Pattern
{
  byte length;//0 to 15 steps
  byte scale;
  byte dir;//0=>forward, 1=>backword, 2=>ping-pong, 3=>random
  byte shuffle;
  byte flam; // 10, 14, 18, 22,26,30,34,38 ms, not affected by tempo
  unsigned int inst[NBR_INST];
  unsigned int step[NBR_STEP];
  byte velocity[NBR_INST][NBR_STEP]; // Is this really needed ?
  byte extNote[128];// DUE to EEPROM 64 bytes PAGE WRITE
  byte extLength;
  byte groupPos;
  byte groupLength;
  byte totalAcc;
};

Pattern pattern[2];//current pattern and next pattern in the buffer
Pattern bufferedPattern;//to copy paste pattern
boolean ptrnBuffer = 0;
boolean patternWasEdited = FALSE;
boolean selectedPatternChanged = FALSE;
boolean nextPatternReady = FALSE;
boolean patternNeedSaved = FALSE;
//volatile boolean patternNeedSwitchBuffer = FALSE;

struct GroupPattern
{
  byte length;//length of the pattern bloc
  byte firstPattern;//first pattern of the bloc
  byte pos;
  boolean priority;//use to know if volatile group have priority on stored pattern group
}
group;

unsigned int tempInst[NBR_INST]={
  0};
unsigned int muteInst = 0;
byte muteOut[NBR_STEP_BTN]=  { 
  BD, BD, SD, SD, LT, LT, MT, MT, HT, HT, RM, HC, CH, OH, CRASH, RIDE};
volatile unsigned int tempDoutTrig;// used to know what OH or CH is trigged
//Velocity table match with original TR909
byte instVelHigh[NBR_INST]={
  0, 0, 90, 17, 37, 108, 112, 107, 50, 54, 85, 40, 1, 0, 111, 109};
byte instVelLow[NBR_INST]={
  0, 0, 20, 6, 11, 95, 111, 106, 30, 15, 33, 12, 0, 0, 95, 108};

//Track----------------------------------------------
typedef struct Track Track;
struct Track
{
  unsigned int length;//lenght before loop 0 to 999
  byte patternNbr[MAX_PTRN_TRACK];//Should be 999 
};
Track track[2];

typedef struct TrackSetup TrackSetup;
struct TrackSetup
{
  byte next;
  byte current;//number of the track 0 to 15
  unsigned int pos;//track position 0 to 999
  volatile byte indexCpt;//incrementer each pattern end
  boolean wasEdited;
};
TrackSetup trk;
boolean selectedTrackChanged = FALSE;
boolean trkBuffer = 0;
boolean trackNeedSaved = FALSE;
boolean trackJustSaved = FALSE;
volatile boolean trackPosNeedIncremante = FALSE;
unsigned long timeSinceSaved;

//Ext inst-------------------------------------------
boolean keyboardMode;
byte keybOct = DEFAULT_OCT;
byte noteIndex = 0;//external inst note index

//Encoder--------------------------------------------
byte encoder_A;
byte encoder_B;
byte encoder_A_prev=0;
boolean encSwValue[2];
boolean encSwState;

//Mux------------------------------------------------
byte muxAddr[5]={
  B0, B100, B10, B110, B1};// Due to the  A, B, C pin on PORTA sequence
byte muxInst[10]={
  LT, SD, BD, MT, HT, HC, RM, CH, CRASH, RIDE};//To match with DAC mux OUT

//LCD------------------------------------------------
boolean needLcdUpdate = TRUE;
byte curIndex;//position of the cursor
byte cursorPos[MAX_CUR_POS]={
  0, 4, 8, 12};
const char *letterUpTrackWrite[MAX_CUR_POS]={
  "P", "P", "L", "N"};
const char *letterUpExtInst[MAX_CUR_POS]={
  "I", "N", "L", "O"};
const char *letterUpConf[MAX_CONF_PAGE][MAX_CUR_POS]={{
  "S", "B", "M", "M"},{
  "P", " ", "E", "M"}};

//MIDI-----------------------------------------------
volatile boolean midiNoteOnActive = FALSE;
boolean midiStart;
boolean midiStop;
boolean midiContinue;
boolean instWasMidiTrigged[NBR_INST] ={
  FALSE};
byte midiVelocity[NBR_INST]={
  100};

//Din synchro----------------------------------------
boolean dinStartState = LOW;
boolean dinClkState = LOW;

//Bootloader-----------------------------------------
unsigned long bootElapseTime = 0;
byte btnPlayStopByte = 0;
byte btnEnterByte = 0;
