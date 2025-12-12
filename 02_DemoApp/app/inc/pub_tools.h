#include "api.h"

#define  KEY_NULL       KEYPAD_NO_KEY

#define  KEY_0          DIGITAL0
#define  KEY_1          DIGITAL1
#define  KEY_2          DIGITAL2
#define  KEY_3          DIGITAL3
#define  KEY_4          DIGITAL4
#define  KEY_5          DIGITAL5
#define  KEY_6          DIGITAL6
#define  KEY_7          DIGITAL7
#define  KEY_8          DIGITAL8
#define  KEY_9          DIGITAL9

#define	 KEY_F1			FUNCTION
#define  KEY_F2			PGUP
#define  KEY_F3			PGDOWN

#define  KEY_CANCEL     CANCEL
#define  KEY_DELETE     CLEAR
#define  KEY_ENTER      ENTER

#define  KEY_BACK       CLEAR 
#define  KEY_FUN        FUNCTION

#define  KEY_ESC        CANCEL
#define  KEY_DEL        KEY_DELETE
#define  KEY_POINT		KEY_F3

typedef enum {
	TT_CARDSALE = 0X01, //����
	TT_VOID = 0x02, //����
	TT_REFUND = 0x03, //�˻�
	TT_CHECKIN = 0x04, //ǩ��  
	TT_CHECKOUT = 0x05, //ǩ��
	TT_BATCHSETTLE = 0x06, //������
	TT_BATCHUPLOAD = 0x07, //������
	TT_PARAM = 0x08, //��������
	TT_STATUSUPLOAD = 0x09, //״̬����
	TT_ECHOTEST = 0x10, //�������
	TT_REVERSAL = 0x11,	//����
	TT_BALANCE = 0x12    //����ѯ

} STTRANSTYPE;

#define APP_ENTER   102  /*ȷ�ϼ��˳�*/
#define APP_EXIT    101  /*Ӧ�ó����˳�*/
#define APP_SUCC    (0) /*�ɹ�      */
#define APP_OK    (0) /*�ɹ�      */
#define APP_FAIL    -1  /*ʧ��      */
#define APP_QUIT    -2  /*�˳���ȡ��*/
#define APP_TIMEOUT -3  /*��ʱ      */
#define  APP_CANCEL -4 //ȡ��

#define CLRBUF(buf)  memset(buf, 0, sizeof(buf));

#define     _malloc     OsSysMalloc
#define     _free       OsSysFree
#define     _realloc    OsSysRealloc

int Util_Asc2Bcd(char* AscBuf, char* BcdBuf, int AscLen);
int Util_Bcd2Asc(char* BcdBuf, char* AscBuf, int AscLen);
int Util_Int2Bcd(unsigned int IntData, char* BcdBuf, int BcdLen);

#define SHOW_NORMAL  	0	
#define SHOW_REVERSE 	1  

#define ALIGN_LEFT			0
#define ALIGN_CENTER		1
#define	ALIGN_RIGHT			2

int Public_DispResp(char* pszRespCode, int TimeOut);
int Public_MsgBox(char* pszTitle, char* pszStr, int nTimeOut);
void Sys_Delay_ms(short time);


#define ASSERT_FAIL(e) \
{\
	int iRet=e;\
	api_log_output(1,"FILE=%s Line=%d  iRet=%d\n",__FILE__,__LINE__,iRet);\
	if (iRet != APP_OK)\
	{\
		return APP_FAIL;\
	}\
}
int DebugUtil_Config(unsigned char Switch, unsigned char Type);
/*==================================================
============================================================*/
int DebugUtil_WriteString(char* sFileName, int nLine, char* sFmt, ...);

/*==================================================
============================================================*/
int DebugUtil_WriteHex(char* sFileName, int nLine, const unsigned char* msg, unsigned int len);

/*==================================================

============================================================*/
void DebugUtil_Export(unsigned char Type);

/***********************************************

***********************************************/
void DebugUtil_ResetMemoryLog();

/***********************************************

***********************************************/
char* DebugUtil_ExportMemoryLog(int* buflen);

#define __FZ__	strlen(__FILE__)
//#ifndef _SDK_DEBUG
//#define _SDK_DEBUG
//#endif

#ifdef _SDK_DEBUG
#define __TRACEASC__(msg,...)  DebugUtil_WriteString( __FILE__+__FZ__-(__FZ__>15?15:__FZ__), __LINE__,msg,##__VA_ARGS__)
#define __TRACEHEX__(msg, len) DebugUtil_WriteHex(__FILE__+__FZ__-(__FZ__>15?15:__FZ__), __LINE__,msg,len)
#define __TRACEASCDATA__(msg,...)  DebugUtil_WriteString(NULL, 0,msg,##__VA_ARGS__)
#define __TRACEHEXDATA__(msg, len) DebugUtil_WriteHex(NULL, 0,msg,len)
#define __TRACEASCERROR__()  DebugUtil_WriteString( __FILE__+__FZ__-(__FZ__>15?15:__FZ__), __LINE__,NULL)

#else
#define __TRACEASC__(msg,...)
#define __TRACEHEX__(msg, len)
#define __TRACEASCDATA__(msg,...)
#define __TRACEHEXDATA__(msg, len)
#define __TRACEASCERROR__()
#endif

