#pragma once

#define START_OF_SYSEX      (char)0xF0
#define END_OF_SYSEX        0xF7

#define SYSEX_MANUFACTURER  0x7d
// The next two give us "909" bitshifted to the left to fit into 7 bits of MIDI data.

#define SYSEX_DEVID_1 0x07
#define SYSEX_DEVID_2 0x1A

#define HEADERSIZE      6
// Sysex Commands:
#define NAVA_BANK_DMP   0x00
#define NAVA_PTRN_DMP   0x01
#define NAVA_TRACK_DMP  0x02
#define NAVA_CONFIG_DMP 0x03
#define NAVA_LEVELS_DMP 0x04
#define NAVA_FULL_DMP   0x05

// Request command's 
#define NAVA_BANK_REQ   0x40
#define NAVA_PTRN_REQ   0x41
#define NAVA_TRACK_REQ  0x42
#define NAVA_CONFIG_REQ 0x43
#define NAVA_LEVELS_REQ 0x44
#define NAVA_FULL_REQ   0x45
#define NAVA_FBANK_REQ  0x46
#define NAVA_FTRACK_REQ 0x47
#define NAVA_ACK        0x48
#define BANK_PARTS      8  // Must be one of 4,8 or 16.

#define SYSEX_PTRN_SIZE   564
#define SYSEX_BANK_SIZE   1076 // 1076 //2100
#define SYSEX_TRACK_SIZE  1186
#define SYSEX_CONFIG_SIZE 89
#define SYSEX_BUFFER_SIZE SYSEX_TRACK_SIZE // 2100 when using 4 BANK_PARTS
uint8_t *SysEx;
      
/*struct SysexPortSettings
{
    static const bool UseRunningStatus = true;
    static const bool HandleNullVelocityNoteOnAsNoteOff = true;
    static const bool Use1ByteParsing = true;
    static const unsigned SysExMaxSize = 128;
    static const bool UseSenderActiveSensing = false;
    static const bool UseReceiverActiveSensing = false;
    static const uint16_t SenderActiveSensingPeriodicity = 0;
};*/
