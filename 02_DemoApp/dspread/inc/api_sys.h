#ifndef _API_SYS_H
#define _API_SYS_H
#include "api_types.h"


#define API_THREADCREATE_ERROR -0x1001
#define API_GETTHREADID_ERROR -0x1002
#define API_MEMORY_ALLOC_ERROR -0x1003
#define API_MEMORY_FREE_ERROR -0x1004
#define API_MEMORY_REALLOC_ERROR -0x1005
#define API_REBOOT_ERROR -0x1006
#define API_POWEROFF_ERROR -0x1007
#define API_SEMCREATE_ERROR -0x1008
#define API_MUTEXCREATE_ERROR -0x1009
#define API_MUTEXLOCK_ERROR -0x1010
#define API_CREATEQUEUE_ERROR -0x1011
#define API_QUEUEPUT_ERROR -0x1012
#define API_GETHEAPINFO_ERROR -0x1013
#define API_QUEUEGET_ERROR -0x1014
#define API_GENERATERANDOM_ERROR -0x1015
#define API_SETRTC_ERROR -0x1016
#define API_GETRTC_ERROR -0x1017
#define API_GETSN_ERROR -0x1018
#define API_GETFIRMVERSION_ERROR -0x1019
#define API_GETHW_ERROR -0x1020
#define API_UPDATELOGO_ERROR -0x1021

typedef enum enumBootCause
{
	API_BOOTCAUSE_SOFTRESET,
	API_BOOTCAUSE_HARDTRESET,
	API_BOOTCAUSE_PWRKEY,
	API_BOOTCAUSE_USB,
	API_BOOTCAUSE_WATCHDOG
}API_BOOT_CAUSE;

typedef struct
{
	u8 sec;   ///< Second
	u8 min;   ///< Minute
	u8 hour;  ///< Hour
	u8 day;   ///< Day
	u8 month; ///< Month
	u16 year;  ///< Year
	u8 wDay;  ///< Week Day
} rtc_time_t;

typedef enum apiThreadPriority
{
	API_PRIORITY_IDLE = 1, // reserved
	API_PRIORITY_LOW = 8,
	API_PRIORITY_BELOW_NORMAL = 16,
	API_PRIORITY_NORMAL = 24,
} apiThreadPriority_t;

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
#define SP_VER          0x11
#define COMPILE_DATE    0xA0
#define COMPILE_TIMER   0xA1

#define AP_BOOT_VER  	0x00
#define AP_CORE_VER  	0x01
#define AP_VSM_VER  	0x02
#define AP_RECOVER_VER  0X03

#define SP_BOOT_VER 	0x10
#define SP_CORE_VER 	0x11
#define SP_SECLIB_VER 	0x12


#define VER_STR_MAX_LEN 20
#define MACHTYPE_DS50   0xA1


#define SYS_CMD_BAT_DETECT_ENABLE           0x01    
#define SYS_CMD_BAT_DETECT_DISENABLE        0x02
#define SYS_CMD_GET_BAT_DETECT_STAT         0x03 

#define SYS_CMD_POWEROFF_SCREEN    		    0x04
#define SYS_CMD_CHANGE_SCREEN_BUFFER    	0x05
#define SYS_CMD_EXIT_POWEROFF_SCREEN		0x51

#define SYS_CMD_GET_SPIFLASH_INFO           0x06
#define SYS_CMD_READ_SPIFLASH               0x07
#define SYS_CMD_WRITE_SPIFLASH              0x08
#define SYS_CMD_GET_SPIFLASH_ID             0x16

#define SYS_CMD_SET_AUTO_SLEEP_TIME         0x09  
#define SYS_CMD_SET_AUTO_SLEEP_AVAIBLE      0x0a  
#define SYS_CMD_SET_AUTO_SLEEP_WAKE_TIME    0x0b  
#define SYS_CMD_SET_AUTO_SLEEP_WAKE_AVAIBLE 0x0c  
#define SYS_CMD_GET_DEEPSLEEP_WAKE_REASON   0x0d 
#define SYS_CMD_SET_NET_AUTO_SLEEP_AVAIBLE   0x0e 

#define SYS_CMD_SET_OTHER_KEY_WKUP          0x0f  


#define SYS_CMD_SET_SYS_DEFAULT_NET_DEVEICE    0x20  
#define SYS_CMD_GET_SYS_DEFAULT_NET_DEVEICE    0x21  
#define SYS_CMD_GET_CUSTOMER_INFO     		   0x25 
#define SYS_CMD_READ_CUSTOMERID                0x26


#define SYS_CMD_APP_HOOK_FUNC               0x30


#define SYS_CMD_GET_BAT_PERCENT             0x40
#define SYS_CMD_GET_POWERON_MODE            0x41
#define SYS_CMD_SCAN_LED                    0x42


#define SYS_CMD_GET_BAT_ERRNO               0x100

#define SYS_CMD_OS_DEBUG_ON                 0x80
#define SYS_CMD_OS_DEBUG_OFF                0x81
#define SYS_CMD_SET_WAKE_TIME               0x82

#define FLASH_CHECH_ADDR		0x66F000  


typedef struct __hwc_list
{
	u32 crc;
	u8 ProjectItem[32];
	u8 HWboomVer[32];
	u8 wireless;
	u8 bluetooth;
	u8 modem;
	u8 net;
	u8 wifi;
	u8 gps;
	u8 scanner;
	u8 camera;
	u8 fingerprint;
	u8 icc;
	u8 nfc;
	u8 mag;
	u8 idcard;
	u8 touchscreen;
	u8 sdc;
	u8 printer;
	u8 audio;
	u8 nes;
	u8 psam;
	u8 beep;
	u8 esim;
	u8 flash;
	u8 cameraled;
	u8 LSI;
	u8 LCD;
	u8 SEG;
	u8 res1;
	u8 res2;
	u8 res3;
}hwc_list_t;



s32 OsSysMsleep(u32 nMs);

u32 OsSysTick();


s32 OsSysThreadCreate(void* thread_fun, S8* thread_name, u32 thread_stack_size, void* params, u32 uxPriority, u32* pThreadId);

void OsSysThreadDelete(u32 uThread);

void OsSysThreadSuspend(u32 uThread);

void OsSysThreadResume(u32 uThread);

u32 OsSysThreadId(void);

void* OsSysMalloc(u32 size);

s32 OsSysFree(void* buffer);

void* OsSysRealloc(void* buffer, unsigned int size);

s32 OsSysReboot();

s32 OsSysPowerOff();


u32 OsSysSemNew(int value);

void OsSysSemFree(int sem_id);

void OsSysSemWait(int sem_id);

bool OsSysSemWaitTimeout(u32 sem_id, u32 timeout);

void OsSysSemSignal(u32 sem_id);

u32 OsSysMutexCreate();

void OsSysMutexLock(u32 mutex_id);

s32 OsSysMutexLockTimeout(u32 mutex_id, u32 timeout);

void OsSysMutexUnlock(u32 mutex_id);

void OsSysMutexDelete(u32 mutex_id);

s32 OsSysGetHeapInfo(u32* memsize, u32* avail_size, u32* avail_block_size);


u32 OsSysQueueCreate(u32 count, u32 itemsize);

s32 OsSysQueuePut(s32 queue_id, void* item, u32 timeout);

s32 OsSysQueuePutFront(s32 queue_id, void* item, u32 timeout);

s32 OsSysQueueGet(s32 queue_id, void* item, u32 timeout);

void OsSysQueueDelete(s32 queue_id);

u32 OsSysQueueSpaceAvail(u32 queue_id);

void OsSysQueueClear(u32 queue_id);


s32 OsSysGenerateRandom(void* buf, u32 len);


s32 OsSysSetSleepMode(u8 time);

bool OsSysGetSleepState();




int OsSysGetSysVersion(char* version);


s32 OsSysSetRtcTime(S8* pTimestr);

s32 OsSysGetRtcTime(S8* timeStr);

s32 OsSysSetSpTime(u8* lpTime);

s32 OsSysGetSpTime(u8* lpTime);

s32 OsSysReadSn(u8* tsn, u32* tsnlen, u8* csn, u32* csnlen);

s32 OsSysGetFirmVersion(u8* lpOut, u32 nType);

s32 OsSysRebootSp(void);

s32 OsSysGetHwc(hwc_list_t* hwc_list);

s32 OsSysSpConrolCmd(u32 nCmd, u32 lParam, u32 wParam);

s32 OsSysEnterDownloadMode();
int OsSysGetBootCause(void);
void* OsSysInit(u8* ver);
s32 OsGetSleepState(void);
s32 OsSpWakeUpAp(void);
s32 OsApWakeUpSp(void);
s32 OsSleepCmd(u32 nCmd, u32 lParam, u32 wParam);


s32 OsSysWatchDogDisable();

void OsSysAppLock();
void OsSysAppUnLock();
int OsSysGetAppLock();
int OsSysUpdateLogo(char* filename);
void api_sys_setTmsHandle(int (*func)());
void OsSysSetSecHandle(void (*func)(int, char*));
#endif