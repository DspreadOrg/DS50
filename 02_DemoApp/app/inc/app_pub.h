#ifndef __APP_PUB_H__
#define __APP_PUB_H__


#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "api_mqtt.h"
#include "cJSON.h"
#include "SDK.h"

#define BOOT_VER  		0x00
#define CORE_VER  		0x01
#define VSM_VER  		0x02
#define APP_VER  		0x03
#define ROM_VER  		0x04
#define VIVA_VER  		0x05
#define TERMAINAL_VER  	0x06
#define LIB_VER  	    0x07
#define MODEL_CODE      0x08
#define OS_VER          0x09
#define SP_BOOT_VER     0x10
#define SP_CORE_VER 	0x11
#define SP_SECLIB_VER   0x12

#define TRANSRECORD_FILE     "/ext/trans/transrecord"              //Transaction Record File


#define INDEX_SIZE sizeof(int)  //Index size to use for the beginning of the file
#define MAX_RECORDS 10          //Maximum number of file records
#define MAX_AMOUNT_LENGTH  8    //Maximum length of amount
#define MAX_FILES 256     


// Defines the RSSI signal strength threshold
#define RSSI_BEST_THRESHOLD  20
#define RSSI_GOOD_THRESHOLD  10
#define RSSI_WEAK_THRESHOLD  0
#define RSSI_INVALID         99


typedef struct{ 
    u8 audioplay[512];          // Voice Playlis
    u8 transRtcTime[64+1];      //The time the device was created
	u8 transTrxTime[64+1];      //The time the server returns
	u8 transdate[64+1];  	    // transaction year
	u8 transtime[64+1];  	    // transaction hour
	u8 custvpa[64+1];
	u8 rrn[64+1];
	u8 stanid[64+1];
	u8 transSeqNo[32+1];		//QR code platform transaction serial number
	u8 amount[12+1];            // Amount of the transaction
	u8 language[12+1];          //Language
	u8 channelId[2+1];		    // Trading channels 
	u8 cTransStatus;	        //1-Normal, 2-Cancelled
}TRANSINFO;

typedef enum {
    LANGUAGE_ENGLISH,
    LANGUAGE_HINDI,
    LANGUAGE_KANNADA,
    LANGUAGE_TAMIL,
    LANGUAGE_MARATHI,
    LANGUAGE_COUNT 
} Language;

Language currentLanguage;

typedef struct {
    bool decimal_point;
    int  count; 
	int  fractional_digits;
} DECIMALPOINT;

extern DECIMALPOINT point;
extern u32 mutex_lvgl;
extern char serialID[64]; // devise serial number
extern bool inputpinpassward;
extern bool lcdType;
extern u32 hibernate_startTick;

int app_write_record(TRANSINFO* new_record);

//Read all records
int app_read_all_records(TRANSINFO* records);
//Amount formatting
void app_amount_formatting(char* src, char* dst);
void app_pan_formatting(char* src, char* dst);
//Get signal strength
int app_wireless_get_csq(char* signalStatus);
//Convert string to hexadecimal
void app_dataToHexString(const char* input, int size, char* output);
size_t app_hex_to_ascii(const char* hex_str, char* ascii_str);
void app_generate_dashes(int count, char* buf);
void app_seg_showMoney(char* str);
int app_reviseAmount(char* oldAmount, char* newAmount);
void convertToWords(int num, char* result); 
int app_money_play(char* amount); 
unsigned char app_getkey(void);

int pub_GetNetWorkStatus();
int Pub_DateConvert(const char* dateStr, 
                   const char* targetFormat, 
                   char* outStr) ;
int Pub_AscToBcd(const char* src, char* dest,int asclen);
int Pub_BcdToAsc(const unsigned char *pBcd, char *pAsc, unsigned int nBcdLen);

int App_IccCardOpen(int nSlotIndex);
int App_ICCardPresent(int nSlotIndex);
int App_ICCardClose(int nSlotIndex);

int App_PiccCardOpen();
int App_PiccCardPresent();
int App_PiccCardClose();

void rf_test();
#endif
