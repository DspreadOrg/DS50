
#include "SDK.h"
//解包用到链表
//typedef struct _Node
//{
//	unsigned int nKey;
//	struct _Node* next;
//	unsigned char* strData;
//	unsigned short nDataLen;
//}*LNode, * LinkList;

/********************************************8583组包解包模块*******************************************************/
//域数据格式的类型，根据位判断
typedef enum
{
	ISO8583_ASC = 0x01,    //ASCII
	ISO8583_BCD = 0x02,    //BCD,支持0~F
	ISO8583_BIN = 0x03,    //Binary
	ISO8583_LEFT = 0x04,    //左对齐
	ISO8583_RIGHT = 0x05,    //右对齐
	ISO8583_FIX = 0x06,    //定长
	ISO8583_LLVAR = 0x07,    //两位变长
	ISO8583_LLLVAR = 0x08     //三位变长
} ISO8583_DataType;

//ISO8583域属性定义
typedef struct
{
	unsigned char  uiId;			//域号
	char          cDataPad;			//数据补齐字符
	char          cLengthPad;		//长度补齐字符
	// 数据信息，高4位表示数据对齐类型(uiDataAlign): ISO8583_LEFT\ISO8583_RIGHT，低4位表示数据格式(uiDataFormat):ISO8583_ASC\ISO8583_BCD\ISO8583_BIN
	unsigned char uiDataInfo;
	// 长度信息，字节1高4位始终为0，字节1低4位表示长度类型(uiLengthType): ISO8583_FIX\ISO8583_LLVAR\ISO8583_LLLVAR
	//           字节2高4位表示长度对齐类型(uiLengthAlign): ISO8583_LEFT\ISO8583_RIGHT，字节2低4位表示长度格式(uiLengthFormat):ISO8583_ASC\ISO8583_BCD\ISO8583_BIN
	unsigned short uiLengthInfo;
	unsigned short uiMaxLen;			//数据域最大长度
} ISO8583_FieldType;



//8583交易结构体STSYSTEM，具体参考银联规范文档
typedef struct
{
	char sLen[2 + 1]; 				/*数据元长度*/
	char sIisCode[11 + 1];			/*发卡行标识码 (返回包时用) */
	char sAcqCode[11 + 1];			/*收单机构标识码 (返回包时用) */
}STFIELD44;

typedef struct
{
	char sFuncCode[2 + 1];			/*60.1 交易类型码*/
	char sBatchNum[6 + 1];			/*60.2 批次号*/
	char sNetManCode[3 + 1];			/*60.3 网络管理信息*/
	char cTermCapacity; 			/*60.4 终端处理能力*/
	char cIcConditionCode;			/*60.5 基于PBOC借/贷记标准的IC卡条件代码*/
}STFIELD60;

typedef struct
{
	char sOrigBatchNum[6 + 1];		/*60.1 原交易批次号*/
	char szOrigTrace[6 + 1];			/*60.2 原交易流水号*/
	char szOrigDate[4 + 1];			/*60.3 原交易日期*/
}STFIELD61;

typedef struct
{
	char sField63_1[3]; 			/*63.1*/
	char sIisInfo[20];				/*63.2.1 发卡行信息 */
	char sCUP_Info[20]; 			/*63.2.2 中国银联信息 */
	char sMerAcqInfo[20];			/*63.2.3 商户收单信息*/
	char sTermInfo[60]; 			/*63.2.4 POS终端特殊交易信息*/
	char sRfu[3];
}STFIELD63;

/*
@struct STSYSTEM交易数据结构
*/

typedef struct
{
	/* 以下为标准交易数据元内容 */
	char szMsgID[4 + 1];				/*消息类型*/
	char sBitMap[16 + 1]; 			/*位图*/
	char szPan[19 + 1];				/*2 主帐号*/
	char sProcCode[6 + 1];			/*3 交易处理码*/
	char szAmount[12 + 1];			/*4 交易金额(BCD)*/
	char szTrace[6 + 1];				/*11 POS流水号*/
	char szTime[6 + 1];				/*12 交易时间hhmmss */
	char szDate[4 + 1];				/*13 交易日期mmdd  */
	char szExpDate[4 + 1];			/*14 卡有效期 */
	char szSettleDate[4 + 1]; 		/*15 清算日期(返回包时用) */
	char szInputMode[3 + 1];			/*22 输入方式码*/
	char szCardSerialNo[3 + 1];		/*23 卡片序列号*/
	char szServerCode[2 + 1]; 		/*25 服务点条件码*/
	char szPINCode[2 + 1];			/*26 PIN获取码*/
	char szAcqCenterCode[11 + 1]; 	/*32 受理方标识码,pos中心号(返回包时用) */
	char szTrack2[37 + 1];			/*35 二磁道数据*/
	char szTrack3[104 + 1];			/*36 三磁道数据*/
	char szRefnum[12 + 1];			/*37 系统参考号(返回包时用)  */
	char szAuthCode[6 + 1];			/*38 授权码*/
	char szResponse[2 + 1];			/*39 应答码*/
	char szPosID[8 + 1];				/*41 终端号 */
	char szShopID[15 + 1];			/*42 商户号 */
	char szMoneyCode[3 + 1];			/*49 交易货币代码*/
	char szPin[8 + 1];				/*52 个人标识码(密文)*/
	char szSRCInfo[16 + 1]; 		/*53 安全控制信息*/
	char szAddAmt[20 + 1];			/*54 附加金额*/
	char cMustChkMAC;				/*64 返回包*/
	char sField62[600]; 			/** 持卡人身份认证信息*/
	char szInterOrg[3 + 1];			/**<国际组织代码(返回包时用)  */

	STFIELD44 stField44;			/*44 附加响应数据 */
	STFIELD60 stField60;			/*60 自定义域*/
	STFIELD61 stField61;			/*61 自定义域*/
	STFIELD63 stField63;			/*63 自定义域*/

	//以下为附加的交易数据元内容
	char* psAddField;				/*附加域指针如55域内容,空间由具体应用里开辟*/
	int nAddFieldLen;				/*附加域数据长度*/

	char cTransType;				//交易类型
}STSYSTEM;


/**************************************************
功能：8583解包时计算MAC的函数指针
参数说明：data     [in]：    待计算MAC的数据
dataLen  [in]:     数据长度
mac      [out]:    8字节MAC
返回值：RETURN_SUCC     成功
RETURN_FAIL     失败
*************************************************/
typedef int (*ISO8583_CalcMAC) (unsigned char* data, int dataLen, unsigned char* mac);

void* ISO8583_Init(void);
void ISO8583_Free(void** ptr);
int ISO8583_UnPackData(const ISO8583_FieldType* pISO_8583, void* ptr, char* pData, int nLen, ISO8583_CalcMAC CALLBACK_8583CaclMAC);
LNode ISO8583_GetField(void* ptr, unsigned int uiId);
int ISO8583_SetField(ISO8583_FieldType* pISO_8583, void* ptr, unsigned int uiId, const unsigned char* pucData, unsigned int uiLen);
int ISO8583_ClearField(ISO8583_FieldType* pISO_8583, void* ptr, unsigned int uiId);
int ISO8583_PackData(ISO8583_FieldType* pISO_8583, void* ptr, unsigned char* pucISOData);
int ISO8583_GetPackLen(void* ptr);
int Uapp_Sale(void);

