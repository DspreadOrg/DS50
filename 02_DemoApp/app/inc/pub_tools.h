#include "api.h"

#include "ISO8583.h"


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
	TT_CARDSALE = 0X01, //消费
	TT_VOID = 0x02, //撤销
	TT_REFUND = 0x03, //退货
	TT_CHECKIN = 0x04, //签到  
	TT_CHECKOUT = 0x05, //签退
	TT_BATCHSETTLE = 0x06, //批结算
	TT_BATCHUPLOAD = 0x07, //批上送
	TT_PARAM = 0x08, //参数传递
	TT_STATUSUPLOAD = 0x09, //状态上送
	TT_ECHOTEST = 0x10, //回响测试
	TT_REVERSAL = 0x11,	//冲正
	TT_BALANCE = 0x12    //余额查询

} STTRANSTYPE;

#define APP_ENTER   102  /*确认键退出*/
#define APP_EXIT    101  /*应用程序退出*/
#define APP_SUCC    (0) /*成功      */
#define APP_OK    (0) /*成功      */
#define APP_FAIL    -1  /*失败      */
#define APP_QUIT    -2  /*退出、取消*/
#define APP_TIMEOUT -3  /*超时      */
#define  APP_CANCEL -4 //取消

#define CLRBUF(buf)  memset(buf, 0, sizeof(buf));

#define     _malloc     api_sys_malloc
#define     _free       api_sys_free
#define     _realloc    api_sys_realloc

int Util_Asc2Bcd(char* AscBuf, char* BcdBuf, int AscLen);
int Util_Bcd2Asc(char* BcdBuf, char* AscBuf, int AscLen);
int Util_Int2Bcd(unsigned int IntData, char* BcdBuf, int BcdLen);

//显示处理
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


/*----------------------------------------------------------------------------------------------------------------
=========================================================================================
========================================6.9.1.	LOG日志=======================================
=========================================================================================
-----------------------------------------------------------------------------------------------------------------*/
/*==================================================
函数名	DebugUtil_Config
函数原型	int DebugUtil_Config(unsigned char Switch,unsigned char Type)
函数描述	设置日志开关。
输入参数	Switch：开关状态（0关；非0开）
Type：日志输出类型（0串口输出；1存储在FLASH中(必须先调用DebugUtil_Reset来初始化FLASH大小)；2USB输出；3存于内存中最大4K）
若为串口输出，波特率为9600。
若为存储，存储量为10K，超出部分滚动覆盖起始部分。
输出参数	无
返回值	成功	RETURN_SUCC
失败	RETURN_FAIL
是否有函数预处理	无
函数实现中所调用的函数
============================================================*/
int DebugUtil_Config(unsigned char Switch, unsigned char Type);
/*==================================================
函数名	DebugUtil_WriteString
函数原型	int DebugUtil_WriteString(char *sFileName,int nLine,char *sFmt,...)
函数描述	字符串形式输出日志信息。（输出缓存为2K，单次输出总量需在2K内）
输入参数	sFileName：调用该函数的C文件名
nLine：调用该函数的行号
sFmt：日志信息（支持参数可变）(为空表示记录ERROR)
输出参数	无
返回值	成功	RETURN_SUCC
失败	RETURN_FAIL
是否有函数预处理	无
函数实现中所调用的函数	Sys_Uart_Send()，Sys_USB_SendData()，File_PutParam()
============================================================*/
int DebugUtil_WriteString(char* sFileName, int nLine, char* sFmt, ...);

/*==================================================
函数名	DebugUtil_WriteHex
函数原型	int DebugUtil_WriteHex(char *sFileName,int nLine,const unsigned char *msg,unsigned int len)
函数描述	HEX格式输出日志信息
输入参数	sFileName：调用该函数的C文件名
nLine：调用该函数的行号
mag：日志信息
len：日志有效信息长度
输出参数	无
返回值	成功	RETURN_SUCC
失败	RETURN_FAIL
是否有函数预处理	无
函数实现中所调用的函数	Sys_Uart_Send()，Sys_USB_SendData()，File_PutParam()
============================================================*/
int DebugUtil_WriteHex(char* sFileName, int nLine, const unsigned char* msg, unsigned int len);

/*==================================================
函数名	DebugUtil_Export
函数原型	int DebugUtil_Export(unsigned char Type)
函数描述	将FLASH中的日志导出。
输入参数	Type：日志输出类型（0串口输出； 2 USB输出；）
若为串口输出，波特率为115200
输出参数	无
返回值	成功	RETURN_SUCC
失败	RETURN_FAIL
是否有函数预处理	DebugUtil_Config()
函数实现中所调用的函数	Sys_Uart_Send()，Sys_USB_SendData()，File_GetParam()
============================================================*/
void DebugUtil_Export(unsigned char Type);

/***********************************************
功  能: 清空内存中的日志
参  数: 无
返回值：无
***********************************************/
void DebugUtil_ResetMemoryLog();

/***********************************************
功  能: 导出内存中的日志
参  数: buflen[OUT]：日志长度
返回值：日志指针，如果没有日志则返回NULL
***********************************************/
char* DebugUtil_ExportMemoryLog(int* buflen);

#define __FZ__	strlen(__FILE__)
//不需要时可去掉该宏定义
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

