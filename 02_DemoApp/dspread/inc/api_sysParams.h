#ifndef _API_SYSPARAMS_H
#define _API_SYSPARAMS_H

enum
{
	WIFI_SOCKET_MODE = 0,
	WIRELESS_SOCKET_MODE = 1,

}NET_MODE;


int OsParamsSetNetMode(int value);
int OsParamsGetNetMode();
int OsParamsGetSecMode();

#endif