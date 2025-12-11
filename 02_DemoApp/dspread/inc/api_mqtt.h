#ifndef __API_MQTT_H
#define __API_MQTT_H

#include "api_types.h"

enum {
	MQTT_SIG_CONNECT_RSP = 368,
	MQTT_SIG_CLOSE_RSP = 369,
	MQTT_SIG_SUB_RSP = 370,
	MQTT_SIG_UNSUB_RSP = 371,
	MQTT_SIG_PUB_RSP = 372,
};

typedef struct mqtt_message_t {
	int linkId;
	char* topic;
	int qos;
	char* message;
	int message_len;
}mqtt_message_t;

typedef struct mqtt_state_t {
	int linkId;
	int type;
	int ret;
}mqtt_state_t;

int OsMqttSetUserNamePwd(const char* usrname, const char* pwd);
void OsMqttSetSslMode(int mode);
int OsMqttSetPsk(const char* psk_id, const char* psk_key);
int OsMqttSetTlsVer(int ver);
int OsMqttSetWill(char* topic, unsigned char qos, bool retain, char* message);
int OsMqttConnect(char* clinetId, char* addr, unsigned short port, unsigned char cleansession,
	unsigned short keepalive, unsigned char usetls);
int OsMqttClose();
int OsMqttSub(char* topic, unsigned char qos, void (*message)(void* args));
int OsMqttUnSub(char* topic);
int OsMqttPub(char* topic, unsigned char qos, bool retain, char* message, unsigned short messlen);
int OsMqttSetStateHandle(void (*message)(void* args));

int OsMqttSetUserNamePwdEx(int id, const char* usrname, const char* pwd);
int OsMqttSetWillEx(int id, char* topic, unsigned char qos, bool retain, char* message);



/**************************************************
 *function:
 *input:
			  usetls:
				1: MQTT over TCP��
				2: MQTT over TLS����У��֤�飩��
				3: MQTT over TLS��У�� server ֤�飩��
				4: MQTT over TLS���ṩ client ֤�飩��
				5: MQTT over TLS��У�� server ֤�鲢���ṩ client ֤�飩��
 *output:
 *return:
**************************************************/
int OsMqttConnectEx(int id, char* clinetId, char* addr, unsigned short port, unsigned char cleansession,
	unsigned short keepalive, unsigned char usetls);
int OsMqttCloseEx(int id);
int OsMqttSubEx(int id, char* topic, unsigned char qos, void (*message)(void* args));
int OsMqttUnSubEx(int id, char* topic);
int OsMqttPubEx(int id, char* topic, unsigned char qos, bool retain, char* message, unsigned short messlen);
int OsMqttSetStateHandleEx(int id, void (*message)(void* args));
 
/**************************************************
 *function: 
 *input:        
              connect_type:
				1: MQTT over TCP��
				2: MQTT over TLS����У��֤�飩��
				3: MQTT over TLS��У�� server ֤�飩��
				4: MQTT over TLS���ṩ client ֤�飩��
				5: MQTT over TLS��У�� server ֤�鲢���ṩ client ֤�飩��
 *output:
 *return:
**************************************************/
int OsMqttWifiSetUserCfg(int connect_type, char* clinetId, char* usrname, char* pwd);

/**************************************************
 *function: 
 *input: cb:mqtt state callback
 *output:
 *return:
**************************************************/
int OsMqttWifiConn(char* host, int port, unsigned char cleansession,
	char* willTopic, char* willMsg, char willQos, int keepalive, void (*cb)(void* args));

/**************************************************
 *function: 
 *input: message:mqtt message callback
 *output:
 *return:
**************************************************/
int OsMqttWifiSub(char* topic, char qos, void (*message)(void* args));
int OsMqttWifiPub(char* topic, char* msg, char qos, char retain);
int OsMqttWifiClose();

#endif