#define START_OF_SYSEX      0xF0
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
#define NAVA_FULL_DMP   0x04


// Request command's 
#define NAVA_BANK_REQ   0x40
#define NAVA_PTRN_REQ   0x41
#define NAVA_TRACK_REQ  0x42
#define NAVA_CONFIG_REQ 0x43
#define NAVA_FULL_REQ   0x44
