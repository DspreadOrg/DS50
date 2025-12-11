#ifndef __EMQX_H
#define __EMQX_H

typedef struct emqx_mqtt_param_t {
	char username[64];
    char password[64];
    char clientId[64];
    char url[128];
    int port;
	char topic[64];
}emqx_mqtt_param_t;

int app_emqx_mqttConnect();
int Emqx_Sale(char *emqx_amt);
void  show_sale_menu();
#endif