#ifndef _APP_PARAM_H
#define _APP_PARAM_H
#define FILE_POSPARAM           "/ext/posparam.ini"  
#define FILE_EMV_APP_PARAM           "/ext/emv_app.bin"
#define FILE_EMV_CAPK_PARAM         "/ext/emv_capk.bin"

#define APP_VERSION          "V1.0.0"                //LA-APP version number
#define APP_VERSIONCODE      41 
#define MCHNTCD "974347473720001"
#define DEVICEID "00000138"
#define DSP_URL "https://www.dspread.com"
#define HTTP_TRYCNT 3

#define CFG_APP_DEBUG_MODE          1    //1 DEBUG      0  PROD

typedef struct
{
    char mqtt_endpoint[128];
    char mqtt_port[8];
    char mqtt_auth_cert[8];
    char mqtt_username[64];
    char mqtt_password[64];
    char mqtt_topic_command[32];
    char mqtt_topic_response[32];
    char mqtt_topic_voice[32];
    char pgw_endpoint[128];//api.payment.example.com",
    char pgw_port[8];
    char pgw_auth_type[8];
    char pgw_ssl_cert;
    char pgw_api_key[1024];
    char pgw_api_key_expiry[16];
    char pgw_api_sale[32];
    char pgw_api_details[32];
}POS_PARAM_ST;

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

int App_PosParamInit(void);

POS_PARAM_ST* App_GetPosParam(void);
 int write_transaction_info(void) ;
#endif