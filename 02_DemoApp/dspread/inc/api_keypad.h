#ifndef __API_KEYPAD_H
#define __API_KEYPAD_H

#include "api_types.h"


#define ISO_PIN_FORMAT_0  0
#define ISO_PIN_FORMAT_1  1
#define ISO_PIN_FORMAT_2  2
#define ISO_PIN_FORMAT_3  3

#define PINBLOCK_DES_TYPE_DES 0
#define PINBLOCK_DES_TYPE_3DES 1
 
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

//
/*******************************************************************  
Function Purpose: OsGetPinBlockDes  
Input Paramters: 
              	    keyIndex: Key index
					PinFormat: iso pin format 0 - 3
                    alg: 0:DES,1:3DES
              	    pinMinLen: Pin minimun length
              	    pinMaxLen: Pin maximn length
              	    uiTimeoutMs: Second
              	    pCardNo:Card number
                  
                    PininputShow:  mask pin for display  		
Output Paramters:
				pszPinBlockAsc: mask pinblock	
					
Return Value:    0:pin bypass
                 >0:success
                 other :failed
Remark: 
********************************************************************/
int OsGetPinBlockDes(int keyIndex,unsigned char PinFormat,int alg,unsigned char *card_data,int pinMinLen,int pinMaxLen, unsigned int uiTimeoutMs, unsigned char *pszPinBlock,void (*PininputShow(char* maskPinBlock)));
/*******************************************************************  
Function Purpose: OsGetPinBlockAes  
Input Paramters: 
              	    keyIndex: Key index
              	    pinMinLen: Pin minimun length
              	    pinMaxLen: Pin maximn length
              	    uiTimeoutMs: Second
              	    pCardNo:Card number
                  
                    PininputShow:  mask pin for display  		
Output Paramters:
				pszPinBlockAsc: mask pinblock	
					
Return Value:    0:pin bypass
                 >0:success
                 other :failed
Remark: 
				only support iso pin format 4
********************************************************************/
int OsGetPinBlockAes(int keyIndex,unsigned char *card_data,int pinMinLen,int pinMaxLen, unsigned int uiTimeoutMs, unsigned char *pszPinBlock,void (*PininputShow(char* maskPinBlock)));
/*******************************************************************  
/*******************************************************************  
Function Purpose:get online pinblock with dukpt
Input Paramters: 
              	    keyIndex: Key index  only support 0`4
                    PinFormat: iso pin format 0 - 3
                    alg: 0:DES,1:3DES
              	    pinMinLen: Pin minimun length
              	    pinMaxLen: Pin maximn length
              	    uiTimeoutMs: Second
              	    pCardNo:Card number
                  
                    PininputShow:  mask pin for display  		
Output Paramters:
				pszPinBlockAsc: mask pinblock	
				ksn:  asc ksn	
Return Value:    0:pin bypass
                 >0:success
                 other :failed
Remark: 
********************************************************************/
int OsGetPinBlockDukptDes(int keyIndex,unsigned char PinFormat,int alg,unsigned char *card_data,int pinMinLen,int pinMaxLen, unsigned int uiTimeoutMs, unsigned char *pszPinBlock,unsigned char *ksn,void (*PininputShow(char* maskPinBlock)));
/*******************************************************************  
Function Purpose:OsGetPinBlockDukptAes
Input Paramters: 
              	    keyIndex: Key index  only support 0`4
              	    pinMinLen: Pin minimun length
              	    pinMaxLen: Pin maximn length
              	    uiTimeoutMs: Second
              	    pCardNo:Card number
                  
                    PininputShow:  mask pin for display  		
Output Paramters:
				pszPinBlockAsc: mask pinblock	
				ksn:  asc ksn	
Return Value:    0:pin bypass
                 >0:success
                 other :failed
Remark: 
		only support iso pin format 4
********************************************************************/

int OsGetPinBlockDukptAes(int keyIndex,unsigned char *card_data,int pinMinLen,int pinMaxLen, unsigned int uiTimeoutMs, unsigned char *pszPinBlock,unsigned char *ksn,void (*PininputShow(char* maskPinBlock)));

/*******************************************************************  
Function Purpose:get plaintext offline pin block
Input Paramters: 
              	    pinMinLen: Pin minimun length
              	    pinMaxLen: Pin maximn length
              	    uiTimeoutMs: Second
                  
                    PininputShow:  mask pin for display  		
Output Paramters:
				offlinePin: offline pin	
Return Value:    0:pin bypass
                 >0:success
                 other :failed
Remark: 
********************************************************************/
int OsGetOfflinePin(int pinMinLen,int pinMaxLen, unsigned int uiTimeoutMs,unsigned char *offlinePin,void (*PininputShow(char* maskPinBlock)));
#endif

