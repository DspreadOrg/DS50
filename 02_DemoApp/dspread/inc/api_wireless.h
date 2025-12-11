#ifndef _API_WIRELESS_H
#define _API_WIRELESS_H

typedef struct {
	unsigned char* apn;
	unsigned char* usrname;
	unsigned char* pwd;
}pdp_config_t;


typedef struct cell_info_t{
	int mcc;
	int mnc;
	int cell_id;
	int tac;
}cell_info_t;

typedef struct lte_gsm_nb_info_t {
	int lteNcellNum;
	cell_info_t lte_info[10];
	int gsmNcellNum;
	cell_info_t gsm_info[10];
}lte_gsm_nb_info_t;


void OsWlSetPdpConfig(char* apn, char* usrname, char* pwd);

 
int OsWlPdpActive(int simid);

 
int OsWlPdpDeactive(int simid);

 
int OsWlPdpGetStatus(int simid);

lte_gsm_nb_info_t* OsWlGetNbCell();


int OsWlGetNetMode();

int OsWlGetIp(char* ipv4, char* ipv6);
 
int OsWlGetMncMcc(int* mcc, int* mnc);

int OsWlGetLacInfo(int* gsm_lac, int* lte_lac);

int OsWlPing(int type, char* host, int ping_cnt, void(*fun)(int, char*));

 
int OsWlNtp(char* pucIP, unsigned int uiPort, unsigned int timeout);

int OsWlGetCellId(int* gsm_cell_id, int* lte_cell_id);

int OsWlGetSimStatus(unsigned char* status);
int OsWlGetCcid(unsigned char* ccid);

int OsWlGetCsq(int* rssi, int* ber);

int OsWlGetImeiImsi(unsigned char* imei, unsigned char* imsi);
void OsWlSetNonBlocking(int sockfd);
void OsWlSocketSetBlocking(int sockfd);
int OsWlSocketCreate(int net_type);
int OsWlSocketClose(int sockid);
int OsWlSocketConnect(int nettype, int sockid, char* pIP, char * pPort);
int OsWlSocketSend(int sockid, unsigned char* pucdata, unsigned int iLen);
int OsWlSocketRecv(int sockid, unsigned char* pucdata, unsigned short iLen);
int OsWlSocketRecvTimeout(int sockid, unsigned char* pucdata, unsigned short iLen, unsigned int uiTimeOut);
int OsWlGetHostName(char* Domainname, char* ipv4_addr, char* ipv6_addr);
int OsWlSslSetTlsVer(int ver);
void OsWlSslSocketSetCheckMode(int mode);
int OsWlSslSocketRecvTimeout(int ssl_fd, char* recvdata, unsigned short recvlen, unsigned int timeout);
int OsWlSslSocketRecv(int ssl_fd, char* recvdata, unsigned short recvlen);
int OsWlSslSocketSend(int ssl_fd, char* senddata, unsigned short sendlen);
int OsWlSslSocketConnect(int ssl_fd, char* pIp, char* pPort);
int OsWlSslSocketClose(int ssl_fd);
int OsWlSslSocketCreate();
int OsWlSslGetErrorCode();
int OsWlSslSetFile(char* cacert, char* clientcert, char* clientkey);
int OsWlGetNetState();
int OsWlGetOperatoName(char* operatorname, int bufsize);




#endif