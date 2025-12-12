#include "app_emqx.h"
#include "app_pub.h"
#include "cJSON.h"
#include "pub_tools.h"
#include "app_param.h"
#include "dsp_emv.h"
#include "app_card_payment.h"
#include "app_audio.h"

char orgOrderId[32+1] = {0};
char orgOrderId_len = 0;

bool emqt_mqtt_lock = false;

static char glb_sn[32+1] = {0};
void emqx_param_init()
{

}

void get_sn(char *sn)
{
	char sys_sn[32+1] = {0};
	char cust_sn[32+1] = {0};
	int sys_sn_len = 0;
	int cust_sn_len = 0;
	if(OsSysReadSn(sys_sn,&sys_sn_len,cust_sn,&cust_sn_len) == 0)
	{
		sys_sn_len = strlen(sys_sn);
		memcpy(sn,sys_sn,sys_sn_len);
		API_LOG_DEBUG("app_emqx_mqttConnect sn:%s\r\n", sn);
		return;
	}
	else
	{
		API_LOG_DEBUG("app_emqx_mqttConnect sn get error\r\n");
	}	
}

//{ "message": "hello","amount":"12344","transType":"00","orderId":"12394555425" }    transType 00  sale  99 voice broker
static void appMqttMessageArrived(void*args)
{
    char amt[12+1] ={0};
	char dspAmt[12+1] = {0};
    int amtlen = 0;
    int step = -1;
    char msg[1024] = {0};
    mqtt_message_t*message=(mqtt_message_t*)args;
    memcpy(msg,message->message,message->message_len);
    API_LOG_DEBUG("message[%d]:%s\r\n",message->message_len,msg);
    cJSON *body = cJSON_Parse(msg);
    cJSON *item = NULL;
    do
    {
        if(body == NULL)
            break;

        item = cJSON_GetObjectItem(body, "orderId");
		if(item == NULL)
			break;

		if(orgOrderId_len != 0)
		{
			if(strlen(item->valuestring) == orgOrderId_len)
			{
				if(memcmp(item->valuestring,orgOrderId,orgOrderId_len) == 0)
				{
					break;
				}
			}
		}

		orgOrderId_len = strlen(item->valuestring);
		memcpy(orgOrderId,item->valuestring,orgOrderId_len);

        item = cJSON_GetObjectItem(body, "transType");
        if (memcmp(item->valuestring,"00",2) == 0)
        {
            item = cJSON_GetObjectItem(body, "amount");
            if(item != NULL)
            {
                amtlen = strlen(item->valuestring);
	            memcpy(amt, item->valuestring, amtlen);
                step = 1;
                break;
            }
        }
		else if(memcmp(item->valuestring,"99",2) == 0)
        {
            item = cJSON_GetObjectItem(body, "amount");
            if(item != NULL)
            {
                amtlen = strlen(item->valuestring);
	            memcpy(amt, item->valuestring, amtlen);
				step = 2;
				break;
            }
        }
        
        /* code */
    } while (0);

    if (body)
        cJSON_Delete(body);

	if(emqt_mqtt_lock)
		return;	
    if(step == 1)
    {
		emqt_mqtt_lock = true;
        Emqx_Sale(amt);
    }    
	else if(step == 2)
    {
		emqt_mqtt_lock = true;
		Pub_CopyWithPoint(amt,dspAmt);
		app_seg_showMoney(dspAmt);
		app_money_play(dspAmt);
    }

	emqt_mqtt_lock = false;
}
static void emqx_state_fun(void*args){
    int ret = 0;
	char topicbuf[256] = { 0 };
	mqtt_state_t* state = (mqtt_state_t*)args;
	API_LOG_DEBUG("app mqtt state %d:%d\r\n", state->type, state->ret);

	switch (state->type)
	{
	case MQTT_SIG_CLOSE_RSP:
		API_LOG_DEBUG("tms mqtt close\r\n");
		if (OsParamsGetNetMode() == 1) {//4g
			ret =  OsMqttConnectEx(1,glb_sn, App_GetPosParam()->mqtt_endpoint,atoi(App_GetPosParam()->mqtt_port), 0,			60, 4);
		}
		API_LOG_DEBUG("app_vietpay_mqttConnect reconnect ret:%d\r\n", ret);
		break;

	case MQTT_SIG_CONNECT_RSP:
		if (state->ret == 1) {
			sprintf(topicbuf, "%s/%s",App_GetPosParam()->mqtt_topic_command,glb_sn);
			API_LOG_DEBUG("topicbuf:%s\r\n",topicbuf);
			ret += OsMqttSubEx(1,topicbuf, 1, appMqttMessageArrived);
			API_LOG_DEBUG("OsMqttSub ret:%d\r\n");
			if (ret == 0) {
				state->ret = 1;
				state->type = 370;
				emqx_state_fun(state);
			}
		}
		else {
			API_LOG_DEBUG("app mqtt connect error\r\n");
		}
		break;
	case MQTT_SIG_SUB_RSP:
        break;
    }
 }

int app_emqx_service_start() {
	int ret = -1;
	get_sn(glb_sn);
	API_LOG_DEBUG("app_emqx_mqttConnect sn:%s\r\n", glb_sn);
	OsMqttSetStateHandleEx(1, emqx_state_fun);
	OsMqttSetUserNamePwdEx(1, App_GetPosParam()->mqtt_username, App_GetPosParam()->mqtt_password);
	// if (OsParamsGetNetMode() == 1) {//4g
	// 	OsWlSslSetFile("/ext/key/server.pem", "/ext/key/client.pem", "/ext/key/client.key");
	// }
	// else {
	// 	/*	ret=OsWifiLoadCrt(3, "/ext/key/server.pem", 3000);
	// 		API_LOG_DEBUG("OsWifiLoadCrt ret:%d\r\n",ret);*/
	// 	ret=OsWifiLoadCrt(4, "/ext/key/client.pem", 3000);
	// 	API_LOG_DEBUG("OsWifiLoadCrt ret:%d\r\n", ret);
	// 	ret = OsWifiLoadCrt(5, "/ext/key/client.key", 3000);
	// 	API_LOG_DEBUG("OsWifiLoadCrt ret:%d\r\n", ret);
	// }
	ret = OsMqttConnectEx(1,glb_sn, App_GetPosParam()->mqtt_endpoint,atoi(App_GetPosParam()->mqtt_port), 0,			60, 4);
			//OsMqttConnectEx(int id, char* clinetId, char* addr, unsigned short port, unsigned char cleansession,
	//unsigned short keepalive, unsigned char usetls)
	API_LOG_DEBUG("app_emqx_mqttConnect ret:%d\r\n", ret);
	return ret;
}

extern bool inputpinpassward;
extern bool offlinepinflag;
static int tradeType = -1;
extern char cardpayamount[64];
extern int card_read_method;
int Emqx_Sale(char *amt)
{
	int nRet;
	unsigned int sendbaglen;
	unsigned char sendbag[1024] = {0};
	char szParam[2 + 1];
	char szDate[14 + 1] = {0};
	char amount[12 + 1] = {0};
	char dspAmt[50 + 1] = {0};
	int initFlag,ret,i;
	int TransType=0;
	u32 startTick = 0;
	unsigned char recbag[1024] = {0};
	int recbaglen, nLen;
	char cardNum[19+1]={0};
    char pinBuf[32]={0};

	memcpy(cardpayamount, amt, strlen(amt));
	Pub_CopyWithPoint(amt,dspAmt);
	app_seg_showMoney(dspAmt);

	API_LOG_DEBUG("EMV_Process amt:%s\r\n", amt);
	OsSysAppLock();
	app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_qrcodescan.wav", "Please present your card", 0);
	app_lvgl_cardpayShow(dspAmt);
	{
		inputpinpassward = false;
		offlinepinflag = false;
		Pub_EMVOpenCard(CHECK_INSERTIC|CHECK_RFCARD);
	}
	startTick = OsSysTick() + 120000;
	do
	{
		for(i=0;i<1;i++)
		{
			card_read_method = Pub_EMVCheckCard(CHECK_INSERTIC | CHECK_RFCARD);
			if (card_read_method == 2 || card_read_method == 1)
			{
				if (card_read_method == 2)
				{
					tradeType = 2;
				}
				if (card_read_method == 1)
				{
					tradeType = 3;
				}
				if (card_read_method == 1)
					app_beep_play();
				ret = Pub_EmvProcess(card_read_method);
				if (ret == -8) {
					app_dsp_play(0, "/ext/audio/english/dsp_transactioncancel.wav", "Transaction Cancelled", 0);
				}
				goto exit;
			}
		}

		if (OsSysTick() > startTick)
		{
			ret = -1;
			goto exit;
		}
		OsSysMsleep(100);
	} while (1);
exit:
	API_LOG_DEBUG("app_trans_process ret:%d,tradeType:%d\r\n", ret, tradeType);
	if (ret == APP_RC_COMPLETED) {

		app_transaction_resultShow(tradeType, 1, dspAmt, "tradeNoStr", "outtradeNoStr");
		app_dsp_play(0,"/ext/audio/english/dsp_transactionsucc.wav","transaction success", 1);
	}
	else {
		app_transaction_resultShow(tradeType, 2, dspAmt, "tradeNoStr", "outtradeNoStr");
		app_dsp_play(0,"/ext/audio/english/dsp_transactionfail.wav","transaction fail", 1);
	}
	App_ICCardClose(IC_CARD_NO);
	App_PiccCardClose();

	OsSysMsleep(2000);
	OsSysAppUnLock();
	app_seg_showMoney("0.00");
	inputpinpassward = false;
	handle_menu(0);
	return nRet;
}
