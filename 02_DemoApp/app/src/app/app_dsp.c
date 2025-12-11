#include "app_dsp.h"
#include "app_lvgl.h"
#include "app_pub.h"
#include "cJSON.h"
#include "app_card_payment.h"
#include "dsp_emv.h"

char gShopId[24] = { 0 };
char gStaffId[24] = { 0 };
char gQrUrl[256] = { "https://dspread.com/payPageKj/html/entry.html?qrCode=/202408260000001953" };
char gMchntcnName[32] = { 0 };
int gTraceNo = 0;
int gBatchNo = 0;
static int dsp_init_flag = 0;

dsp_tradeInfo_t m_tradeInfo = { 0 };


static unsigned int m_dsp_mutext_lock = 0;
#define DSP_MUTEX_LOCK		 if(m_dsp_mutext_lock == 0){m_dsp_mutext_lock= OsSysSemNew(1);}if(m_dsp_mutext_lock!=0)OsSysSemWait(m_dsp_mutext_lock);
#define	DSP_MUTEX_UNLOCK	  if(m_dsp_mutext_lock!=0)OsSysSemSignal(m_dsp_mutext_lock);


#define OFFLINE_TEST 1

static int write_transaction_info(void) {
	char filename[32] = { 0 };
	strcat(filename, DEVICEID);
	strcat(filename, ".bin");
	int fp = OsFileOpen(filename, API_WRONLY | API_CREAT);
	int ret = OsFileWrite(fp, (char*)&m_tradeInfo, sizeof(dsp_tradeInfo_t));
	OsFileClose(fp);
	return ret == sizeof(dsp_tradeInfo_t) ? 0 : -1;
}


static int read_transaction_info(void) {

	char filename[32] = { 0 };
	strcat(filename, DEVICEID);
	strcat(filename, ".bin");
	int fp = OsFileOpen(filename, API_RDONLY);
	if (fp >= 0) {
		int ret = OsFileRead(fp, (char*)&m_tradeInfo, sizeof(dsp_tradeInfo_t));
		OsFileClose(fp);
		return ret == sizeof(dsp_tradeInfo_t) ? 0 : -1;
	}
	return -1;
}


int app_dsp_getInitFlag(void) {
	if (strlen(serialID) == 0) {
		return 0;
	}
	return dsp_init_flag;
}




int app_dsp_login(char* mchntCd, char* deviceId, char* url, char* shopIdOut, char* staffIdOut,
	char* errorMsg) {
	int ret = -1, i = 0;
	int iRet=-1;
	char* resp_content = NULL;
	int resp_content_len = 0;
	char* headers = (char*)OsSysMalloc(1024);
	memset(headers, 0, 1024);
	char fullUrl[128] = { 0 };
	char* bodyParamsStr = NULL;
	cJSON* root = NULL, * data = NULL;
	cJSON* respRoot = NULL;
	root = cJSON_CreateObject();
	data = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "sysId", "001");
	cJSON_AddStringToObject(root, "mchntCd", mchntCd);
	cJSON_AddStringToObject(root, "deviceSn", serialID);

	cJSON_AddStringToObject(data, "version", "1.0.0");
	cJSON_AddStringToObject(data, "apiFlag", "1");
	cJSON_AddStringToObject(data, "transType", "0004");
	cJSON_AddStringToObject(data, "mchntCd", mchntCd);
	cJSON_AddStringToObject(data, "loginName", "04");
	cJSON_AddStringToObject(data, "loginPwd", "4a7d1ed414474e4033ac29ccb8653d9b");
	cJSON_AddStringToObject(data, "deviceSn", serialID);

	cJSON_AddItemToObject(root, "data", data);

	sprintf(fullUrl, "%s/trade/deviceComm", url);
	bodyParamsStr = cJSON_PrintUnformatted(root);
	if (bodyParamsStr != NULL)
	{
		API_LOG_DEBUG("app_dsp_login post data:\n%s\n", bodyParamsStr);
	}
	snprintf(headers, 1024, "Content-Type: application/json\r\nContent-Length: %lu\r\n", strlen(bodyParamsStr));
	for (i = 0; i < HTTP_TRYCNT; i++) {
		ret = OsHttpHandle(fullUrl, "POST", headers, NULL, bodyParamsStr, strlen(bodyParamsStr), &resp_content, &resp_content_len);
		API_LOG_DEBUG("app_dsp_login OsHttpHandle ret=%d,responseBuf(%d):%s\r\n", ret, resp_content_len, resp_content);
		if (ret == 0) {
			break;
		}
		if (resp_content) {
			OsSysFree(resp_content);
			resp_content = NULL;
		}
	}
	if (ret == 0) {
		ret = -1;
		respRoot = cJSON_Parse(resp_content);
		if (respRoot == NULL) {
			API_LOG_DEBUG("app_dsp_login buf parse error\r\n");
			ret = -1;
			goto exit;
		}
		cJSON* respCode = cJSON_GetObjectItem(respRoot, "respCode");
		if (respCode) {
			if (strcmp(respCode->valuestring, "00")) {
				cJSON* respMsg = cJSON_GetObjectItem(respRoot, "respMsg");
				API_LOG_DEBUG("respMsg:%s\r\n", respMsg->valuestring);
				memcpy(errorMsg, respMsg->valuestring, strlen(respMsg->valuestring));
				ret = -1;
				goto exit;
			}
			else {

				cJSON* transInfo = cJSON_GetObjectItem(respRoot, "transInfo");
				cJSON* shopId = cJSON_GetObjectItem(transInfo, "shopId");
				if (shopId) {
					memcpy(shopIdOut, shopId->valuestring, strlen(shopId->valuestring));
				}
				cJSON* staffId = cJSON_GetObjectItem(transInfo, "staffId");
				if (staffId) {
					memcpy(staffIdOut, staffId->valuestring, strlen(staffId->valuestring));
				}
				API_LOG_DEBUG("shopIdOut:%s,staffIdOut:%s\r\n", shopIdOut, staffIdOut);

				iRet= read_transaction_info();

				API_LOG_DEBUG("read_transaction_info iRet:%d\r\n", iRet);
				if (iRet != -1) {
					API_LOG_DEBUG("trace and batch read from file:%s,traceNo:%s,batchNo:%s\r\n",
						DSP_FILENAME, m_tradeInfo.traceNo, m_tradeInfo.batchNo);
					//gBatchNo = atoi(m_tradeInfo.batchNo);
					//gTraceNo = atoi(m_tradeInfo.traceNo);
				}
				else {
					write_transaction_info();
				}
			//	ret = app_dsp_signin(MCHNTCD, DEVICEID, DSP_URL);
			}
		}
		else {
			ret = -1;
		}
	}
exit:
	if (headers) {
		OsSysFree(headers);
	}
	if (bodyParamsStr) {
		OsSysFree(bodyParamsStr);
	}
	if (root) {
		cJSON_Delete(root);
	}
	if (resp_content) {
		OsSysFree(resp_content);
	}
	if (respRoot) {
		cJSON_Delete(respRoot);
	}
	API_LOG_DEBUG("app_dsp_login ret=%d\r\n", ret);
	if (ret == 0) {
		dsp_init_flag = 1;
	}
	return ret;
}


int app_dsp_signin(char* mchntCd, char* deviceId, char* url) {
	int ret = -1, i = 0;
	char* resp_content = NULL;
	int resp_content_len = 0;
	char* headers = (char*)OsSysMalloc(1024);
	memset(headers, 0, 1024);
	char fullUrl[128] = { 0 };
	char* bodyParamsStr = NULL;
	cJSON* root = NULL, * data = NULL;
	cJSON* respRoot = NULL;
	root = cJSON_CreateObject();
	data = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "sysId", "001");
	cJSON_AddStringToObject(root, "mchntCd", mchntCd);
	cJSON_AddStringToObject(root, "deviceSn", serialID);
	cJSON_AddStringToObject(data, "version", "1.0.0");
	cJSON_AddStringToObject(data, "apiFlag", "1");
	cJSON_AddStringToObject(data, "transType", "0005");
	cJSON_AddStringToObject(data, "mchntCd", mchntCd);
	cJSON_AddStringToObject(data, "deviceSn", serialID);
	cJSON_AddStringToObject(data, "staffId", gStaffId);
	cJSON_AddItemToObject(root, "data", data);

	sprintf(fullUrl, "%s/trade/deviceComm", url);
	bodyParamsStr = cJSON_PrintUnformatted(root);
	if (bodyParamsStr != NULL)
	{
		API_LOG_DEBUG("post data:\n%s\n", bodyParamsStr);
	}
	snprintf(headers, 1024, "Content-Type: application/json\r\nContent-Length: %lu\r\n", strlen(bodyParamsStr));
	for (i = 0; i < HTTP_TRYCNT; i++) {
		ret = OsHttpHandle(fullUrl, "POST", headers, NULL, bodyParamsStr, strlen(bodyParamsStr), &resp_content, &resp_content_len);
		API_LOG_DEBUG("app_dsp_signin OsHttpHandle ret=%d,responseBuf(%d):%s\r\n", ret, resp_content_len, resp_content);
		if (ret == 0)
		{
			break;
		}
		if (resp_content) {
			OsSysFree(resp_content);
			resp_content = NULL;
		}
	}
	if (ret == 0) {
		ret = -1;
		respRoot = cJSON_Parse(resp_content);
		if (respRoot == NULL) {
			API_LOG_DEBUG("app_dsp_signin buf parse error\r\n");
			ret = -1;
			goto exit;
		}
		cJSON* respCode = cJSON_GetObjectItem(respRoot, "respCode");
		if (respCode) {
			if (strcmp(respCode->valuestring, "00")) {
				cJSON* respMsg = cJSON_GetObjectItem(respRoot, "respMsg");
				API_LOG_DEBUG("respMsg:%s\r\n", respMsg->valuestring);
			}
			else {
				cJSON* transInfo = cJSON_GetObjectItem(respRoot, "transInfo");
				cJSON* batchNo = cJSON_GetObjectItem(transInfo, "batchNo");
				cJSON* traceNo = cJSON_GetObjectItem(transInfo, "traceNo");
				if (batchNo && traceNo) {
					API_LOG_DEBUG("read dsp server traceNo:%s,batchNo:%s\r\n",
						traceNo->valuestring, batchNo->valuestring);
#if 0
					if (atoi(batchNo->valuestring) >= gBatchNo)
					{
						gBatchNo = atoi(batchNo->valuestring);
						sprintf(m_tradeInfo.batchNo, "%s", batchNo->valuestring);
					}
					if (atoi(traceNo->valuestring) >= gTraceNo) {
						gTraceNo = atoi(traceNo->valuestring);
						sprintf(m_tradeInfo.traceNo, "%s", traceNo->valuestring);
					}
#endif
					API_LOG_DEBUG("update local traceNo,batchNo\r\n");
					write_transaction_info();
					ret = 0;
				}
				else {
					API_LOG_DEBUG("no traceNo or batchNo\r\n");
					ret = -1;
				}
			}
		}
		else {
			ret = -1;
		}
	}
exit:
	if (headers) {
		OsSysFree(headers);
	}
	if (bodyParamsStr) {
		OsSysFree(bodyParamsStr);
	}
	if (root) {
		cJSON_Delete(root);
	}
	if (resp_content) {
		OsSysFree(resp_content);
	}
	if (respRoot) {
		cJSON_Delete(respRoot);
	}
	API_LOG_DEBUG("app_dsp_signin ret=%d\r\n", ret);
	if (ret == 0) {
		dsp_init_flag = 1;
	}
	return ret;
}



int app_dsp_qrcodePay(
	char* mchntCd, char* deviceId,
	char* url, char* authCode,
	char* amount,char * uniqueOrderIdentifier,
	char* msgOut
)
{

	DSP_MUTEX_LOCK;

	int ret = -1, i = 0;
	char* resp_content = NULL;
	int resp_content_len = 0;
	char* headers = (char*)OsSysMalloc(1024);
	memset(headers, 0, 1024);
	char fullUrl[128] = { 0 };
	char* bodyParamsStr = NULL;
	cJSON* root = NULL, * data = NULL;
	cJSON* respRoot = NULL;
	char traceNo[8] = { 0 };
	char batchNo[8] = { 0 };
	root = cJSON_CreateObject();
	data = cJSON_CreateObject();

	API_LOG_DEBUG("app_dsp_qrcodePay gTraceNo:%d, gBatchNo:%d\r\n", gTraceNo, gBatchNo);
	sprintf(traceNo, "%06d", gTraceNo);
	sprintf(batchNo, "%06d", gBatchNo);
	sprintf(m_tradeInfo.traceNo, "%06d", gTraceNo);
	sprintf(m_tradeInfo.batchNo, "%06d", gBatchNo);

	cJSON_AddStringToObject(root, "sysId", "001");
	cJSON_AddStringToObject(root, "mchntCd", mchntCd);
	cJSON_AddStringToObject(root, "deviceSn", serialID);

	cJSON_AddStringToObject(data, "apiFlag", "1");
	cJSON_AddStringToObject(data, "authCode", authCode);
	cJSON_AddStringToObject(data, "batchNo", batchNo);
	cJSON_AddStringToObject(data, "deviceSn", serialID);
	cJSON_AddStringToObject(data, "mchntCd", mchntCd);
	cJSON_AddStringToObject(data, "staffId", "0002");
	cJSON_AddStringToObject(data, "traceNo", traceNo);
	cJSON_AddStringToObject(data, "transAmount", amount);
	cJSON_AddStringToObject(data, "transType", "1101");
	cJSON_AddStringToObject(data, "version", "V1.0.1_20240109");
	cJSON_AddStringToObject(data, "uniqueOrderIdentifier", uniqueOrderIdentifier);
	cJSON_AddItemToObject(root, "data", data);

	sprintf(fullUrl, "%s/trade/qrcodePay", url);
	bodyParamsStr = cJSON_PrintUnformatted(root);
	if (bodyParamsStr != NULL)
	{
		API_LOG_DEBUG("post data:%s\n", bodyParamsStr);
	}
	snprintf(headers, 1024, "Content-Type: application/json\r\nContent-Length: %lu\r\n", strlen(bodyParamsStr));
	app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "Connecting To Server", LV_COLOR_DSP_BLUE);
	for (i = 0; i < HTTP_TRYCNT; i++) {
		ret = OsHttpHandle(fullUrl, "POST", headers, NULL, bodyParamsStr, strlen(bodyParamsStr), &resp_content, &resp_content_len);
		API_LOG_DEBUG("app_dsp_qrcodePay111 OsHttpHandle ret=%d,responseBuf(%d):%s\r\n", ret, resp_content_len, resp_content);
		if (ret == 0)
		{
			break;
		}
		if (resp_content) {
			OsSysFree(resp_content);
			resp_content = NULL;
		}
	}
	if (ret == 0) {
		ret = -1;
		respRoot = cJSON_Parse(resp_content);
		if (respRoot == NULL) {
			API_LOG_DEBUG("app_dsp_qrcodePay buf parse error\r\n");
			ret = -1;
			goto exit;
		}
		cJSON* respCode = cJSON_GetObjectItem(respRoot, "respCode");
		if (respCode) {
			if (strcmp(respCode->valuestring, "00")) {
				cJSON* respMsg = cJSON_GetObjectItem(respRoot, "respMsg");
				API_LOG_DEBUG("respMsg:%s\r\n", respMsg->valuestring);
				API_LOG_DEBUG("error traceNo:%d,batchNo:%d\r\n", gTraceNo, gBatchNo);
				memcpy(msgOut, respMsg->valuestring, strlen(respMsg->valuestring));
				ret = -1;
				goto exit;
			}
			else {

				cJSON* transInfo = cJSON_GetObjectItem(respRoot, "transInfo");
				if (transInfo) {
					cJSON* transStatusObj = cJSON_GetObjectItem(transInfo, "transStatus");
					if (transStatusObj) {
						if (strcmp(transStatusObj->valuestring, "02") == 0) {
							ret = 0;
						}
					}

				}
				
			}
		}
		else {
			memcpy(msgOut, "qrcode pay fail", strlen("qrcode pay fail"));
			ret = -1;
			goto exit;
		}
	}
exit:
	if (headers) {
		OsSysFree(headers);
	}
	if (bodyParamsStr) {
		OsSysFree(bodyParamsStr);
	}
	if (root) {
		cJSON_Delete(root);
	}
	if (resp_content) {
		OsSysFree(resp_content);
	}
	if (respRoot) {
		cJSON_Delete(respRoot);
	}
	API_LOG_DEBUG("app_dsp_qrcodePay ret=%d\r\n", ret);
	DSP_MUTEX_UNLOCK;
	return ret;
}


int app_dsp_getQrcode(
	char* mchntCd, char* deviceId,
	char* url, char* amount,
	char* qecodeUrlOut, char* outTradeNoOut,char * uniqueOrderIdentifier,
	char* msgOut
)
{
	int ret = -1, i = 0;
	static int try_cnt = 0;
	try_cnt = 0;
	char* resp_content = NULL;
	int resp_content_len = 0;
	char* headers = NULL;
	char traceNo[12] = { 0 }, batchNo[12] = { 0 }, fullUrl[128] = { 0 };
	char* bodyParamsStr = NULL;
	cJSON* root = NULL, * data = NULL;
	cJSON* respRoot = NULL;

get_code_start:
	//PubComm_HttpConnectPrev(url);
	memset(fullUrl, 0, sizeof(fullUrl));
	memset(traceNo, 0, sizeof(traceNo));
	memset(batchNo, 0, sizeof(batchNo));


	headers = (char*)OsSysMalloc(1024);
	memset(headers, 0, 1024);
	if (++gTraceNo % 1000000 == 999999) {
		API_LOG_DEBUG("app_dsp_getQrcode gTraceNo overflow\r\n");
	}
	//gTraceNo = 1;
	API_LOG_DEBUG("app_dsp_getQrcode current gTraceNo:%d, gBatchNo:%d\r\n", gTraceNo, gBatchNo);
	sprintf(traceNo, "%06d", gTraceNo);
	sprintf(batchNo, "%06d", gBatchNo);
	root = cJSON_CreateObject();
	data = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "sysId", "001");
	cJSON_AddStringToObject(root, "mchntCd", mchntCd);
	cJSON_AddStringToObject(root, "deviceSn", serialID);
	cJSON_AddStringToObject(root, "staffId", "0001");

	cJSON_AddStringToObject(data, "version", "1.0.0");
	cJSON_AddStringToObject(data, "apiFlag", "1");
	cJSON_AddStringToObject(data, "transType", "1102");
	cJSON_AddStringToObject(data, "mchntCd", mchntCd);
	cJSON_AddStringToObject(data, "staffId", "0001");
	cJSON_AddStringToObject(data, "shopId", "");
	cJSON_AddStringToObject(data, "traceNo", traceNo);
	cJSON_AddStringToObject(data, "transAmount", amount);
	cJSON_AddStringToObject(data, "notifyUrl", "");
	cJSON_AddStringToObject(data, "remark", "");
	cJSON_AddStringToObject(data, "qrValidTime", "110");
	cJSON_AddStringToObject(data, "batchNo", batchNo);
	cJSON_AddStringToObject(data, "tipAmount", "0");
	cJSON_AddStringToObject(data, "deviceSn", serialID);
	cJSON_AddStringToObject(data, "consumptionAmount", amount);
	cJSON_AddStringToObject(data, "orderAssociationFlag", "1");

	cJSON_AddItemToObject(root, "data", data);

	sprintf(fullUrl, "%s/trade/qrcodePay", url);
	bodyParamsStr = cJSON_PrintUnformatted(root);
	if (bodyParamsStr != NULL)
	{
		API_LOG_DEBUG("app_dsp_getQrcode post data:\n%s\n", bodyParamsStr);
	}
	snprintf(headers, 1024, "Content-Type: application/json\r\nContent-Length: %lu\r\n", strlen(bodyParamsStr));
	app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "Connecting To Server", LV_COLOR_DSP_BLUE);
	for (i = 0; i < HTTP_TRYCNT; i++) {
		ret = OsHttpHandle(fullUrl, "POST", headers, NULL, bodyParamsStr, strlen(bodyParamsStr), &resp_content, &resp_content_len);
		API_LOG_DEBUG("app_dsp_getQrcode OsHttpHandle ret=%d,responseBuf(%d):%s\r\n", ret, resp_content_len, resp_content);
		if (ret == 0) {
			break;
		}
		if (resp_content) {
			OsSysFree(resp_content);
			resp_content = NULL;
		}
	}
	if (ret == 0) {
		respRoot = cJSON_Parse(resp_content);
		if (respRoot == NULL) {
			API_LOG_DEBUG("app_dsp_getQrcode buf parse error\r\n");
			ret = -1;
			goto exit;
		}
		cJSON* respCode = cJSON_GetObjectItem(respRoot, "respCode");
		if (respCode) {
			if (strcmp(respCode->valuestring, "00")) {

				cJSON* respMsg = cJSON_GetObjectItem(respRoot, "respMsg");
				API_LOG_DEBUG("app_dsp_getQrcode respMsg:%s\r\n", respMsg->valuestring);
				memcpy(msgOut, respMsg->valuestring, strlen(respMsg->valuestring));
				msgOut[strlen(respMsg->valuestring)] = 0;
				ret = -1;
				goto exit;
			}
			else {

				cJSON* transInfo = cJSON_GetObjectItem(respRoot, "transInfo");
				if (transInfo) {
					cJSON* qrcodeUrl = cJSON_GetObjectItem(transInfo, "qrcodeUrl");
					if (qrcodeUrl) {
						memcpy(qecodeUrlOut, qrcodeUrl->valuestring, strlen(qrcodeUrl->valuestring));
						API_LOG_DEBUG("app_dsp_getQrcode qecodeUrlOut:%s\r\n", qecodeUrlOut);
						memcpy(msgOut, "success", strlen("success"));
						try_cnt = 3;
					}
					cJSON* outTradeNo = cJSON_GetObjectItem(transInfo, "outTradeNo");
					if (outTradeNo) {
						memcpy(outTradeNoOut, outTradeNo->valuestring, strlen(outTradeNo->valuestring));
						API_LOG_DEBUG("app_dsp_getQrcode outTradeNoOut:%s\r\n", outTradeNoOut);
					}
					cJSON* uniqueOrderIdentifierObj = cJSON_GetObjectItem(transInfo, "uniqueOrderIdentifier");
					if (uniqueOrderIdentifier) {
						memcpy(uniqueOrderIdentifier, uniqueOrderIdentifierObj->valuestring, strlen(uniqueOrderIdentifierObj->valuestring));
						API_LOG_DEBUG("app_dsp_getQrcode uniqueOrderIdentifier:%s\r\n", uniqueOrderIdentifier);
					}
				}
				ret = 0;
				sprintf(m_tradeInfo.traceNo, "%06d", gTraceNo);
				sprintf(m_tradeInfo.batchNo, "%06d", gBatchNo);
				write_transaction_info();
			}
		}
		else {
			ret = -1;
			goto exit;
		}
	}
	else {
		memcpy(msgOut, "request error", strlen("request error"));
		msgOut[strlen("request error")] = 0;
	}
exit:
	if (headers) {
		OsSysFree(headers);
	}

	if (bodyParamsStr) {
		OsSysFree(bodyParamsStr);
	}
	if (root) {
		cJSON_Delete(root);
	}
	if (resp_content) {
		OsSysFree(resp_content);
	}
	if (respRoot) {
		cJSON_Delete(respRoot);
	}
	headers = NULL;
	bodyParamsStr = NULL;
	root = NULL;
	resp_content = NULL;
	respRoot = NULL;
	API_LOG_DEBUG("app_dsp_getQrcode ret=%d,msgOut:%s\r\n", ret, msgOut);
	if ((strstr(msgOut, "Duplicate serial number") != NULL)&& try_cnt < 3) {
		try_cnt++;
		API_LOG_DEBUG("app_dsp_getQrcode reget qrcode\r\n");
		sprintf(m_tradeInfo.traceNo, "%06d", gTraceNo);
		sprintf(m_tradeInfo.batchNo, "%06d", gBatchNo);
		write_transaction_info();
		goto get_code_start;
	}
	return ret;
}








int app_dsp_checkPay(
	char* mchntCd, char* deviceId,
	char* url, char* tradeNo, int* transState,
	char* msgOut
)
{
	DSP_MUTEX_LOCK;
	int ret = -1;
	int itraceNo = 0;
	char* resp_content = NULL;
	int resp_content_len = 0;
	char* headers = (char*)OsSysMalloc(1024);
	memset(headers, 0, 1024);
	char fullUrl[128] = { 0 };
	char* bodyParamsStr = NULL;
	cJSON* root = NULL, * data = NULL;
	cJSON* respRoot = NULL;
	//PubComm_HttpConnectPrev(url);
	root = cJSON_CreateObject();
	data = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "sysId", "001");
	cJSON_AddStringToObject(root, "mchntCd", mchntCd);
	cJSON_AddStringToObject(root, "deviceSn", serialID);


	cJSON_AddStringToObject(data, "version", "1.0.0");
	cJSON_AddStringToObject(data, "apiFlag", "1");
	cJSON_AddStringToObject(data, "transType", "1104");
	cJSON_AddStringToObject(data, "mchntCd", mchntCd);
	cJSON_AddStringToObject(data, "outTradeNo", tradeNo);
	cJSON_AddItemToObject(root, "data", data);

	sprintf(fullUrl, "%s/trade/qrcodePay", url);
	bodyParamsStr = cJSON_PrintUnformatted(root);
	if (bodyParamsStr != NULL)
	{
		API_LOG_DEBUG("post data:\n%s\n", bodyParamsStr);
	}
	snprintf(headers, 1024, "Content-Type: application/json\r\nContent-Length: %lu\r\n", strlen(bodyParamsStr));
	ret = OsHttpHandle(fullUrl, "POST", headers, NULL, bodyParamsStr, strlen(bodyParamsStr), &resp_content, &resp_content_len);
	API_LOG_DEBUG("app_dsp_checkPay tradeNo:%s\r\napi_http_handle ret=%d,responseBuf(%d):%s\r\n", tradeNo, ret, resp_content_len, resp_content);
	if (ret == 0) {
		respRoot = cJSON_Parse(resp_content);
		if (respRoot == NULL) {
			API_LOG_DEBUG("app_dsp_qrcodePay buf parse error\r\n");
			ret = -1;
			goto exit;
		}
		cJSON* respCode = cJSON_GetObjectItem(respRoot, "respCode");
		if (respCode) {
			if (strcmp(respCode->valuestring, "00")) {
				cJSON* respMsg = cJSON_GetObjectItem(respRoot, "respMsg");
				if (respMsg) {
					API_LOG_DEBUG("respMsg:%s\r\n", respMsg->valuestring);
					API_LOG_DEBUG_HEX(respMsg->valuestring,strlen(respMsg->valuestring));
					memcpy(msgOut, respMsg->valuestring, strlen(respMsg->valuestring));
				}
				ret = -1;
				goto exit;
			}
			else {
				cJSON* transInfo = cJSON_GetObjectItem(respRoot, "transInfo");
				cJSON* transStatus = cJSON_GetObjectItem(transInfo, "transStatus");
				if (transStatus) {
					*transState = atoi(transStatus->valuestring);
				}
			}
			ret = 0;
		}
		else {
			memcpy(msgOut, "https error", "https error");
			ret = -1;
			goto exit;
		}
	}

exit:
	if (headers) {
		OsSysFree(headers);
	}

	if (bodyParamsStr) {
		OsSysFree(bodyParamsStr);
	}
	if (root) {
		cJSON_Delete(root);
	}
	if (resp_content) {
		OsSysFree(resp_content);
	}
	if (respRoot) {
		cJSON_Delete(respRoot);
	}
	DSP_MUTEX_UNLOCK;
	return ret;
}


/*
{
	"respCode": "00",
	"respMsg": "SUCCESS",
	"transInfo": {
		"transAmount": null,
		"activityAmount": null,
		"actualAmount": null,
		"transStatus": "02",
		"transStatusDesc": "�˿�ɹ�",
		"fundType": null,
		"traceNo": "000010",
		"outTradeNo": "202501151034540000184115",
		"transDatetime": "2025-01-15 10:34:54",
		"transDatetimeEnd": null,
		"channelType": "1",
		"transType": null,
		"handlingCharge": "3",
		"remark": "",
		"origOutTradeNo": "202501141440390000183723",
		"origTransAmount": "1",
		"refundFee": "1",
		"qrcodeUrl": null,
		"qrValidTime": null,
		"tipAmount": "0",
		"consumptionAmount": "1"
	},
	"macValue": null
}
*/
int app_dsp_qrcodePayRefund(
	char* mchntCd, char* deviceId,
	char* url, char* amount,
	char* traceNo, char* batchNo,
	char* outTradeId, int* transState,
	char* msgOut) {

	int ret = -1;
	int itraceNo = 0;
	char* resp_content = NULL;
	int resp_content_len = 0;
	char* headers = (char*)OsSysMalloc(1024);
	memset(headers, 0, 1024);
	char fullUrl[128] = { 0 };
	char* bodyParamsStr = NULL;
	cJSON* root = NULL, * data = NULL;
	cJSON* respRoot = NULL;
	root = cJSON_CreateObject();
	data = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "sysId", "001");
	cJSON_AddStringToObject(root, "staffId", gStaffId);
	cJSON_AddStringToObject(root, "mchntCd", mchntCd);
	cJSON_AddStringToObject(root, "deviceSn", serialID);


	cJSON_AddStringToObject(data, "version", "1.0.0");
	cJSON_AddStringToObject(data, "apiFlag", "1");
	cJSON_AddStringToObject(data, "transType", "1106");
	cJSON_AddStringToObject(data, "deviceSn", serialID);
	cJSON_AddStringToObject(data, "traceNo", traceNo);
	cJSON_AddStringToObject(data, "origOutTradeNo", outTradeId);
	cJSON_AddStringToObject(data, "refundFee", amount);
	cJSON_AddStringToObject(data, "remark", "");
	cJSON_AddStringToObject(data, "batchNo", batchNo);

	cJSON_AddItemToObject(root, "data", data);

	sprintf(fullUrl, "%s/trade/qrcodePay", url);
	bodyParamsStr = cJSON_PrintUnformatted(root);
	if (bodyParamsStr != NULL)
	{
		API_LOG_DEBUG("post data:\n%s\n", bodyParamsStr);
	}
	snprintf(headers, 1024, "Content-Type: application/json\r\nContent-Length: %lu\r\n", strlen(bodyParamsStr));
	app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "Connecting To Server", LV_COLOR_DSP_BLUE);
	ret = OsHttpHandle(fullUrl, "POST", headers, NULL, bodyParamsStr, strlen(bodyParamsStr), &resp_content, &resp_content_len);
	API_LOG_DEBUG("app_dsp_qrcodePayRefund traceNo:%s OsHttpHandle ret=%d,responseBuf(%d):%s\r\n", traceNo, ret, resp_content_len, resp_content);
	if (ret == 0) {
		respRoot = cJSON_Parse(resp_content);
		if (respRoot == NULL) {
			API_LOG_DEBUG("app_dsp_qrcodePay buf parse error\r\n");
			ret = -1;
			goto exit;
		}
		else {
			cJSON* respCode = cJSON_GetObjectItem(respRoot, "respCode");
			if (respCode) {
				if (strcmp(respCode->valuestring, "00")) {
					ret = -1;
					cJSON* respMsg = cJSON_GetObjectItem(respRoot, "respMsg");
					if (respMsg) {
						API_LOG_DEBUG("respMsg:%s\r\n", respMsg->valuestring);
						memcpy(msgOut, respMsg->valuestring, strlen(respMsg->valuestring));
						goto exit;
					}
				}
				else {
					cJSON* transInfo = cJSON_GetObjectItem(respRoot, "transInfo");
					if (transInfo) {
						cJSON* transStatus = cJSON_GetObjectItem(transInfo, "transStatus");
						if (transStatus) {
							if (strcmp(transStatus->valuestring,"02")==0) {
								ret = 0;
							}
						}
					}
				}

			}
			else {
				ret = -1;
				memcpy(msgOut, "server error", strlen("server error"));
			}

		}
	}
	else {
		ret = -1;
		memcpy(msgOut, "server error", strlen("server error"));
	}

exit:
	if (headers) {
		OsSysFree(headers);
	}
	if (bodyParamsStr) {
		OsSysFree(bodyParamsStr);
	}
	if (root) {
		cJSON_Delete(root);
	}
	if (resp_content) {
		OsSysFree(resp_content);
	}
	if (respRoot) {
		cJSON_Delete(respRoot);
	}
	return ret;
}


int app_dsp_transactionFlow(
	char* mchntCd, char* deviceId,
	char* url, int pageNo,
	int pageSize, int* total, dsp_tradeInfo_t* infos,
	char* msgOut) {
	int i = 0;
	int ret = -1;
	int itraceNo = 0;
	char* resp_content = NULL;
	int resp_content_len = 0;
	char* headers = (char*)OsSysMalloc(1024);
	memset(headers, 0, 1024);
	char fullUrl[128] = { 0 };
	char* bodyParamsStr = NULL;
	cJSON* root = NULL, * params = NULL;
	cJSON* respRoot = NULL;
	root = cJSON_CreateObject();
	params = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "pageNo", pageNo);
	cJSON_AddNumberToObject(root, "pageSize", pageSize);



	cJSON_AddStringToObject(params, "mchntCd", mchntCd);
	cJSON_AddStringToObject(params, "deviceSn", serialID);

	cJSON_AddItemToObject(root, "params", params);

	sprintf(fullUrl, "%s/trade/transactionFlow", url);
	bodyParamsStr = cJSON_PrintUnformatted(root);
	if (bodyParamsStr != NULL)
	{
		API_LOG_DEBUG("post data:\n%s\n", bodyParamsStr);
	}
	snprintf(headers, 1024, "Content-Type: application/json\r\nContent-Length: %lu\r\n", strlen(bodyParamsStr));
	app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "Connecting To Server", LV_COLOR_DSP_BLUE);

	for (i = 0; i < HTTP_TRYCNT; i++) {
		ret = OsHttpHandle(fullUrl, "POST", headers, NULL, bodyParamsStr, strlen(bodyParamsStr), &resp_content, &resp_content_len);
		API_LOG_DEBUG("app_dsp_transactionFlow OsHttpHandle ret=%d,responseBuf(%d):%s\r\n", ret, resp_content_len, resp_content);
		if (resp_content_len) {
			API_LOG_DEBUG("app_dsp_getStaticQrcode responseBuf(%d):%s\r\n", resp_content_len, resp_content);
		}
		if (ret == 0) {
			break;
		}
		if (resp_content) {
			OsSysFree(resp_content);
			resp_content = NULL;
		}
	}
	if (ret == 0) {
		respRoot = cJSON_Parse(resp_content);
		if (respRoot == NULL) {
			API_LOG_DEBUG("app_dsp_qrcodePay buf parse error\r\n");
			ret = -1;
			strcpy(msgOut, "get records error");
			goto exit;
		}
		else {
			cJSON* respCode = cJSON_GetObjectItem(respRoot, "respCode");
			if (respCode) {
				if (strcmp(respCode->valuestring, "00")) {
					cJSON* respMsg = cJSON_GetObjectItem(respRoot, "respMsg");
					if (respMsg) {
						API_LOG_DEBUG("respMsg:%s\r\n", respMsg->valuestring);
						memcpy(msgOut, respMsg->valuestring, strlen(respMsg->valuestring));
						ret = -1;
						goto exit;
					}
				}
				else {
					API_LOG_DEBUG("extract transInfo\r\n");
					cJSON* transInfo = cJSON_GetObjectItem(respRoot, "transInfo");
					cJSON* totalJson = cJSON_GetObjectItem(transInfo, "total");
					if (totalJson) {
						*total = totalJson->valueint;
					}

					cJSON* rows = cJSON_GetObjectItem(transInfo, "rows");
					if (rows != NULL) {
						dsp_tradeInfo_t* pinfos = infos;
						int row_count = cJSON_GetArraySize(rows);
						for (i = 0; i < row_count; i++) {
							cJSON* row = cJSON_GetArrayItem(rows, i);
							cJSON* item = cJSON_GetObjectItem(row, "traceNo");
							if (item != NULL) {
								if (item->valuestring != NULL)
									memcpy(pinfos->traceNo, item->valuestring, strlen(item->valuestring));
							}
							item = cJSON_GetObjectItem(row, "batchNo");
							if (item != NULL) {
								if (item->valuestring != NULL)
									memcpy(pinfos->batchNo, item->valuestring, strlen(item->valuestring));
							}
							item = cJSON_GetObjectItem(row, "transStatus");
							if (item != NULL) {
								if (item->valuestring != NULL)
									memcpy(pinfos->transStatus, item->valuestring, strlen(item->valuestring));
							}
							item = cJSON_GetObjectItem(row, "transTypeId");
							if (item != NULL) {
								if (item->valuestring != NULL)
									memcpy(pinfos->transTypeId, item->valuestring, strlen(item->valuestring));
							}
							item = cJSON_GetObjectItem(row, "outTradeId");
							if (item != NULL) {
								if (item->valuestring != NULL)
									memcpy(pinfos->origOutTradeId, item->valuestring, strlen(item->valuestring));
							}
							item = cJSON_GetObjectItem(row, "transDatetime");
							if (item != NULL) {
								if (item->valuestring != NULL)
									memcpy(pinfos->transDatetime, item->valuestring, strlen(item->valuestring));
							}

							item = cJSON_GetObjectItem(row, "transAmount");
							if (item != NULL) {
								if (item->valuestring != NULL)
									memcpy(pinfos->money, item->valuestring, strlen(item->valuestring));
							}

							item = cJSON_GetObjectItem(row, "cardNo");
							if (item != NULL) {
								if (item->valuestring != NULL)
									memcpy(pinfos->cardNo, item->valuestring, strlen(item->valuestring));
							}


							pinfos++;
						}
					}
					ret = i;
				}
			}
			else {
				ret = -1;
			}
		}
	}
	else {
		strcpy(msgOut, "get records error");
	}

exit:

	if (headers) {
		OsSysFree(headers);
	}
	if (bodyParamsStr) {
		OsSysFree(bodyParamsStr);
	}
	if (root) {
		cJSON_Delete(root);
	}
	if (resp_content) {
		OsSysFree(resp_content);
	}
	if (respRoot) {
		cJSON_Delete(respRoot);
	}
	return ret;
}





int app_dsp_settlement(
	char* mchntCd, char* deviceId,
	char* url, char* amount,
	char* traceNo, char* batchNo,
	char* outTradeId, int* transState,
	char* msgOut) {

	int ret = -1;
	int itraceNo = 0;
	char* resp_content = NULL;
	int resp_content_len = 0;
	char* headers = (char*)OsSysMalloc(1024);
	memset(headers, 0, 1024);
	char fullUrl[128] = { 0 };
	char* bodyParamsStr = NULL;
	cJSON* root = NULL, * data = NULL;
	cJSON* respRoot = NULL;
	root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "mchntCd", mchntCd);
	cJSON_AddStringToObject(root, "deviceSn", serialID);



	sprintf(fullUrl, "%s/trade/settlement", url);
	bodyParamsStr = cJSON_PrintUnformatted(root);
	if (bodyParamsStr != NULL)
	{
		API_LOG_DEBUG("post data:\n%s\n", bodyParamsStr);
	}
	snprintf(headers, 1024, "Content-Type: application/json\r\nContent-Length: %lu\r\n", strlen(bodyParamsStr));
	app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "Connecting To Server", LV_COLOR_DSP_BLUE);
	ret = OsHttpHandle(fullUrl, "POST", headers, NULL, bodyParamsStr, strlen(bodyParamsStr), &resp_content, &resp_content_len);
	API_LOG_DEBUG("app_dsp_checkPay traceNo:%s OsHttpHandle ret=%d,responseBuf(%d):%s\r\n", traceNo, ret, resp_content_len, resp_content);
	if (ret == 0) {
		respRoot = cJSON_Parse(resp_content);
		if (respRoot == NULL) {
			API_LOG_DEBUG("app_dsp_qrcodePay buf parse error\r\n");
			ret = -1;
			goto exit;
		}
		else {
			cJSON* respCode = cJSON_GetObjectItem(respRoot, "respCode");
			if (respCode) {
				if (strcmp(respCode->valuestring, "00")) {
					cJSON* respMsg = cJSON_GetObjectItem(respRoot, "respMsg");
					if (respMsg) {
						API_LOG_DEBUG("respMsg:%s\r\n", respMsg->valuestring);
						memcpy(msgOut, respMsg->valuestring, strlen(respMsg->valuestring));
						ret = -1;
						goto exit;
					}
				}
				else {
					ret = 0;
				}
			}
			else {
				ret = -1;
			}
		}
	}
exit:
	if (headers) {
		OsSysFree(headers);
	}
	if (bodyParamsStr) {
		OsSysFree(bodyParamsStr);
	}
	if (root) {
		cJSON_Delete(root);
	}
	if (resp_content) {
		OsSysFree(resp_content);
	}
	if (respRoot) {
		cJSON_Delete(respRoot);
	}
	return ret;
}



int app_dsp_getStaticQrcode(
	char* mchntCd, char* deviceId,
	char* url, char* qrUrl, char* mchntcnNameStr,
	char* msgOut) {

	int ret = -1, i = 0;
	int itraceNo = 0;
	char* resp_content = NULL;
	int resp_content_len = 0;
	char* headers = (char*)OsSysMalloc(1024);
	memset(headers, 0, 1024);
	char fullUrl[128] = { 0 };
	char* bodyParamsStr = NULL;
	cJSON* root = NULL, * data = NULL;
	cJSON* respRoot = NULL;
	root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "mchntCd", mchntCd);
	cJSON_AddStringToObject(root, "deviceSn", serialID);

	sprintf(fullUrl, "%s/trade/getQRCode", url);
	bodyParamsStr = cJSON_PrintUnformatted(root);
	if (bodyParamsStr != NULL)
	{
		API_LOG_DEBUG("app_dsp_getStaticQrcode post data:\n%s\n", bodyParamsStr);
	}
	snprintf(headers, 1024, "Content-Type: application/json\r\nContent-Length: %lu\r\n", strlen(bodyParamsStr));
	app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "Connecting To Server", LV_COLOR_DSP_BLUE);
	for (i = 0; i < HTTP_TRYCNT; i++) {
		ret = OsHttpHandle(fullUrl, "POST", headers, NULL, bodyParamsStr, strlen(bodyParamsStr), &resp_content, &resp_content_len);

		API_LOG_DEBUG("app_dsp_getStaticQrcode OsHttpHandle ret=%d\r\n", ret);
		if (resp_content_len) {
			API_LOG_DEBUG("app_dsp_getStaticQrcode responseBuf(%d):%s\r\n", resp_content_len, resp_content);
		}
		if (ret == 0) {
			break;
		}

		if (resp_content) {
			OsSysFree(resp_content);
			resp_content = NULL;
		}

	}
	if (ret == 0) {
		respRoot = cJSON_Parse(resp_content);
		if (respRoot == NULL) {
			API_LOG_DEBUG("app_dsptStaticQrcode buf parse error\r\n");
			ret = -1;
			goto exit;
		}
		else {
			cJSON* respCode = cJSON_GetObjectItem(respRoot, "respCode");
			if (respCode) {
				if (strcmp(respCode->valuestring, "00")) {
					cJSON* respMsg = cJSON_GetObjectItem(respRoot, "respMsg");
					if (respMsg) {
						API_LOG_DEBUG("respMsg:%s\r\n", respMsg->valuestring);
						memcpy(msgOut, respMsg->valuestring, strlen(respMsg->valuestring));
						ret = -1;
						goto exit;
					}
				}
				else {

					cJSON* transInfo = cJSON_GetObjectItem(respRoot, "transInfo");
					cJSON* staticQrurl = cJSON_GetObjectItem(transInfo, "url");
					if (qrUrl) {
						strcpy(qrUrl, staticQrurl->valuestring);
					//	qrUrl[strlen(qrUrl)] = 0;
						ret = 0;
					}
					else {
						ret = -1;
						memcpy(msgOut, "get qr url error", strlen("get qr url error"));
					}

					cJSON* mchntcnName = cJSON_GetObjectItem(transInfo, "mchntcnName");
					if (mchntcnName) {
						memcpy(mchntcnNameStr, mchntcnName->valuestring, strlen(mchntcnName->valuestring));
						ret = 0;
					}
					else {
						ret = -1;
						memcpy(msgOut, "get mchntcnName error", strlen("get mchntcnName error"));
					}




				}
			}
			else {
				ret = -1;
			}
		}
	}
	else {
		memcpy(msgOut, "server error", strlen("server error"));
	}
exit:
	if (headers) {
		OsSysFree(headers);
	}
	if (bodyParamsStr) {
		OsSysFree(bodyParamsStr);
	}
	if (root) {
		cJSON_Delete(root);
	}
	if (resp_content) {
		OsSysFree(resp_content);
	}
	if (respRoot) {
		cJSON_Delete(respRoot);
	}
	return ret;
}

int app_dsp_cardPay(
	char* url, APPEMVINFO *emvInfo,EmvOnlineData_t* pOnlineData) {
	DSP_MUTEX_LOCK;
	int ret = -1, i = 0;
	int itraceNo = 0;
	char* resp_content = NULL;
	int resp_content_len = 0;
	char* headers = (char*)OsSysMalloc(1024*2);
	memset(headers, 0, 1024*2);

	char traceNo[8] = { 0 };
	char batchNo[8] = { 0 };
	char *icData=NULL;

	char fullUrl[128] = { 0 };
	char* bodyParamsStr = NULL;
	cJSON* root = NULL, * data = NULL;
	cJSON* respRoot = NULL;
	root = cJSON_CreateObject();
	data = cJSON_CreateObject();

	sprintf(traceNo, "%06d", gTraceNo);
	sprintf(batchNo, "%06d", gBatchNo);
	sprintf(m_tradeInfo.traceNo, "%06d", gTraceNo);
	sprintf(m_tradeInfo.batchNo, "%06d", gBatchNo);
	write_transaction_info();

	cJSON_AddStringToObject(root, "deviceSn", serialID);
	cJSON_AddStringToObject(root, "staffId", "0001");
	cJSON_AddStringToObject(root, "sysId", "001");

	cJSON_AddStringToObject(data, "appVersion", "1.0.0");
	cJSON_AddStringToObject(data, "batchNo", batchNo);
	cJSON_AddStringToObject(data, "cardNo", emvInfo->szPan);
	cJSON_AddStringToObject(data, "cardSN", emvInfo->szCardSerialNo);

	icData=(char*)OsSysMalloc(emvInfo->cFieldlen*2+1);
	memset(icData,0,emvInfo->cFieldlen*2+1);
	Pub_BcdToAsc(emvInfo->sField55,icData,emvInfo->cFieldlen);

	cJSON_AddStringToObject(data, "icData", icData);
	cJSON_AddStringToObject(data, "traceNo", traceNo);
	cJSON_AddStringToObject(data, "track2",emvInfo->sztrack2);
	cJSON_AddItemToObject(root, "data", data);
	sprintf(fullUrl, "%s/trade/cardPay", url);
	bodyParamsStr = cJSON_PrintUnformatted(root);
	if (icData) {
		OsSysFree(icData);
	}

	if (bodyParamsStr != NULL)
	{
		API_LOG_DEBUG("app_dsp_cardPay post data:\n%s\n", bodyParamsStr);
	}
	snprintf(headers, 1024*2, "Content-Type: application/json\r\nContent-Length: %lu\r\n", strlen(bodyParamsStr));
	app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "Connecting To Server", LV_COLOR_DSP_BLUE);
	for (i = 0; i < HTTP_TRYCNT; i++) {
		ret = OsHttpHandle(fullUrl, "POST", headers, NULL, bodyParamsStr, strlen(bodyParamsStr), &resp_content, &resp_content_len);
		API_LOG_DEBUG("app_dsp_cardPay OsHttpHandle ret=%d,responseBuf(%d):%s\r\n", ret, resp_content_len, resp_content);
		if (ret == 0) {
			break;
		}
		if (resp_content) {
			OsSysFree(resp_content);
			resp_content = NULL;
		}
	}
#if OFFLINE_TEST
            memcpy(pOnlineData->iccResponse,"00",2); //responcess
            //Analyze the EMV data returned by the server, which should include 91tag or 71 or 72tag
            // memcpy(pOnlineData->ackdata,"\x72\x20\x86\x0E\x04\xDA\x00\x00\x02\x01\x92\x42\x81\x02\x7A\x08\xE4\x36\x86\x06\x04\xDA\x00\x00\x00\x01\x86\x06\x04\xDA\x00\x00\x01\x01",34);
			// pOnlineData->ackdatalen = 34;
			ret = 0;
#else	
	if (ret == 0) {
		//Process corresponding data
	}
#endif
exit:

	if (headers) {
		OsSysFree(headers);
	}
	if (bodyParamsStr) {
		OsSysFree(bodyParamsStr);
	}
	if (root) {
		cJSON_Delete(root);
	}
	if (resp_content) {
		OsSysFree(resp_content);
	}
	if (respRoot) {
		cJSON_Delete(respRoot);
	}
	DSP_MUTEX_UNLOCK;
	return ret;
}





int app_dsp_cardPayRefund(
	char* mchntCd, char* deviceId,
	char* url, char* amount, char* cardNo,
	char* deviceSN, char* traceNo, char* batchNo
	,char * origOutTradeNo,char* msgOut) {

	int ret = -1, i = 0;
	char* resp_content = NULL;
	int resp_content_len = 0;
	char* headers = (char*)OsSysMalloc(1024);
	memset(headers, 0, 1024);
	char fullUrl[128] = { 0 };
	char* bodyParamsStr = NULL;
	cJSON* root = NULL, * data = NULL;
	cJSON* respRoot = NULL;

	char newtraceNo[8] = { 0 };
	char newbatchNo[8] = { 0 };

	root = cJSON_CreateObject();
	data = cJSON_CreateObject();

	gTraceNo = ++gTraceNo % 10000000;
	sprintf(newtraceNo, "%06d", gTraceNo);
	sprintf(m_tradeInfo.traceNo, "%06d", gTraceNo);
	write_transaction_info();


	cJSON_AddStringToObject(root, "deviceSn", serialID);
	cJSON_AddStringToObject(root, "mchntCd", mchntCd);
	cJSON_AddStringToObject(root, "staffId", "0001");
	cJSON_AddStringToObject(root, "sysId", "001");

	//cJSON_AddStringToObject(data, "origTransAmount", amount);
	cJSON_AddStringToObject(data, "apiFlag", "1");
	cJSON_AddStringToObject(data, "appVersion", "1.0.0");
	//cJSON_AddStringToObject(data, "origBatchNo", batchNo);
	//cJSON_AddStringToObject(data, "batchNo", batchNo);
	//cJSON_AddStringToObject(data, "cardNo", cardNo);
	//cJSON_AddStringToObject(data, "cardSN", "002");
	cJSON_AddStringToObject(data, "deviceSn", serialID);
	cJSON_AddStringToObject(data, "icData", "");
	cJSON_AddStringToObject(data, "mchntCd", mchntCd);
	//cJSON_AddStringToObject(data, "payType", "1");
	//cJSON_AddStringToObject(data, "posem", "072");
	//cJSON_AddStringToObject(data, "random", "466469");
	//cJSON_AddStringToObject(data, "pinData", "0F277DCAD4AD8663");
	//cJSON_AddStringToObject(data, "origTraceNo", traceNo);
	//cJSON_AddStringToObject(data, "traceNo", newtraceNo);
	//cJSON_AddStringToObject(data, "trackSecond", "6250863835422101D320420603588253000F");
	cJSON_AddStringToObject(data, "transAmount", amount);
	cJSON_AddStringToObject(data, "transType", "1210");
	cJSON_AddStringToObject(data, "version", "1.0.0");
	cJSON_AddStringToObject(data, "cardNo", cardNo);
	cJSON_AddStringToObject(data, "origOutTradeNo", origOutTradeNo);

	cJSON_AddItemToObject(root, "data", data);
	sprintf(fullUrl, "%s/trade/cardPay", url);
	bodyParamsStr = cJSON_PrintUnformatted(root);
	if (bodyParamsStr != NULL)
	{
		API_LOG_DEBUG("app_dsp_cardPayRefund post data:\n%s\n", bodyParamsStr);
	}
	snprintf(headers, 1024, "Content-Type: application/json\r\nContent-Length: %lu\r\n", strlen(bodyParamsStr));
	app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "Connecting To Server", LV_COLOR_DSP_BLUE);
	for (i = 0; i < HTTP_TRYCNT; i++) {
		ret = OsHttpHandle(fullUrl, "POST", headers, NULL, bodyParamsStr, strlen(bodyParamsStr), &resp_content, &resp_content_len);
		API_LOG_DEBUG("app_dsp_cardPayRefund OsHttpHandle ret=%d,responseBuf(%d):%s\r\n", ret, resp_content_len, resp_content);
		if (ret == 0) {
			break;
		}
		if (resp_content) {
			OsSysFree(resp_content);
			resp_content = NULL;
		}
	}
	if (ret == 0) {
		respRoot = cJSON_Parse(resp_content);
		if (respRoot == NULL) {
			API_LOG_DEBUG("app_dsp_cardPay buf parse error\r\n");
			memcpy(msgOut, "server content error", strlen("server content error"));
			ret = -1;
			goto exit;
		}
		else {
			cJSON* respCode = cJSON_GetObjectItem(respRoot, "respCode");
			if (respCode) {
				if (strcmp(respCode->valuestring, "00")) {
					cJSON* respMsg = cJSON_GetObjectItem(respRoot, "respMsg");
					if (respMsg) {
						API_LOG_DEBUG("respMsg:%s\r\n", respMsg->valuestring);
						memcpy(msgOut, respMsg->valuestring, strlen(respMsg->valuestring));
						ret = -1;
						goto exit;
					}
				}
				else {
					memcpy(msgOut, "SUCCESS", strlen("SUCCESS"));
					ret = 0;
				}
			}
			else {
				ret = -1;
			}
		}
	}
	else {
		memcpy(msgOut, "server error", strlen("server error"));
	}
exit:
	if (headers) {
		OsSysFree(headers);
	}
	if (bodyParamsStr) {
		OsSysFree(bodyParamsStr);
	}
	if (root) {
		cJSON_Delete(root);
	}
	if (resp_content) {
		OsSysFree(resp_content);
	}
	if (respRoot) {
		cJSON_Delete(respRoot);
	}
	return ret;
}



int app_dsp_terminalInit(char* url,
	char* outdataKey, char* outpinKey,
	char* outmerchantId, char* outtid,
	char* outmerchantName, char* msgOut) {
	int ret = -1, i = 0;
	char* resp_content = NULL;
	int resp_content_len = 0;
	char* headers = (char*)OsSysMalloc(1024);
	memset(headers, 0, 1024);
	char fullUrl[128] = { 0 };
	char* bodyParamsStr = NULL;
	cJSON* root = NULL, * data = NULL;
	cJSON* respRoot = NULL;
	root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "deviceSn", serialID);
	sprintf(fullUrl, "%s/trade/terminalInitialization", url);
	bodyParamsStr = cJSON_PrintUnformatted(root);
	if (bodyParamsStr != NULL)
	{
		API_LOG_DEBUG("post data:\n%s\n", bodyParamsStr);
	}
	app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "Connecting To Server", LV_COLOR_DSP_BLUE);
	snprintf(headers, 1024, "Content-Type: application/json\r\nContent-Length: %lu\r\n", strlen(bodyParamsStr));
	for (i = 0; i < HTTP_TRYCNT; i++) {
		ret = OsHttpHandle(fullUrl, "POST", headers, NULL, bodyParamsStr, strlen(bodyParamsStr), &resp_content, &resp_content_len);
		API_LOG_DEBUG("app_dsp_terminalInit OsHttpHandle ret=%d,responseBuf(%d):%s\r\n", ret, resp_content_len, resp_content);
		if (ret == 0) {
			break;
		}
		if (resp_content) {
			OsSysFree(resp_content);
			resp_content = NULL;
		}
	}
	if (ret == 0) {
		respRoot = cJSON_Parse(resp_content);
		if (respRoot == NULL) {
			memcpy(msgOut, "get merchant error", strlen("get merchant error"));
			API_LOG_DEBUG("app_dsp_login buf parse error\r\n");
			ret = -1;

			goto exit;
		}
		cJSON* dataKey = cJSON_GetObjectItem(respRoot, "dataKey");
		if (dataKey) {
			memcpy(outdataKey, dataKey->valuestring, strlen(dataKey->valuestring));
		}
		cJSON* pinKey = cJSON_GetObjectItem(respRoot, "pinKey");
		if (pinKey) {
			memcpy(outpinKey, pinKey->valuestring, strlen(pinKey->valuestring));
		}
		cJSON* merchantId = cJSON_GetObjectItem(respRoot, "merchantId");
		if (merchantId) {
			memcpy(outmerchantId, merchantId->valuestring, strlen(merchantId->valuestring));
		}
		cJSON* tid = cJSON_GetObjectItem(respRoot, "tid");
		if (tid) {
			memcpy(outtid, tid->valuestring, strlen(tid->valuestring));
		}
		cJSON* merchantName = cJSON_GetObjectItem(respRoot, "merchantName");
		if (merchantName) {
			memcpy(outmerchantName, merchantName->valuestring, strlen(merchantName->valuestring));
		}
	}
	else {
		memcpy(msgOut, "get merchant info error", strlen("get merchant info error"));
		goto exit;
	}
	ret = 0;
exit:
	if (headers) {
		OsSysFree(headers);
	}

	if (bodyParamsStr) {
		OsSysFree(bodyParamsStr);
	}
	if (root) {
		cJSON_Delete(root);
	}
	if (resp_content) {
		OsSysFree(resp_content);
	}
	if (respRoot) {
		cJSON_Delete(respRoot);
	}
	API_LOG_DEBUG("app_dsp_terminalInit ret=%d\r\n", ret);
	return ret;
}



int app_dsp_terminalStorage(char* url, char* msgOut) {
	int ret = -1, i = 0;
	char* resp_content = NULL;
	int resp_content_len = 0;
	char* headers = (char*)OsSysMalloc(1024*2);
	memset(headers, 0, 1024);
	char fullUrl[128] = { 0 };
	char* bodyParamsStr = NULL;
	cJSON* root = NULL, * data = NULL;
	cJSON* respRoot = NULL;
	root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "deviceSn", serialID);
	sprintf(fullUrl, "%s/trade/terminalStorage", url);
	bodyParamsStr = cJSON_PrintUnformatted(root);
	if (bodyParamsStr != NULL)
	{
		API_LOG_DEBUG("post data:\n%s\n", bodyParamsStr);
	}
	snprintf(headers, 1024, "Content-Type: application/json\r\nContent-Length: %lu\r\n", strlen(bodyParamsStr));
	for (i = 0; i < HTTP_TRYCNT; i++) {
		ret = OsHttpHandle(fullUrl, "POST", headers, NULL, bodyParamsStr, strlen(bodyParamsStr), &resp_content, &resp_content_len);
		API_LOG_DEBUG("app_dsp_terminalStorage OsHttpHandle ret=%d,responseBuf(%d):%s\r\n", ret, resp_content_len, resp_content);
		if (ret == 0) {
			break;
		}
		if (resp_content) {
			OsSysFree(resp_content);
			resp_content = NULL;
		}
	}
	if (ret == 0) {
		respRoot = cJSON_Parse(resp_content);
		if (respRoot == NULL) {
			memcpy(msgOut, "get merchant error", strlen("get merchant error"));
			API_LOG_DEBUG("app_dsp_login buf parse error\r\n");
			ret = -1;

			goto exit;
		}
		cJSON* respCode = cJSON_GetObjectItem(respRoot, "respCode");
		if (respCode != NULL)  
		{
			if (strcmp(respCode->valuestring, "00"))
			{
				cJSON *respMsg = cJSON_GetObjectItem(respRoot, "respMsg");
				if (respMsg)
				{
					memcpy(msgOut, respMsg->valuestring, strlen(respMsg->valuestring));
				}
				ret = -1;
				goto exit;
			}
		}
	}
	else {
		memcpy(msgOut, "http error", strlen("http error"));
		goto exit;
	}
	ret = 0;
exit:
	if (headers) {
		OsSysFree(headers);
	}

	if (bodyParamsStr) {
		OsSysFree(bodyParamsStr);
	}
	if (root) {
		cJSON_Delete(root);
	}
	if (resp_content) {
		OsSysFree(resp_content);
	}
	if (respRoot) {
		cJSON_Delete(respRoot);
	}
	API_LOG_DEBUG("app_dsp_terminalInit ret=%d\r\n", ret);
	return ret;
}





int app_dsp_qrcodePayClose(char * mchntCd,char *deviceId,char* url,char * outTradeNo,char* msgOut) {
	DSP_MUTEX_LOCK;
	int ret = -1, i = 0;
	char* resp_content = NULL;
	int resp_content_len = 0;
	char* headers = (char*)OsSysMalloc(1024);
	memset(headers, 0, 1024);
	char fullUrl[128] = { 0 };
	char* bodyParamsStr = NULL;
	cJSON* root = NULL, * data = NULL;
	cJSON* respRoot = NULL;

	char traceNo[8] = { 0 };
	char batchNo[8] = { 0 };

	root = cJSON_CreateObject();
	data = cJSON_CreateObject();

	sprintf(traceNo, "%06d", gTraceNo);
	sprintf(batchNo, "%06d", gBatchNo);
	sprintf(m_tradeInfo.traceNo, "%06d", gTraceNo);
	sprintf(m_tradeInfo.batchNo, "%06d", gBatchNo);
	write_transaction_info();

	root = cJSON_CreateObject();
	data = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "sysId", "001");
	cJSON_AddStringToObject(root, "staffId", "0001");
	cJSON_AddStringToObject(root, "mchntCd", mchntCd);
	cJSON_AddStringToObject(root, "deviceSn", deviceId);

	cJSON_AddStringToObject(data, "version", "1.0.0");
	cJSON_AddStringToObject(data, "apiFlag", "1");
	cJSON_AddStringToObject(data, "transType", "2101");
	cJSON_AddStringToObject(data, "mchntCd", mchntCd);
	cJSON_AddStringToObject(data, "staffId", "0001");
	cJSON_AddStringToObject(data, "shopId", "");
	cJSON_AddStringToObject(data, "traceNo",traceNo);
	cJSON_AddStringToObject(data, "remark","");
	cJSON_AddStringToObject(data, "qrValidTime","110");
	cJSON_AddStringToObject(data, "batchNo",batchNo);
	cJSON_AddStringToObject(data, "outTradeNo", outTradeNo);
	cJSON_AddItemToObject(root, "data", data);

	sprintf(fullUrl, "%s/trade/qrcodePay", url);
	bodyParamsStr = cJSON_PrintUnformatted(root);
	if (bodyParamsStr != NULL)
	{
		API_LOG_DEBUG("app_dsp_qrcodePayClose post data:\n%s\n", bodyParamsStr);
	}
	snprintf(headers, 1024, "Content-Type: application/json\r\nContent-Length: %lu\r\n", strlen(bodyParamsStr));
	app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "Connecting To Server", LV_COLOR_DSP_BLUE);
	for (i = 0; i < HTTP_TRYCNT; i++) {
		ret = OsHttpHandle(fullUrl, "POST", headers, NULL, bodyParamsStr, strlen(bodyParamsStr), &resp_content, &resp_content_len);
		API_LOG_DEBUG("app_dsp_qrcodePayClose OsHttpHandle ret=%d,responseBuf(%d):%s\r\n", ret, resp_content_len, resp_content);
		if (ret == 0) {
			break;
		}
		if (resp_content) {
			OsSysFree(resp_content);
			resp_content = NULL;
		}
	}
	if (ret == 0) {
		respRoot = cJSON_Parse(resp_content);
		if (respRoot == NULL) {
			memcpy(msgOut, "qrcodepay close error", strlen("qrcodepay close error"));
			API_LOG_DEBUG("app_dsp_qrcodePayClose buf parse error\r\n");
			ret = -1;
			goto exit;
		}
		cJSON* respCode = cJSON_GetObjectItem(respRoot, "respCode");
		if (strcmp(respCode->valuestring, "00")) {
			cJSON* respMsg = cJSON_GetObjectItem(respRoot, "respMsg");
			if (respMsg) {
				memcpy(msgOut, respMsg->valuestring, strlen(respMsg->valuestring));
				API_LOG_DEBUG_HEX(respMsg->valuestring, strlen(respMsg->valuestring));
			}
			ret = -1;
			goto exit;
		}
	}
	else {
		memcpy(msgOut, "http error", strlen("http error"));
		goto exit;
	}
	ret = 0;
exit:
	if (headers) {
		OsSysFree(headers);
	}

	if (bodyParamsStr) {
		OsSysFree(bodyParamsStr);
	}
	if (root) {
		cJSON_Delete(root);
	}
	if (resp_content) {
		OsSysFree(resp_content);
	}
	if (respRoot) {
		cJSON_Delete(respRoot);
	}
	API_LOG_DEBUG("app_dsp_qrcodePayClose ret=%d\r\n", ret);
	DSP_MUTEX_UNLOCK;
	return ret;
}

