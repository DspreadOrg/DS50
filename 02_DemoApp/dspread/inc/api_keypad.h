#ifndef __API_KEYPAD_H
#define __API_KEYPAD_H

#include "api_types.h"


 
#define KEYPAD_NO_KEY        0x00
#define KEYPAD_VOLUME_INC    0x81
#define KEYPAD_VOLUME_DEC    0x82
#define POWER                  	  0x80 
#define KEYPAD_POWER_LONG         0x84  
#define PGUP                      0x2a 
#define PGDOWN                    0x23 
#define ENTER                     0x18  
#define FUNCTION                  0x70  
#define CLEAR                     0x17  
#define CANCEL                    0x1b  
#define PLUS                      0x68  
#define MENU                      0x12 
#define DIGITAL0                  0x30
#define DIGITAL1                  0x31
#define DIGITAL2                  0x32
#define DIGITAL3                  0x33
#define DIGITAL4                  0x34
#define DIGITAL5                  0x35
#define DIGITAL6                  0x36
#define DIGITAL7                  0x37
#define DIGITAL8                  0x38
#define DIGITAL9                  0x39


void OsKeypadSetMode(unsigned char mode);
u8 OsKeypadGetKey(void);

#endif

