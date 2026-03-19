#ifndef _APP_PARAM_H
#define _APP_PARAM_H
#define FILE_POSPARAM           "/ext/cust/posparam.ini"  
#define FILE_EMV_APP_PARAM           "/ext/cust/emv_app.bin"
#define FILE_EMV_CAPK_PARAM         "/ext/cust/emv_capk.bin"

#define APP_VERSION          "V1.0.4"                //LA-APP version number
#define DEVICE_MODEL          "DS50"
#define APP_VERSIONCODE      41 
#define MCHNTCD "974347473720001"
#define DEVICEID "00000138"
#define STATIC_QR_URL "https://www.dspread.com"
#define HTTP_TRYCNT 3

#define PIN_KEY_INDEX           10
#define DATA_KEY_INDEX         11

#define PIN_IPEK_INDEX 0x01
#define EMV_IPEK_INDEX 0x02
#define TRK_IPEK_INDEX 0x03

#define OFFLINE_TEST 0             // FOR OFFLINE TEST

#define CFG_APP_DEBUG_MODE          0    // if set 1 ,will update mksk key and dukpt key to device

typedef struct
{
    char mqtt_endpoint[128];
    char mqtt_port[8];
    char mqtt_auth_cert;
    char mqtt_username[64];
    char mqtt_password[64];
    char mqtt_topic[32];
    char pgw_endpoint[128];//api.payment.example.com",
    char pgw_port[8];
    char pgw_auth_type[8];
    char pgw_ssl_cert;
    char pgw_api_key[1024];
    char pgw_api_key_expiry[16];
    char pgw_api_sale[64];
    char pgw_api_details[64];
    char static_qr_url[256];
    char cust_name[64];
}POS_PARAM_ST;

typedef struct {
	char traceNo[8];//serial number
	char batchNo[8];//batch number
	char transStatus[32];
	char acqAuthCode[28];
	char transDatetime[28];
	char money[12];
	char cardNo[24];
    char cardOrg[32];
}dsp_tradeInfo_t;

int App_PosParamInit(void);

POS_PARAM_ST* App_GetPosParam(void);
 int write_transaction_info(void) ;
#endif