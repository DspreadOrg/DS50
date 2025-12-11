#include "pub_tools.h"
#define     _malloc     OsSysMalloc
#define     _free       OsSysFree
#define     _realloc    OsSysRealloc

/*******************************************************************************
*��������: IsAscForHex
*��������: �ж��ַ����Ƿ���HEX��
*����˵��:
	�������: pAsc  �ַ�����
			  nAscLen  �ַ����鳤��

*�� �� ֵ: 	���� 0   ȫ��0-f(F)������
			  ���� -1  �з�0-f(F)������
*��    ��: 13.08.01		�޸���:
*��    ��: ����
*******************************************************************************/
static int IsAscForHex(const char* pAsc, int nAscLen)
{
	int i;
	for (i = 0; i < nAscLen; i++)
	{
		if ((pAsc[i] >= '0' && pAsc[i] <= '9')
			|| (pAsc[i] >= 'a' && pAsc[i] <= 'f')
			|| (pAsc[i] >= 'A' && pAsc[i] <= 'F'))
		{
			continue;
		}
		return -1;
	}
	return 0;
}



/**
* @fn Util_Asc2Bcd
* @brief  ASCII��תBCD��
* @param in  AscBuf      �����ת����ASCII������
* @param in  AscLen       �����ASCII�����ݳ��ȣ���ת��BCD�����ݵ�˫������
* @param out  BcdBuf      ת�������BCD������
* @return
* @li  UUTIL_SUCCESS        =  0			�ɹ�
* @li  UUTIL_FAIL           	   = -1,       		ʧ��
* @����˵��: ��BCD�룬λ������󲹡�F��
*/
int Util_Asc2Bcd(char* AscBuf, char* BcdBuf, int AscLen)
{
	int i, nAscLen, len;
	const char* pAsc = AscBuf;
	unsigned char* pBcd = BcdBuf;

	if (pAsc == NULL || pBcd == NULL || AscLen < 0)
	{
		return APP_FAIL;
	}
	//��ȡASCII �ַ�������
	nAscLen = AscLen;

	for (i = 0; i < nAscLen; i++)
	{
		if ((pAsc[i] >= '0' && pAsc[i] <= '9')
			|| (pAsc[i] >= 'a' && pAsc[i] <= 'f')
			|| (pAsc[i] >= 'A' && pAsc[i] <= 'F'))
		{
			continue;
		}
		break;
	}

	if (i < nAscLen) nAscLen = i;

	if (IsAscForHex(pAsc, nAscLen) < 0)
	{
		return APP_FAIL;
	}

	len = nAscLen / 2;

	for (i = 0; i < len; i++)
	{
		pBcd[i] = ((pAsc[i * 2] <= 0x39) ? pAsc[i * 2] - 0x30 //��ֵΪ0~9
			: ((pAsc[i * 2] >= 0x61) ? pAsc[i * 2] - 0x20 - 0x41 + 10 //��ֵΪСд��ĸ
				: pAsc[i * 2] - 0x41 + 10)); //��ֵΪ��д��ĸ
		pBcd[i] = pBcd[i] << 4;


		pBcd[i] += ((pAsc[i * 2 + 1] <= 0x39) ? pAsc[i * 2 + 1] - 0x30//��ֵΪ0~9
			: ((pAsc[i * 2 + 1] >= 0x61) ? pAsc[i * 2 + 1] - 0x20 - 0x41 + 10 //��ֵΪСд��ĸ
				: pAsc[i * 2 + 1] - 0x41 + 10)); //��ֵΪ��д��ĸ
	}

	if (nAscLen % 2 != 0)
	{
		pBcd[i] = ((pAsc[i * 2] <= 0x39) ? pAsc[i * 2] - 0x30 //��ֵΪ0~9
			: ((pAsc[i * 2] >= 0x61) ? pAsc[i * 2] - 0x20 - 0x41 + 10 //��ֵΪСд��ĸ
				: pAsc[i * 2] - 0x41 + 10)); //��ֵΪ��д��ĸ
		pBcd[i] = pBcd[i] << 4;

		pBcd[i] |= 0x0F;

		i++;
	}
	//����ַ�������
	if (nAscLen < AscLen && nAscLen != 0)
	{
		memset(pBcd + i, 0xFF, AscLen - i);
	}

	return APP_SUCC;
}


/**
* @fn Util_Bcd2Asc
* @brief  BCD��תASCII��
* @param in  BcdBuf      �����ת����BCD������
* @param in  AscLen         ASCII�����ݳ��ȣ���BCD�����ݵ�˫������
* @param out  AscBuf      ת�������ASCII������
* @return
* @li  UUTIL_SUCCESS        =  0			�ɹ�
* @li  UUTIL_FAIL           	   = -1,       		ʧ��
*/
int Util_Bcd2Asc(char* BcdBuf, char* AscBuf, int AscLen)
{
	int i;
	char ch;
	const unsigned char* pBcd = BcdBuf;
	char* pAsc = AscBuf;

	//AscLen����Ϊ�����򱨴�
	if ((pAsc == NULL) || (pBcd == NULL) || (AscLen < 0))
		return APP_FAIL;

	for (i = 0; i < AscLen / 2; i++)
	{
		ch = (pBcd[i] & 0xF0) >> 4;
		pAsc[i * 2] = (ch > 9) ? ch + 0x41 - 10 : ch + 0x30;
		ch = pBcd[i] & 0x0F;
		pAsc[i * 2 + 1] = (ch > 9) ? ch + 0x41 - 10 : ch + 0x30;
	}
	if (AscLen % 2)//������Ϊ����ʱ�账��
	{
		ch = (pBcd[i] & 0xF0) >> 4;
		pAsc[i * 2] = (ch > 9) ? ch + 0x41 - 10 : ch + 0x30;
	}
	if (IsAscForHex(pAsc, AscLen) < 0)
	{
		return APP_FAIL;
	}

	return APP_SUCC;
}
/*******************************************************************************
*��������: Util_Int2Bcd
*��������: int������ת��ΪBCD��
		   �ҿ�BCD�룬λ��������0
*����˵��:
	�������: IntData:   ��ת����int������
			  BcdLen��   ת����BCD�����ݳ���

	  �������: BcdBuf��   ת����BCD����

*�� �� ֵ: UUTIL_FAIL           = -1,       //ʧ��
		   UUTIL_SUCCESS        =  0        //�ɹ�
*��ʷ��¼:
*��    ��: 13.08.01		�޸���:
*��    ��: ����
*******************************************************************************/
int Util_Int2Bcd(unsigned int IntData, char* BcdBuf, int BcdLen)
{
	int iRet;
	char temp[20];
	char* pBcdbuf;
	unsigned int bcdBufSize = BcdLen * 2;

	if (BcdBuf == NULL)
	{
		return APP_FAIL;
	}

	pBcdbuf = (char*)_malloc(bcdBufSize);
	if (pBcdbuf == NULL)
	{
		return APP_FAIL;
	}
	memset(pBcdbuf, '0', bcdBufSize);
	memset(temp, 0, sizeof(temp));
	sprintf(temp, "%u", IntData);
	if (bcdBufSize < strlen(temp))
	{
		free(pBcdbuf);
		return APP_FAIL;
	}
	memcpy(pBcdbuf + bcdBufSize - strlen(temp), temp, strlen(temp));
	iRet = Util_Asc2Bcd(pBcdbuf, (unsigned char*)BcdBuf, bcdBufSize);
	_free(pBcdbuf);

	if (iRet < 0)
	{
		return APP_FAIL;
	}
	return APP_SUCC;
}




int Sys_DisplayLine(unsigned char ucLine, unsigned char* strMsg, unsigned char ucReverse, unsigned char ucAlign);

unsigned char Sys_GetKey(int nTimeOut);

// ������
typedef struct
{
	unsigned char XCode;             // ������
	unsigned char XMsg1[10 + 1];   // ������1
	unsigned char XMsg2[40 + 1];   // ������2
} XCODE;

XCODE XCode[] =
{
	{ 0x00, "Transaction successful",				"Transaction successful" },//A
	{ 0x01, "Transaction failed",					"Please ask the cardholder to contact the issuing bank" },//C
	{ 0x03, "Transaction failed",					"Invalid merchant" },//C
	{ 0x04, "Transaction failed",					"This card has been confiscated" },//D
	{ 0x05, "Transaction failed",					"Cardholder authentication failed" },//C
	{ 0x06, "Transaction failed",					"Please contact the issuing bank" },
	{ 0x07, "Transaction failed",					"Confiscate card, please contact the acquiring bank" },
	{ 0x09, "Transaction failed",					"Please try again" },
	{ 0x10, "Transaction successful",				"Partial acceptance" },//A
	{ 0x11, "Transaction successful",				"Success, VIP customer"},//A
	{ 0x12, "Transaction failed",					"Invalid transaction" },//C
	{ 0x13, "Transaction failed",					"Invalid amount" },//B
	{ 0x14, "Transaction failed",					"Invalid card number" },//B
	{ 0x15, "Transaction failed",					"This card does not have a corresponding issuer" },//C
	{ 0x21, "Transaction failed",					"The card has not been initialized or is sleeping" },//C
	{ 0x22, "Transaction failed",					"Incorrect operation or exceeding the allowed trading days" },//C
	{ 0x25, "Transaction failed",					"No original transaction, please contact the issuing bank" },//C
	{ 0x30, "Transaction failed",					"Please try again" },//C
	{ 0x31, "Transaction failed",					"This card cannot be accepted" },
	{ 0x34, "Transaction failed",					"Cheating card, card retention" },//D
	{ 0x38, "Transaction failed",					"The number of password errors has exceeded the limit. Please contact the issuing party" },//D
	{ 0x40, "Transaction failed",					"Transaction types not supported by the issuing party" },//C
	{ 0x41, "Transaction failed",					"Report lost card, please confiscate (pos)" },//D
	{ 0x43, "Transaction failed",					"Stolen card, please confiscate it" },//D
	{ 0x51, "Transaction failed",					"Insufficient available balance" },//C
	{ 0x54, "Transaction failed",					"The card has expired" },//C
	{ 0x55, "Transaction failed",					"Password Error" },//C
	{ 0x57, "Transaction failed",					"This card transaction is not allowed" },//C
	{ 0x58, "Transaction failed",					"The issuing party does not allow the card to conduct this transaction on this terminal" },//C
	{ 0x59, "Transaction failed",					"Card verification error" },//C
	{ 0x61, "Transaction failed",					"Transaction amount exceeds the limit" },//C
	{ 0x62, "Transaction failed",					"Restricted card" },//C
	{ 0x64, "Transaction failed",					"The transaction amount does not match the original transaction" },//C
	{ 0x65, "Transaction failed",					"Exceeding the limit of consumption times" },//C
	{ 0x66, "Transaction failed",					"Please contact the acquiring bank or UnionPay" },
	{ 0x68, "Transaction failed",					"Transaction timeout, please try again" },//C
	{ 0x75, "Transaction failed",					"Password error count exceeds limit" },//C
	{ 0x77, "Transaction failed",					"Operator signs in again and makes the transaction again" },//C
	{ 0x90, "Transaction failed",					"System Error,Pls try again later" },//C
	{ 0x91, "Transaction failed",					"The issuer's status is abnormal, please try again later" },//C
	{ 0x92, "Transaction failed",					"The issuing party's line is abnormal, please try again later" },//C
	{ 0x94, "Transaction failed",					"Reject, duplicate transaction, please try again later" },//C
	{ 0x96, "Transaction failed",					"Rejected, exchange center exception, please try again later" },//C
	{ 0x97, "Transaction failed",					"Terminal not registered" },//D
	{ 0x98, "Transaction failed",					"Card issuer timeout" },//E
	{ 0x99, "Transaction failed",					"PIN format error, please check in again" },//B
	{ 0xA0, "Transaction failed",					"MAC format error, please check in again" },//B
	{ 0xA1, "Transaction failed",					"Inconsistent transfer currency" },//C
	{ 0xA2, "Transaction successful",				"Please confirm with the issuing bank" },//A14
	{ 0xA3, "Transaction failed",					"Account incorrect" },//B
	{ 0xA4, "Transaction successful",				"Please confirm with the issuing bank" },//A
	{ 0xA5, "Transaction successful",				"Please confirm with the issuing bank" },//A
	{ 0xA6, "Transaction successful",				"Please confirm with the issuing bank" },//A
	{ 0xA7, "Transaction failed",					"Rejected, exchange center exception, please try again later" },//C

	{ 0xFF, "", "" }
};
static int GetRespCode(unsigned char* pszRespCode, unsigned char* pszResult, unsigned char* Codeinfo)
{
	unsigned char respcode = 0;
	int i = 0;
	Util_Asc2Bcd(pszRespCode, &respcode, 2);

	while (XCode[i].XCode != 0xFF)
	{
		//__TRACEASC__("respcode=%02X", respcode);
		//__TRACEASC__("XCode=%02X", XCode[i].XCode);
		if (XCode[i].XCode != respcode)
		{
			i++;
			continue;
		}
		strcpy(pszResult, XCode[i].XMsg1);
		strcpy(Codeinfo, XCode[i].XMsg2);

		return APP_SUCC;
	}

	return APP_FAIL;
}

//Ӧ������ʾ����
int Public_DispResp(char* pszRespCode, int TimeOut)
{
	int ret;
	char szContent[256] = { 0 }, szCode[256] = { 0 };
	char szResult[32] = { 0 };
	if (strlen(pszRespCode) <= 0) return APP_FAIL;

	ret = GetRespCode(pszRespCode, szResult, szCode);
	if (ret == APP_FAIL)
	{
		strcpy(szResult, "Transaction failed");
		strcpy(szCode, "Unknown return code");
	}
	memset(szContent, 0, sizeof(szContent));
	sprintf(szContent, "%2.2s-%s", pszRespCode, szCode);
	// Public_MsgBox(szResult, szContent, TimeOut);
	app_lvgl_clearClientBg();
	app_lvgl_msgTips(szContent);
	Sys_Delay_ms(1000);
	return APP_SUCC;
}
static int TruncateChar(char* Src, int AvailableChar)
{
	int Len, i;

	Len = strlen(Src);
	for (i = 0; (i < AvailableChar) && (i < Len); i++)
	{
		if (Src[i] > 0x80)
		{
			i++;
			if ((i >= AvailableChar) || (i >= Len))
			{
				if (i >= Len)
				{
					Src[i - 1] = 0;
				}
				i--;
				break;
			}
		}
	}
	return i;
}



#define SHOWLEN 16

static int DispMessage(int nFirstLine, char* pszMsg)
{
	int len, j, i = 0, showlen = 0;

	char dispbuf[256];


	len = strlen(pszMsg);
	if (len <= 0)
	{
		return APP_SUCC;
	}
	len = strlen(pszMsg);
	while (1)
	{
		memset(dispbuf, 0, sizeof(dispbuf));
		int tLen = len - showlen;
		j = (SHOWLEN < tLen) ? SHOWLEN : tLen;
		j = TruncateChar(&pszMsg[showlen], j);
		strncpy(dispbuf, &pszMsg[showlen], j);

		//Sys_DisplayLine(nFirstLine + i, dispbuf, 0, ALIGN_CENTER);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips(dispbuf);
		i++;
		showlen += j;
		if (showlen >= len) break;
	}
	return APP_SUCC;
}

int Public_MsgBox(char* pszTitle, char* pszStr, int nTimeOut)
{
	api_lcd_clearScreen();

	//Sys_DisplayLine(2, pszTitle, 1, ALIGN_CENTER);
	DispMessage(2, pszStr);
	OsBeepPlay(1, 100, 0);

	Sys_GetKey(nTimeOut * 1000);

	api_lcd_clearScreen();
	return APP_SUCC;
}
void Sys_Delay_ms(short time);
unsigned char com_mode = 0;
//ͨѶ�ӿ�
int Public_CommConnect(int type)
{
	if (type == 0)
		OsUsbOpen();

	com_mode = type;
	return APP_OK;

}

/*******************************************************************
// ������         : Public_ConnectFlow
// ��������       : �������������ӡ����͡����ա��رսӿ�
// ����˵��       :
// ����ֵ         :  =0   �ɹ�
					<0   ʧ��
// ����           : 2018.5.17
// ��ע           : ��ѯʹ��
*******************************************************************/
//ֱ���޸�Ϊ�����ӿڣ����ڲ�ѯʹ��ʱ�����Ż�
int Public_ConnectFlow(unsigned char* pstr, int len, unsigned char* recvData)
{
	int ret = 0;
	int startTick;
	unsigned char buf[1026];
	if (com_mode == 0)//����ģʽ
	{

		buf[0] = 0x02;

		memcpy(buf + 1, pstr, len);
		buf[len + 1] = 0x03;
		buf[len + 2] = 0x00;
		ret = OsUsbWrite(buf, len + 3);
		//__TRACEHEX__(buf, len + 3);
	}
	else
	{
		ret = OsUsbWrite(pstr, len);
	}
	//__TRACEASC__("OsUsbWrite=%d", ret);

	if (ret < 0)
	{
		return ret;
	}
	memset(buf, 0, sizeof(buf));
	startTick = OsSysTick();
	while (1)
	{
		unsigned char key = app_getkey();
		if (key == KEY_CANCEL)
		{
			return -1;
		}
		Sys_Delay_ms(20);
		ret = OsUsbRead(buf, 1024);
		// __TRACEASC__("OsUsbRead=%d",ret);
		if (ret > 0)
		{
			break;
		}
		if (OsSysTick() - startTick > 30 * 1000)
		{
			return -2;
		}
	}
	if (com_mode == 0)//����ģʽ
	{
		memcpy(recvData, buf + 1, ret - 3);
		return ret - 3;
	}
	else
	{
		memcpy(recvData, buf, ret);
		return ret;
	}
}


int Public_CommHangUp(int Type)
{
	OsUsbClose();

	return APP_OK;
}
void PubAllTrim(char* pszStr)
{
	int nLen, i;
	nLen = strlen(pszStr);

	for (i = nLen - 1; i >= 0; i--)
	{
		if (pszStr[i] == ' ')
		{
			pszStr[i] = 0x00;
		}
	}
}

int PubAddSymbolToStr(char* pszStr, int nLen, char cChar, int nFlag)
{
	int nOriLen, i;
	char sBuff[128];

	memset(sBuff, 0, sizeof(sBuff));

	nOriLen = strlen(pszStr);
	if (nOriLen < nLen)
	{
		if (nFlag == 0)
		{
			for (i = 0; i < (nLen - nOriLen); i++)
			{
				sBuff[i] = cChar;
			}
			memcpy(sBuff + i, pszStr, nOriLen);
			memcpy(pszStr, sBuff, nLen);
		}
		else
		{
			for (i = nOriLen; i < nLen; i++)
			{
				pszStr[i] = cChar;
			}
		}
	}
	return APP_OK;
}

int PubAmtToDispOrPnt(const char* pszInAmt, char* pszOutAmt)
{
	int i;
	char szAmt[12 + 1] = { 0 };
	if (NULL == pszInAmt || NULL == pszOutAmt)
	{
		return APP_FAIL;
	}
	if (memcmp(pszInAmt, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 12) == 0)
	{
		return APP_FAIL;
	}

	memcpy(szAmt, pszInAmt, 12);
	PubAllTrim(szAmt);
	if (0 == strlen(szAmt))
	{
		return APP_FAIL;
	}

	for (i = 0; i < strlen(szAmt); i++)
	{
		if (!isdigit(szAmt[i]))
			return APP_FAIL;
	}

	memset(szAmt, 0, sizeof(szAmt));
	memcpy(szAmt, pszInAmt, 10);

	PubAllTrim(szAmt);

	if ((strlen(szAmt) > 9) && (szAmt[0] != '0'))  /**10λȫ���֣����ܳ���atol�����߽�*/
	{
		sprintf(pszOutAmt, "%s.%02d", szAmt, atoi(pszInAmt + 10));
	}
	else
	{
		sprintf(pszOutAmt, "%ld.%02d", atol(szAmt), atoi(pszInAmt + 10));
		PubAddSymbolToStr(pszOutAmt, 13, ' ', 0);
	}
	return APP_OK;
}
static int IsDigit(const char* pszText)
{
	int idx = 0;
	int iLen = strlen(pszText);
	for (idx = 0; idx < iLen; idx++)
	{
		if (!isdigit(pszText[idx])) return 0;
	}
	return 1;
}

static void PubTrimTailChars(char* pszString, char ChRemoveChar)
{
	int		i, iLen;

	if (!pszString || !*pszString)
	{
		return;
	}

	iLen = strlen((char*)pszString);
	for (i = iLen - 1; i >= 0; i--)
	{
		if (pszString[i] != ChRemoveChar)
		{
			break;
		}
		pszString[i] = 0;
	}
}

//��ʾ�����ݲ�������֧��������̻�����Ļ��ʾ �������һ���ֽ�Ϊ���Ŵ����з�����ʾ,����Ϊ��������ʾ
int Public_DispBalance(const char* pszBalance)
{
	int keyType = 0;
	int nRet;
	int nParamLen;
	char szTmpBuf[128], szBuf[128], * p;
	char szBuf2[128];
	if (pszBalance == NULL)
	{
		return APP_FAIL;
	}
	snprintf(szTmpBuf, 127, "%s", pszBalance);
	PubTrimTailChars(szTmpBuf, 0x20);
	if (szTmpBuf[0] == '+' || szTmpBuf[0] == '-')
	{
		snprintf(szBuf, 127, "%s", &szTmpBuf[1]);
	}
	else
	{
		snprintf(szBuf, 127, "%s", szTmpBuf);
	}
	p = strchr(szBuf, '.');
	//���ж�һ�����
	if (p != NULL && strlen(p + 1) == 2)
	{
		memset(szBuf2, 0, sizeof(szBuf2));
		memcpy(szBuf2, szBuf, p - szBuf);
		/*if(!IsDigit(szBuf2))
		{
		   return APP_FAIL;
		}*/
		if (!IsDigit(p + 1))
		{
			return APP_FAIL;
		}
	}
	else
	{
		return APP_FAIL;
	}
	api_lcd_clearScreen();
	{
		// Sys_DisplayLine(2, "��ѯ�ɹ�", 0, ALIGN_CENTER);
		// Sys_DisplayLine(3, "�������:", 0, ALIGN_LEFT);
		app_lvgl_clearClientBg();
		strcpy(szBuf,"available balance\n");
		strcat(	szBuf,szTmpBuf);
		app_lvgl_msgTips(szBuf);
		Sys_GetKey(5000);
	}

	return APP_OK;
}

