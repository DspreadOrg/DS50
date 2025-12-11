#ifndef __APP_CARD_PAYMENT_H__
#define __APP_CARD_PAYMENT_H__

enum EMVCHECKTYPE
{
 
	CHECK_INSERTIC      =   1,
 
	CHECK_STRIPE        =   2,
 
	CHECK_RFCARD        =   4,
 
	CHECK_KEYIN         =   8
};

//EMV Trans Result 
typedef enum
{ 
	ES_ONLINE_APPROVE 	= 1, //ONLINE accept
	ES_ONLINE_DENIAL 	= 2, //ONLINE denial
	ES_OFFLINE_APPROVE = 3, //OFFLINE accept
	ES_OFFLINE_DENIAL 	= 4 //OFFLINE denial
}AppEmvResult; 


typedef struct 
{ 
    
 	unsigned char  cEmvResult;          /*EMV Result, UmsEmvResult*/ 
	unsigned char  szPan[19+1];         /*PAN */
	unsigned char  sztrack2[38+1];      /*stack2 data*/
	unsigned char  szCardSerialNo[3+1]; /*23 �򣬿�Ƭ���к�*/ 
 	unsigned char  sField55[256];       /*IC����Ϣ,55��,(BIN)*/ 
 	int cFieldlen;           /*IC��Ϣ����,55����Ч����*/ 
 	unsigned char  sTC[8];        		/*IC������֤��(TCֵ)tag9f26,(BIN)*/ 
 	unsigned char  sARQC[8];            /*��Ȩ��������(ARQC),(BIN)*/ 
 	unsigned char  sARPC[8];            /*��Ȩ��Ӧ����(ARPC),(BIN)*/ 
 	unsigned char  sTVR[5];             /*�ն���֤���(TVR)ֵtag95,(BIN)*/ 
 	unsigned char  sAID[16+1];            /*Ӧ�ñ�ʶ��AID,(BCD)*/ 
 	unsigned char  szAppLable[16+1];    /*Ӧ�ñ�ǩ*/ 
	unsigned char  szAppName[16+1];     /*Ӧ����ѡ����*/ 
	unsigned char  sTSI[2];             /*����״̬��Ϣ(TSI)tag9B,(BIN)*/ 
 	unsigned char  sATC[2];             /*Ӧ�ý��׼�����(ATC)ֵtag9f36,(BIN)*/ 
	unsigned char  emvInfo[64];     /*card */
}APPEMVINFO; 


int Pub_EMVSetAid(const char *aids[]);
int Pub_EMVSetCapk(const char *capk[]);

int Pub_EMVInit(void);
int Pub_EMVOpenCard(unsigned char type);
int Pub_EMVCheckCard(unsigned char type);
int Pub_EMVPackTLVData(char *outData,unsigned int *TagList);
int Pub_EMVProcessApp(int card_read_method,char *cardNum,APPEMVINFO *emvInfo);
int Pub_EMVCompleteApp(bool status ,unsigned char *script,int issuAuthDatalen);

int Pub_EmvProcess(int card_read_method);
void EmvL2_SetDefaultAidAndCapk();
void EmvL2_Init();
#endif

