#ifndef __API_SEC_SMGT_H
#define __API_SEC_SMGT_H
#include "api_types.h"
 
int OsSecSmgtSetRunningMode(int mode);
int OsSecSmgtGetRunningMode(u8 *dataout, u8 *datalen);
int OsSecSmgtSetRunningStatus(int status, int error_code);
int OsSecSmgtGetRunningStatus(u8 *dataout, u8 *datalen);

int OsSecSmgtReqMutualAuth(u8 mode, u8 *dataout, u8 *datalen);
int OsSecSmgtSendMutualAuth(int datalen, u8 *data);
int OsSecSmgtReqMutalAsyAuth(u8 *datain,u32 len, u8 *dataout, u32 *datalen);
int api_sec_smgtSendMutualAsyAuth(int datalen, u8 *data);


int OsSecInputAdminPasswordOperate(u8 admin,u8 mode,int timeout);
int OsSecGetAdminPasswordStatus(u8 *status);
int OsSecGetPasswordLockTime(int * timeout);
int OsSecGetPasswordValidStatus(u8 *rbuf);
int OsSecExitPasswordInput(void);

int OsSecSmgtConfigTamper(void);
int OsSecSmgtGetTamperStatus(u8 *dataout, int *len);
int OsSecSmgtClearTamper(void);
int OsSecSmgtClearLock(void);

int OsSecSmgtProxyCrtl(int datalen,u8* data);
 
int OsSecSmgtGetAdbStatus(void);


 
int OsSecSmgtDelAdbConfg(void);



int OsSecGetTamperAuthCode(u8* outCodelen, u8* outCodedata);


/**************************************************
 *function: 
 *input: 
 *output:
 *return:
 * 0: Success 
 *-1: The authorization code is invalid and the number of retries has reached the maximum allowed number. 
 *-2: The authorization code is invalid, but the number of retries has not reached the maximum allowed number.
* -3: Authorization was successful but clearing failed.
 *adding:
 *author: xh
 *date:2025/05/21
**************************************************/
int OsSecTamperAuthRun(char codeLen, u8* codedata);
 
#endif
