#include "app_emqx.h"
#include "app_pub.h"
#include "cJSON.h"
#include "pub_tools.h"

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
        show_sale_menu();
    }    
	else if(step == 2)
    {
		emqt_mqtt_lock = true;
		Upub_CopyWithPoint(amt,dspAmt);
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
			ret =  OsMqttConnectEx(1,glb_sn, "q123b328.ala.cn-hangzhou.emqxsl.cn",8883, 0,			60, 4);
		}
		API_LOG_DEBUG("app_vietpay_mqttConnect reconnect ret:%d\r\n", ret);
		break;

	case MQTT_SIG_CONNECT_RSP:
		if (state->ret == 1) {
			sprintf(topicbuf, "%s/user/message",glb_sn);
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

int app_emqx_mqttConnect() {
	int ret = -1;
	get_sn(glb_sn);
	API_LOG_DEBUG("app_emqx_mqttConnect sn:%s\r\n", glb_sn);
	OsMqttSetStateHandleEx(1, emqx_state_fun);
	OsMqttSetUserNamePwdEx(1, glb_sn, glb_sn);
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
	ret = OsMqttConnectEx(1,glb_sn, "q123b328.ala.cn-hangzhou.emqxsl.cn",8883, 0,			60, 4);
			//OsMqttConnectEx(int id, char* clinetId, char* addr, unsigned short port, unsigned char cleansession,
	//unsigned short keepalive, unsigned char usetls)
	API_LOG_DEBUG("app_emqx_mqttConnect ret:%d\r\n", ret);
	return ret;
}

