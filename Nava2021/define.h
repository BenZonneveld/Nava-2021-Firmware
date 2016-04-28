//-------------------------------------------------
//                  NAVA v1.x
//                    define
//-------------------------------------------------

#ifndef define_h
#define define_h
#pragma once

//DEBUG
#define DEBUG 1

//MIDI
#define NOTE_ON_CMD 0x90
#define NOTE_OFF_CMD 0x80
#define CLOCK_CMD 0xF8
#define START_CMD 0xFA
#define STOP_CMD 0xFC
#define ALL_NOTE_OFF 0x7B
#define CONTINU_CMD 0xFB
#define MIDI_BAUD 31250
#define MIDI_ACCENT_VELOCITY 127
#define MIDI_HIGH_VELOCITY (MIDI_ACCENT_VELOCITY-16) 
#define MIDI_LOW_VELOCITY (MIDI_HIGH_VELOCITY-16)

//Button
#define BTN_PLAY     B10000000
#define BTN_STOP     B1
#define BTN_LASTSTEP B1000000
#define BTN_SCALE    B10
#define BTN_SHUF     B100000
#define BTN_CLEAR    B100
#define BTN_INST     B10000
#define BTN_SHIFT    B1000
#define BTN_TRK  B1000
#define BTN_BACK     B100
#define BTN_FWD      B10
#define BTN_NUM      B1
#define BTN_PTRN B10000000
#define BTN_TAP      B1000000
#define BTN_DIR      B100000
#define BTN_GUIDE    B10000
#define BTN_BANK     B100 /* B1 */
#define BTN_MUTE     B1 /* B10 */
#define BTN_TEMPO    B10 /* B100 */
#define BTN_ENTER    B1000
#define BTN_ENCODER  B100
#define ENC_SW_GET   PINB & BTN_ENCODER
#define JUSTPRESSED 1
#define JUSTRELEASE 0
#define HOLD_TIME 500 //1s to trig hold 
#define NBR_BTN_STEP 16

//Led
#define LED_STOP  B1
/*#define LED_SHIFT B10
 #define LED_INST  B100
 #define LED_CLR   B1000
 #define LED_SHUF  B10000
 #define LED_LAST  B100000
 #define LED_SCALE B1000000*/
#define LED_PLAY  B10000000
#define LED_MASK  0xDD55 //mask to fade selected inst led
#define LED_MASK_OH  0xFD55 

//LCD
#define MAX_CUR_POS 4
#define MAX_CONF_PAGE 2

//Utility
#define TOGGLE 0
#define MOMENTARY 1
#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0
#define BOOTLOADER_TIME 5000 // time staying in bootloader 

//Sequencer
#define NBR_INST 16
#define NBR_STEP_BTN 16
#define NBR_STEP 16
#define NBR_PATTERN 16 //Number of pattern in a bank
#define NBR_BANK 8
#define MID_VEL 40 //velocity 0 to 127
#define HIGH_VEL 80
#define MAX_VEL 127 
#define MAX_SEQ_DIR 3
#define FORWARD 0
#define BACKWARD 1
#define PING_PONG 2
#define RANDOM 3
#define MAX_BANK 7 //bank A to H
#define MAX_PTRN 128
#define MAX_TRACK 16
#define MAX_PTRN_TRACK 1024
#define MIN_BPM 30
#define MAX_BPM 250
#define DEFAULT_BPM 120
#define PPQN 96
#define SCALE_16 PPQN/4
#define SCALE_32 PPQN/8
#define SCALE_16t PPQN/6
#define SCALE_8t PPQN/3
#define MAX_SHUF_TYPE 7
#define DEFAULT_SHUF 1
#define DEFAULT_LEN 16
#define DEFAULT_SCALE 24
#define MASTER 0
#define SLAVE 1
#define MAX_BLOCK_LEN 16
#define POSITION 0
#define LENGTH 1
#define MAX_TOTAL_ACC 13
#define MIN_TOTAL_ACC 0

//Ext inst
#define MAX_OCT 8
#define DEFAULT_OCT 3 //corresponding to +0
#define MAX_EXT_INST_NOTE 99


//trig out  and dinsynchro
#define TRIG_HIGH PORTA |= 1 << 2
#define TRIG_LOW  PORTA &= ~(1 << 2)
#define TRIG1_PIN 26
#define TRIG2_PIN 27
#define DIN_START_PIN 24
#define DIN_CLK_PIN 25

//Dincsync out
#define DIN_START_HIGH PORTA |= 1
#define DIN_START_LOW  PORTA &= ~(1)
#define DIN_CLK_HIGH   PORTA |= 1 << 1
#define DIN_CLK_LOW    PORTA &= ~(1 << 1)

//Dio
#define SW_CS_LOW PORTD&=~(1<<4)
#define SW_CS_HIGH PORTD|=(1<<4) 
#define LED_CS_LOW   PORTB&=~(1<<4)
#define LED_CS_HIGH  PORTB|=(1<<4)
#define TRIG_CS_LOW   PORTB&=~(1<<3)
#define TRIG_CS_HIGH  PORTB|=(1<<3)
#define DAC_CS_HIGH  PORTD|=(1<<5)
#define DAC_CS_LOW  PORTD &=~(1<<5)

#define DEBOUNCE_TIME 5

//Mux
#define MUX_ADDR_PORT PORTA
#define NBR_MUX_OUT 5
#define NBR_MUX 2
#define MUX_INH_PORT PORTD 
#define MUX_INH1_BIT 6
#define MUX_INH2_BIT 7
#define MUX_INH1_HIGH PORTD|=(1<<6)
#define MUX_INH1_LOW PORTD &=~(1<<6)
#define MUX_INH2_HIGH PORTD|=(1<<7)
#define MUX_INH2_LOW PORTD &=~(1<<7)
#define MUX_ADDR_0 PORTA &=~(1<<7)&~(1<<6)&~(1<<5)
#define MUX_ADDR_1 PORTA |=(1<<7)&~(1<<6)&~(1<<5)

//Inst button
#define BD_BTN 0
#define BD_LOW_BTN 1
#define SD_BTN 2
#define SD_LOW_BTN 3
#define LT_BTN 4
#define LT_LOW_BTN 5
#define MT_BTN 6
#define MT_LOW_BTN 7
#define HT_BTN 8
#define HT_LOW_BTN 9
#define RM_BTN 10
#define HC_BTN 11
#define CH_BTN 12
#define CH_LOW_BTN 13
#define CRASH_BTN 14
#define RIDE_BTN 15
#define OH_BTN 0x3000 //bit 13 and bit 12 of a 16bits word

//Inst Match with bit shift register out (cf schematic)
#define BD 8
#define SD 9
#define LT 10
#define MT 11
#define HT 2
#define RM 3
#define HC 4
#define HH 5
#define CRASH 7
#define RIDE 6
#define HH_SLCT 1
#define OH 15 //unused shift OUT
#define CH 14 //unused shift OUT
#define TRIG_OUT 0
#define TOTAL_ACC 12
#define EXT_INST 13

#endif//end if define_h
