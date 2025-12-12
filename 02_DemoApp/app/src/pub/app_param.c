
#include "SDK.h"
#include "app_param.h"
#include "cJSON.h"

static POS_PARAM_ST g_posParam ={0};
dsp_tradeInfo_t m_tradeInfo = { 0 };
POS_PARAM_ST* App_GetPosParam(void)
{
	return &g_posParam;
}
void App_PosParamProcess(char *pchParam)
{ 
	cJSON* root = NULL;
    cJSON* item = NULL;
	do
	{
		root = cJSON_Parse(pchParam);
		if (root == NULL)
			break;
		item = cJSON_GetObjectItem(root, "mqtt_endpoint");
		if(item)
			memcpy(g_posParam.mqtt_endpoint,item->valuestring,strlen(item->valuestring));
		item = cJSON_GetObjectItem(root, "mqtt_port");
		if(item)
			memcpy(g_posParam.mqtt_port,item->valuestring,strlen(item->valuestring));
		item = cJSON_GetObjectItem(root, "mqtt_auth_cert");
		if(item)
			memcpy(g_posParam.mqtt_auth_cert,item->valuestring,strlen(item->valuestring));
		item = cJSON_GetObjectItem(root, "mqtt_username");
		if(item)
			memcpy(g_posParam.mqtt_username,item->valuestring,strlen(item->valuestring));
		item = cJSON_GetObjectItem(root, "mqtt_password");
		if(item)
			memcpy(g_posParam.mqtt_password,item->valuestring,strlen(item->valuestring));
		item = cJSON_GetObjectItem(root, "mqtt_topic_command");
		if(item)
			memcpy(g_posParam.mqtt_topic_command,item->valuestring,strlen(item->valuestring));
		item = cJSON_GetObjectItem(root, "mqtt_topic_response");
		if(item)
			memcpy(g_posParam.mqtt_topic_response,item->valuestring,strlen(item->valuestring));
		item = cJSON_GetObjectItem(root, "mqtt_topic_voice");
		if(item)
			memcpy(g_posParam.mqtt_topic_voice,item->valuestring,strlen(item->valuestring));

		item = cJSON_GetObjectItem(root, "pgw_endpoint");
		if(item)
			memcpy(g_posParam.pgw_endpoint,item->valuestring,strlen(item->valuestring));
		item = cJSON_GetObjectItem(root, "pgw_port");
		if(item)
			memcpy(g_posParam.pgw_port,item->valuestring,strlen(item->valuestring));
		item = cJSON_GetObjectItem(root, "pgw_auth_type");
		if(item)
			memcpy(g_posParam.pgw_auth_type,item->valuestring,strlen(item->valuestring));
		item = cJSON_GetObjectItem(root, "pgw_ssl_cert");
		if(item)
			g_posParam.pgw_ssl_cert = item->valueint;
		item = cJSON_GetObjectItem(root, "pgw_api_key");
		if(item)
			memcpy(g_posParam.pgw_api_key,item->valuestring,strlen(item->valuestring));
		item = cJSON_GetObjectItem(root, "pgw_api_key_expiry");
		if(item)
			memcpy(g_posParam.pgw_api_key_expiry,item->valuestring,strlen(item->valuestring));
		item = cJSON_GetObjectItem(root, "pgw_api_sale");
		if(item)
			memcpy(g_posParam.pgw_api_sale,item->valuestring,strlen(item->valuestring));
		item = cJSON_GetObjectItem(root, "pgw_api_details");
		if(item)
			memcpy(g_posParam.pgw_api_details,item->valuestring,strlen(item->valuestring));
		/* code */
	} while (0);

	if (root)
		cJSON_Delete(root);
	
}

int write_transaction_info(void) {
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
int App_PosParamInit(void)
{	
	char *buff = NULL;
	int fd,ret;
	buff = OsSysMalloc(2048);
	memset(buff,0,2048);
	memset(&g_posParam,0,sizeof(POS_PARAM_ST));
	do
	{
		if(OsFileExist(FILE_POSPARAM) == 0)
		{
			API_LOG_DEBUG("App_PosParamInit: read posparam.ini not exist");
			break;
		}
		fd = OsFileOpen(FILE_POSPARAM, API_RDONLY);
		if(fd <= 0)
		{
			API_LOG_DEBUG("App_PosParamInit: read posparam.ini failed");
			break;
		}
		ret = OsFileRead(fd,buff,2048);
		if(ret <= 0)
		{
			API_LOG_DEBUG("App_PosParamInit: read posparam.ini ret =%d",ret);
			OsFileClose(fd);
			break;
		}
		API_LOG_DEBUG("App_PosParamInit: read posparam.ini success [%d]",ret);
		API_LOG_DEBUG("App_PosParamInit: read posparam.ini [%s]",buff);
		App_PosParamProcess(buff);
		OsFileClose(fd);	
	} while (0);

	OsSysFree(buff);

	API_LOG_DEBUG("g_posParam.mqtt_endpoint=[%s]",g_posParam.mqtt_endpoint);
	API_LOG_DEBUG("g_posParam.mqtt_port=[%s]",g_posParam.mqtt_port);
	API_LOG_DEBUG("g_posParam.mqtt_auth_cert=[%s]",g_posParam.mqtt_auth_cert);
	API_LOG_DEBUG("g_posParam.mqtt_username=[%s]",g_posParam.mqtt_username);
	API_LOG_DEBUG("g_posParam.mqtt_password=[%s]",g_posParam.mqtt_password);
	API_LOG_DEBUG("g_posParam.mqtt_topic_command=[%s]",g_posParam.mqtt_topic_command);
	API_LOG_DEBUG("g_posParam.mqtt_topic_response=[%s]",g_posParam.mqtt_topic_response);
	API_LOG_DEBUG("g_posParam.mqtt_topic_voice=[%s]",g_posParam.mqtt_topic_voice);
	API_LOG_DEBUG("g_posParam.pgw_endpoint=[%s]",g_posParam.pgw_endpoint);
	API_LOG_DEBUG("g_posParam.pgw_port=[%s]",g_posParam.pgw_port);
	API_LOG_DEBUG("g_posParam.pgw_auth_type=[%s]",g_posParam.pgw_auth_type);
	API_LOG_DEBUG("g_posParam.pgw_ssl_cert=[%d]",g_posParam.pgw_ssl_cert);
	API_LOG_DEBUG("g_posParam.pgw_api_key=[%s]",g_posParam.pgw_api_key);
	API_LOG_DEBUG("g_posParam.pgw_api_key_expiry=[%s]",g_posParam.pgw_api_key_expiry);
	API_LOG_DEBUG("g_posParam.pgw_api_sale=[%s]",g_posParam.pgw_api_sale);
	API_LOG_DEBUG("g_posParam.pgw_api_details=[%s]",g_posParam.pgw_api_details);

	if(-1 == read_transaction_info())
	{
		write_transaction_info();
	}

#if CFG_APP_DEBUG_MODE	

	int nRet;
	char buf[30+1]={0};
	unsigned char mainKey[32]="\x55\x55\x55\x55\x55\x55\x55\x55\x66\x66\x66\x66\x66\x66\x66\x66";
	unsigned char PINKey[32]="\x4A\x2D\xC7\x50\x9E\x20\xFD\x47\x60\xBF\x3F\xEE\xB8\xC8\xE6\x5F";
	unsigned char kcv[4]="\xBB\x02\x56\x9C";


	nRet= OsSecUpdatePlainKey(0, 0x01, 0, mainKey, 16);
	API_LOG_DEBUG("tms OsSecUpdatePlainKey=[%d]",nRet);

	nRet= OsSecUpdateCipherKey(0x10, 1, 1, 0, 10, PINKey, 16, 1, kcv, 4);

	API_LOG_DEBUG("tms OsSecUpdateCipherKey=[%d]",nRet);

	if(OsFileExist(FILE_EMV_APP_PARAM)!=1 || OsFileExist(FILE_EMV_CAPK_PARAM)!=1)
	{
		EmvL2_SetDefaultAidAndCapk();
	}
#endif	
	return 0;
}

