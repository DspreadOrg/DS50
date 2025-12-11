#ifndef _DDI_WIFI_H
#define _DDI_WIFI_H
#include "api_types.h"


#pragma pack(1)
typedef struct
{
	char cSsid[64]; 		 
	char cBssid[20];	 
	int iChannel;		 
	int iRssi;			 
}ST_AP_INFO; 
#define CRT_SIZE 		4*1024//

typedef struct
{
	ST_AP_INFO stApinfo;		 
	int apErrFlag;
}ST_AP_INFO_ASY; 



typedef struct
{
	char cBssid[5][20];	 
	int iMacCnt;
}ST_AP_MAC; 

typedef struct
{
	char ssid[64]; 
	char pwd[64]; 
	int chl; 
	int ecn; 
	int max_conn; 
	int ssid_hidden; 

}APINFO; 
#pragma pack()


#pragma pack(1)

typedef struct
{
	int iDHCPEnable;	 
	char cIp[20]; 			 
	char cNetMask[20];		 
	char cGateWay[20];		 
  
}ST_WIFI_PARAM; 


typedef struct
{
	int iEcn;			 
	char cSsid[64]; 	 
	int iRssi;			 
	char cBssid[20];	 
	int iChannel;		 
	int iFreqOffset;	 
}ST_AP_LIST; 

#pragma pack()

typedef enum
{
	eCmdDomainToIP = 0,			 
	eCmdNetTime,
}ENUM_WIFICtrlCmd;

struct WIFI_STRUCT
{
	int cOpenState;			 
	int cApClose;
};

typedef enum {
	AP_AIRKISS_ID = 1,
	AP_WEB_ID,
	AP_BT_ID,

}APCTYPE;


int OsWifiInit();
int OsWifiOpen();
int OsWifiGetWebStatus();

int OsWifiClose();
int OsWifiWake(BOOL wkup_V) ;  
s32 OsWifiGetConnectStatus(void);
int OsEspCheckApAsync(ST_AP_INFO *pstApInfo);
int OsEspNTPSystime(char *pcNTP, int iGmTz);
s32 OsWifiGetScanResults (ST_AP_LIST *pstApList, int iApCount) ;
s32 OsWifiIoctl(u32 nCmd, u32 lParam, u32 wParam) ;
s32 OsWifiConnectHotSpot(unsigned char *pucSsid, unsigned char *pucPassword) ;
s32 OsWifiDisConnectHotSpot(void) ;
s32 OsWifiConnectParamConfig(ST_WIFI_PARAM *pstWifiParam);
s32 OsWifiConnectParamGet(ST_WIFI_PARAM *pstWifiParam);
int OsWifiGetConnectedHotSpotInfo(ST_AP_INFO *pstApInfo);

int OsWifiSocketCreate(int type);
int OsWifiSocketClose(int sockid);
int OsWifiTcpConnect(int sockid,char *serveraddr,char *port,int timeout);
int OsWifiTcpClose(int sockid);
int OsWifiSend(int sockid,char *data, int datalen, int timeout);
int OsWifiRecv(int sockid, unsigned char *pucdata, unsigned short iLen, unsigned int uiTimeOut);
/*
* Description: The certificate is downloaded, and it is recommended to restart it after the download is complete. You need to configure SSL before connecting to SSL
* Input: cType:0??client_ca?? 1??client_cert?? 2??client_key
		cType:3??mqtt_ca?? 4??mqtt_cert?? 5??mqtt_key
		pchFileName: the full path of the certificate. Max_size: The maximum file size
* Output:N
* Return: <0:
*/

int OsWifiLoadCrt(char cType, char* pchFileName, int Max_size);

int OsEspSetSslConfig(int i32Sockid, int mode);
int OsWifiSslSocketCreate(void);
int OsWifiSslSocketClose(int sockid);
int OsWifiSslConnect(int sockid,char *serveraddr,char *port,int timeout);
int OsWifiSslClose(int sockid);
int OsWifiSslSend(int sockid, char *data, int datalen, int timeout);
int OsWifiSslRecv(int sockid, unsigned char *pucdata, unsigned short iLen, unsigned int uiTimeOut);

int OsWifiGetMac(char *pcMacBuf);
int OsWifiSetMac(char *pcMacBuf);

int OsWifiSntpCfg( int enable,int timezone, unsigned char* SNTP1,unsigned char* SNTP2,unsigned char* SNTP3);
int OsWifiUptime(void);
int OsWifiWebNetConnect(unsigned char* pui8Ssid, unsigned char* pui8Password, unsigned int ui32Timeout);
int OsWifiUserRota(char* URL, int URLlen, int i32Timeout);
int OsWifiGetVersion(char* buf, int buflen);
int OsWifiWebNetworkClose(void);
int OsWifiUpdate(const char* filename);
int OsWifiAutoConnect(bool mode);

#endif
