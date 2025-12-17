#ifndef __APP_CARD_PAYMENT_H__
#define __APP_CARD_PAYMENT_H__

enum EMVCHECKTYPE
{
	CHECK_INSERTIC      =   1,
	CHECK_STRIPE        =   2,
	CHECK_RFCARD        =   4,
	CHECK_KEYIN         =   8
};

typedef struct 
{ 
    
 	unsigned char  cEmvResult;          /*EMV Result, UmsEmvResult*/ 
	unsigned char  szPan[19+1];         /*PAN */
	unsigned char  sztrack2[64+1];      /*stack2 data*/
	int track2len;
	unsigned char  szCardSerialNo[3+1]; /*23*/ 
 	unsigned char  sField55[256];       /*IC,55,(BIN)*/ 
 	int cFieldlen;           			/*IC,55 length*/ 
 	unsigned char  sTC[8];        		/*IC(TCֵ)tag9f26,(BIN)*/ 
 	unsigned char  sARQC[8];            /*(ARQC),(BIN)*/ 
 	unsigned char  sARPC[8];            /*(ARPC),(BIN)*/ 
 	unsigned char  sTVR[5];             /*(TVR)ֵtag95,(BIN)*/ 
 	unsigned char  sAID[16+1];            /*AID,(BCD)*/ 
 	unsigned char  szAppLable[16+1];    /**/ 
	unsigned char  szAppName[16+1];     /**/ 
	unsigned char  sTSI[2];             /*(TSI)tag9B,(BIN)*/ 
 	unsigned char  sATC[2];             /*(ATC)ֵtag9f36,(BIN)*/ 
	unsigned char  emvInfo[64];     /*card */
	unsigned char cardOrg[32];
	unsigned char szPinBlock[16+1]; /*PIN block*/
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

