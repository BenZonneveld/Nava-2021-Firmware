#ifndef _EEPROM_H_
#define _EEPROM_H_

#define PTRN_SIZE          (unsigned long)(448)
#define PTRN_OFFSET        (unsigned long)(0)
#define PTRN_TRIG_SIZE     (unsigned long)(32)
#define PTRN_TRIG_OFFSET   (PTRN_OFFSET + PTRN_TRIG_SIZE)
#define PTRN_SETUP_SIZE    (unsigned long)(32)
#define PTRN_SETUP_OFFSET  (PTRN_TRIG_OFFSET + PTRN_SETUP_SIZE)
#define PTRN_EXT_SIZE      (unsigned long)(128)
#define PTRN_EXT_OFFSET    (PTRN_SETUP_OFFSET + PTRN_EXT_SIZE)
#define PTRN_VEL_SIZE      (unsigned long)(256)
#define PTRN_VEL_OFFSET    (PTRN_EXT_OFFSET + PTRN_VEL_SIZE)
#define TRACK_SIZE         (unsigned long)(1024)
#define TRACK_OFFSET       (PTRN_SIZE * MAX_PTRN)
#define OFFSET_SETUP       (TRACK_OFFSET + (TRACK_SIZE * MAX_TRACK))
#define SETUP_SIZE         (unsigned long)(64)
#define OFFSET_GROUP       (unsigned long)(37) //save only .groupPos and .groupLength
#define GROUP_SIZE         (unsigned long)(2)
#define MAX_PAGE_SIZE      (unsigned long)(64)
#define HRDW_ADDRESS       0x50
#define HRDW_ADDRESS_UP    (0x50 | B100)
#define DELAY_WR           5

#endif
