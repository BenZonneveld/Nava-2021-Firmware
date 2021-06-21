#define SYSEX_MANUFACTURER  0x7d
// The next two give us "909" bitshifted to the left to fit into 7 bits of MIDI data.

#define SYSEX_DEVID_1 0x07
#define SYSEX_DEVID_2 0x1A

// Sysex Commands:
#define NAVA_BANK_REQ   0x00
#define NAVA_BANK_DMP   0x01
#define NAVA_TRACK_REQ  0x02
#define NAVA_TRACK_DMP  0x03
#define NAVA_CONFIG_REQ 0x04
#define NAVA_CONFIG_DMP 0x05
#define NAVA_FULL_REQ   0x06
#define NAVA_FULL_DMP   0x07
