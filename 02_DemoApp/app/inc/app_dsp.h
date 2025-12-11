#ifndef __APP_DSP_H__
#define __APP_DSP_H__
#include <stdbool.h>

#include "app_card_payment.h"
#include "dsp_emv.h"

#define DSP_FILENAME "/ext/trans_info.bin"
extern char gShopId[24];
extern char gStaffId[24];
extern int gTraceNo ;
extern int gBatchNo ;
extern char gQrUrl[256];
extern char gMchntcnName[32];


typedef struct {
	char traceNo[8];//serial number
	char batchNo[8];//batch number
	char transStatus[3];
	char transTypeId[8];
	char origOutTradeId[28];
	char transDatetime[28];
	char money[12];
	char cardNo[24];
	//char mchntcd[24];
	//char mchntcdDeviceid[24];
}dsp_tradeInfo_t;


int app_dsp_getInitFlag(void);
int app_dsp_login(char* mchntCd, char* deviceId, char* url, char* shopIdOut, char* staffIdOut,char* errorMsg);
int app_dsp_signin(char* mchntCd, char* deviceId, char* url);

int app_dsp_qrcodePay(
	char* mchntCd, char* deviceId,
	char* url, char* authCode,
	char* amount,char* uniqueOrderIdentifier,
	char* msgOut);


int app_dsp_getQrcode(
	char* mchntCd, char* deviceId,
	char* url, char* amount,
	char* qecodeUrlOut, char* outTradeNoOut,char* uniqueOrderIdentifier,
	char* msgOut);


int app_dsp_checkPay(
	char* mchntCd, char* deviceId,
	char* url, char* tradeNo, int* transState,
	char* msgOut);



int app_dsp_qrcodePayRefund(
	char* mchntCd, char* deviceId,
	char* url, char* amount,
	char* traceNo, char* batchNo,
	char* outTradeId, int* transState,
	char* msgOut);

int app_dsp_transactionFlow(
	char* mchntCd, char* deviceId,
	char* url, int pageNo,
	int pageSize, int* total, dsp_tradeInfo_t* infos,
	char* msgOut);

int app_dsp_settlement(
	char* mchntCd, char* deviceId,
	char* url, char* amount,
	char* traceNo, char* batchNo,
	char* outTradeId, int* transState,
	char* msgOut);

int app_dsp_getStaticQrcode(
	char* mchntCd, char* deviceId,
	char* url, char* qrUrl, char* mchntcnNameStr,
	char* msgOut);


int app_dsp_cardPay(
	char* url, APPEMVINFO *emvInfo,EmvOnlineData_t* pOnlineData);

int app_dsp_cardPayRefund(
	char* mchntCd, char* deviceId,
	char* url, char* amount, char* cardNo,
	char* deviceSN, char* traceNo, char* batchNo,
	char* origOutTradeNo,char* msgOut);



int app_dsp_terminalInit(char* url,
	char* outdataKey, char* outpinKey,
	char* outmerchantId, char* outtid,
	char* outmerchantName, char* msgOut);

int app_dsp_terminalStorage(char* url, char* msgOut);
int app_dsp_qrcodePayClose(char * mchntCd,char *deviceId,char* url,char * outTradeNo,char* msgOut);

#endif
