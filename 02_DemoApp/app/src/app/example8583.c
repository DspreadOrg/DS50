#include "pub_tools.h"
#include "app_card_payment.h"
#include "app_dsp.h"

extern int card_read_method;
extern char cardpayamount[64];

char saveAmount[12 + 1]; // ȫ�֣�������

int cancelFlag = 0;		 // ȫ�֣�0:���ѹ�����δ��KEY_CANCEL

// �����淶��8583����
const ISO8583_FieldType sg_IsoCfg[] =
	{
		{0, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BCD, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 4},		// MSGID
		{1, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BIN, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 8},		// BITMAP
		{2, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BCD, (ISO8583_LLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 21},	// ���˺� ����
		{3, '0', '0', (ISO8583_RIGHT << 4) + ISO8583_BCD, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 6},		// ���״�����
		{4, '0', '0', (ISO8583_RIGHT << 4) + ISO8583_BCD, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 12},		// ���׽��
		{11, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BCD, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 6},		// �ܿ���ϵͳ���ٺ�
		{12, '0', '0', (ISO8583_RIGHT << 4) + ISO8583_BCD, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 6},		// �ܿ������ڵ�ʱ��
		{13, '0', '0', (ISO8583_RIGHT << 4) + ISO8583_BCD, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 4},		// �ܿ������ڵ�����
		{14, '0', '0', (ISO8583_RIGHT << 4) + ISO8583_BCD, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 4},		// ����Ч��
		{15, '0', '0', (ISO8583_RIGHT << 4) + ISO8583_BCD, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 4},		// ��������
		{22, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BCD, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 3},		// ��������뷽ʽ��
		{23, '0', '0', (ISO8583_RIGHT << 4) + ISO8583_BCD, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 3},		// �����к�
		{25, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BCD, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 2},		// �����������
		{26, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BCD, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 2},		// �����PIN��ȡ��
		{28, '0', '0', (ISO8583_LEFT << 4) + ISO8583_ASC, (ISO8583_LLLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 128}, // ��̬�˵�
		//{30, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BIN, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 32}, //�ŵ���������
		{30, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BIN, (ISO8583_LLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 64},	 // ���ļ�����
		{32, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BCD, (ISO8583_LLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 11},	 // ��������ʶ��
		{34, '0', '0', (ISO8583_LEFT << 4) + ISO8583_ASC, (ISO8583_LLLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 32},	 // IC������
		{35, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BCD, (ISO8583_LLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 37},	 // 2�ŵ�����
		{36, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BCD, (ISO8583_LLLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 104},	 // 3�ŵ�����
		{37, '0', '0', (ISO8583_LEFT << 4) + ISO8583_ASC, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 12},		 // �����ο���
		{38, '0', '0', (ISO8583_LEFT << 4) + ISO8583_ASC, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 6},		 // ��Ȩ��ʶӦ����
		{39, '0', '0', (ISO8583_LEFT << 4) + ISO8583_ASC, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 2},		 // Ӧ����
		{41, '0', '0', (ISO8583_LEFT << 4) + ISO8583_ASC, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 8},		 // �ܿ����ն˱�ʶ��
		{42, '0', '0', (ISO8583_LEFT << 4) + ISO8583_ASC, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 15},		 // �ܿ��ű�ʶ��
		{43, '0', '0', (ISO8583_LEFT << 4) + ISO8583_ASC, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 40},		 // �̻���
		{44, ' ', '0', (ISO8583_RIGHT << 4) + ISO8583_ASC, (ISO8583_LLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 25},	 // ��������
		{45, ' ', '0', (ISO8583_LEFT << 4) + ISO8583_ASC, (ISO8583_LLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 79},	 // �ն˳���+�ͺ�+SN
		{48, ' ', '0', (ISO8583_LEFT << 4) + ISO8583_BCD, (ISO8583_LLLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 62},	 // ��������-˽��
		{49, ' ', '0', (ISO8583_LEFT << 4) + ISO8583_ASC, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 3},		 // ���׻��Ҵ���
		{50, '0', '0', (ISO8583_RIGHT << 4) + ISO8583_ASC, (ISO8583_LLLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 999}, // �ն�RSA��Կ
		{52, ' ', '0', (ISO8583_LEFT << 4) + ISO8583_BIN, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 8},		 // ���˱�ʶ������
		{53, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BCD, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 16},		 // ��ȫ������Ϣ
		{54, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BIN, (ISO8583_LLLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 20},	 // ���
		{55, '0', '0', (ISO8583_RIGHT << 4) + ISO8583_BIN, (ISO8583_LLLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 255}, // IC����Ϣ��
		{56, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BIN, (ISO8583_LLLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 40},	 // Ӧ����Ϣ
		{57, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BIN, (ISO8583_LLLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 999},	 // Addlwl ��ά����Ϣ
		{58, ' ', '0', (ISO8583_RIGHT << 4) + ISO8583_ASC, (ISO8583_LLLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 100}, // PBOC����Ǯ����׼������Ϣ
		{59, '0', '0', (ISO8583_LEFT << 4) + ISO8583_ASC, (ISO8583_LLLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 999},	 // Addlwl ���к�Ψһ��ʶ
		{60, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BCD, (ISO8583_LLLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 999},	 // �Զ�����
		{61, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BCD, (ISO8583_LLLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 999},	 // ԭʼ��Ϣ��
		{62, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BIN, (ISO8583_LLLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 999},	 // �Զ�����
		{63, '0', '0', (ISO8583_LEFT << 4) + ISO8583_ASC, (ISO8583_LLLVAR << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 999},	 // �Զ�����
		{64, '0', '0', (ISO8583_LEFT << 4) + ISO8583_BIN, (ISO8583_FIX << 8) + (ISO8583_RIGHT << 4) + ISO8583_BCD, 8},		 // ���ļ�����
		{0, 0, 0, 0, 0, 0}};

int ChkRespon(void *ptr, STSYSTEM *pstSystem, const char *psBitmap)
{
	struct
	{
		char sProcCode[6];			/**<3 ������*/
		char szAmount[12 + 1];		/**<4 ���*/
		char szTrace[6 + 1];		/**<11 POS��ˮ��*/
		char szServerCode[2 + 1];	/**<25 �����������*/
		char szPosID[8 + 1];		/**<41 �ն˺� */
		char szShopID[15 + 1];		/**<42 �̻��� */
		char szCurrencyCode[3 + 1]; /**<49 ���׻��Ҵ���*/
	} stMustChkResp;
	int nBitmapLen = 8;
	int n;
	int i;
	int j;
	unsigned char cBitMask;
	LNode P;
	char *pszTitle = "���";

	if (NULL == psBitmap)
	{
		return -1;
	}
	memset(&stMustChkResp, 0, sizeof(stMustChkResp));
	if (psBitmap[0] & 0x80)
	{
		nBitmapLen = 16;
	}
	else
	{
		nBitmapLen = 8;
	}

	n = 0;
	for (i = 0; i < nBitmapLen; i++)
	{
		cBitMask = 0x80;
		for (j = 1; j < 9; j++, cBitMask >>= 1)
		{
			if ((psBitmap[i] & cBitMask) == 0)
				continue;
			n = (i << 3) + j;
	
			switch (n)
			{
			case 2:
				P = ISO8583_GetField(ptr, n);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)pstSystem->szPan, P->strData, P->nDataLen);
				}
				else
				{
					return APP_FAIL;
				}
				break;
			case 3:
				P = ISO8583_GetField(ptr, n);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)stMustChkResp.sProcCode, P->strData, P->nDataLen);
				}
				__TRACEHEX__(stMustChkResp.sProcCode, 6);
				__TRACEHEX__(pstSystem->sProcCode, 6);
				if (memcmp(stMustChkResp.sProcCode, pstSystem->sProcCode, 6) != 0)
				{
					app_lvgl_clearClientBg();
					app_lvgl_msgTips("Return processing code does not match");
					Sys_Delay_ms(1000);
					return APP_FAIL;
				}
				break;
			case 4:
				P = ISO8583_GetField(ptr, 39);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)pstSystem->szResponse, P->strData, P->nDataLen);
				}
				else
				{
					app_lvgl_clearClientBg();
					app_lvgl_msgTips("No response code returned");
					Sys_Delay_ms(1000);
					return APP_FAIL;
				}

				// P = ISO8583_GetField(ptr, n);
				// if (P != NULL && P->nDataLen > 0)
				// {
				// 	memcpy((unsigned char *)stMustChkResp.szAmount, P->strData, P->nDataLen);
				// }
				// if (memcmp(pstSystem->szResponse, "10", 2) == 0)
				// {
				// 	memcpy(pstSystem->szAmount, stMustChkResp.szAmount, 12);
				// }
				// if ((memcmp(stMustChkResp.szAmount, pstSystem->szAmount, 12) != 0))
				// {
				// 	if (atoll(stMustChkResp.szAmount) != atoll(pstSystem->szAmount))
				// 	{
				// 		//Public_MsgBox(pszTitle, "���ؽ���", 10);
				// 		app_lvgl_clearClientBg();
				// 		app_lvgl_msgTips("The returned amount does not match");
				//		Sys_Delay_ms(1000);
				// 		return APP_FAIL;
				// 	}
				// }
				break;
			case 11:
#if 0
				nFieldLen = ISO_GetField(ISO_RSP, n, (unsigned char *)stMustChkResp.szTrace);
				if (nFieldLen <= 0)
				{
					return APP_FAIL;
				}
				if (APP_OK != PubChkDigitStr(stMustChkResp.szTrace, nFieldLen))/**<�����ˮ�ŵĺϷ���*/
				{
					continue;/**<�����ˮ�Ų������ֵľͲ��ʶԣ����posp�Ĵ���*/
				}
				if (memcmp(stMustChkResp.szTrace, pstSystem->szTrace, 6) != 0)
				{
					if (atol(stMustChkResp.szTrace) != atol(pstSystem->szTrace))
					{
						Public_MsgBox(pszTitle, "������ˮ�Ų���", 10);
						return APP_FAIL;
					}
				}
				break;
#endif
			case 12:
				P = ISO8583_GetField(ptr, n);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)pstSystem->szTime, P->strData, P->nDataLen);
				}
				else
				{
					return APP_FAIL;
				}
				break;
			case 13:
				P = ISO8583_GetField(ptr, n);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)pstSystem->szDate, P->strData, P->nDataLen);
				}
				else
				{
					return APP_FAIL;
				}
				break;
			case 14:
				P = ISO8583_GetField(ptr, n);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)pstSystem->szExpDate, P->strData, P->nDataLen);
				}
				else
				{
					return APP_FAIL;
				}
				break;
			case 15:
				P = ISO8583_GetField(ptr, n);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)pstSystem->szSettleDate, P->strData, P->nDataLen);
				}
				else
				{
					return APP_FAIL;
				}
				break;
			case 25:
				P = ISO8583_GetField(ptr, n);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)stMustChkResp.szServerCode, P->strData, P->nDataLen);
				}
				else
				{
					return APP_FAIL;
				}
				if (memcmp(stMustChkResp.szServerCode, pstSystem->szServerCode, 2) != 0)
				{
					//Public_MsgBox(pszTitle, "����������벻��", 10);
					app_lvgl_clearClientBg();
					app_lvgl_msgTips("Service point condition code does not match");
					Sys_Delay_ms(1000);
					return APP_FAIL;
				}
				break;
			case 32:
				P = ISO8583_GetField(ptr, n);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)pstSystem->szAcqCenterCode, P->strData, P->nDataLen);
				}
				else
				{
					//Public_MsgBox(pszTitle, "δ����������ʶ��", 10);
					app_lvgl_clearClientBg();
					app_lvgl_msgTips("Unreturned recipient identification code");
					Sys_Delay_ms(1000);
					return APP_FAIL;
				}
				break;
			case 37:
				P = ISO8583_GetField(ptr, n);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)pstSystem->szRefnum, P->strData, P->nDataLen);
				}
				else
				{
					//Public_MsgBox(pszTitle, "δ����ϵͳ�ο���", 10);
					app_lvgl_clearClientBg();
					app_lvgl_msgTips("System reference number not returned");
					Sys_Delay_ms(1000);
					return APP_FAIL;
				}
				break;
			case 38:
				P = ISO8583_GetField(ptr, n);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)pstSystem->szAuthCode, P->strData, P->nDataLen);
				}
				else
				{
					return APP_FAIL;
				}
				break;
			case 39:
				P = ISO8583_GetField(ptr, n);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)pstSystem->szResponse, P->strData, P->nDataLen);
				}
				else
				{
					//Public_MsgBox(pszTitle, "δ����Ӧ����", 10);
					app_lvgl_clearClientBg();
					app_lvgl_msgTips("No response code returned");
					Sys_Delay_ms(1000);
					return APP_FAIL;
				}
				break;
			case 41:
#if 1
				P = ISO8583_GetField(ptr, n);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)stMustChkResp.szPosID, P->strData, P->nDataLen);
				}
				else
				{
					return APP_FAIL;
				}
				if (memcmp(stMustChkResp.szPosID, pstSystem->szPosID, 8) != 0)
				{
					//Public_MsgBox(pszTitle, "�����ն˺Ų���", 10);
					app_lvgl_clearClientBg();
					app_lvgl_msgTips("The returned terminal number does not match");
					Sys_Delay_ms(1000);
					return APP_FAIL;
				}
				break;
#endif
			case 42:
#if 1
				P = ISO8583_GetField(ptr, n);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)stMustChkResp.szShopID, P->strData, P->nDataLen);
				}
				else
				{
					return APP_FAIL;
				}
				__TRACEHEX__(stMustChkResp.szShopID, 15);
				__TRACEHEX__(pstSystem->szShopID, 15);
				if (memcmp(stMustChkResp.szShopID, pstSystem->szShopID, 15) != 0)
				{
					//Public_MsgBox(pszTitle, "�����̻��Ų���", 10);
					app_lvgl_clearClientBg();
					app_lvgl_msgTips("The merchant number returned does not match");
					Sys_Delay_ms(1000);
					return APP_FAIL;
				}
#endif
				break;
			case 44:
				P = ISO8583_GetField(ptr, n);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)&(pstSystem->stField44), P->strData, P->nDataLen);
				}
				else
				{
					return APP_FAIL;
				}
				break;
			case 49:
				P = ISO8583_GetField(ptr, n);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)stMustChkResp.szCurrencyCode, P->strData, P->nDataLen);
				}
				else
				{
					return APP_FAIL;
				}
				if (memcmp(stMustChkResp.szCurrencyCode, "156", 3) != 0)
				{
					//Public_MsgBox(pszTitle, "���ػ��Ҵ��벻��", 10);
					app_lvgl_clearClientBg();
					app_lvgl_msgTips("Return currency code does not match");
					Sys_Delay_ms(1000);
					return APP_FAIL;
				}
				break;
			case 62:
				P = ISO8583_GetField(ptr, n);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)pstSystem->sField62, P->strData, P->nDataLen);
				}
				else
				{
					return APP_FAIL;
				}
				__TRACEHEX__(pstSystem->sField62, 50);
				break;
			case 63:
				P = ISO8583_GetField(ptr, n);
				if (P != NULL && P->nDataLen > 0)
				{
					memcpy((unsigned char *)&(pstSystem->stField63), P->strData, P->nDataLen);
				}
				else
				{
					return APP_FAIL;
				}
				memcpy(pstSystem->szInterOrg, pstSystem->stField63.sField63_1, 3);
				break;
			case 64:
				pstSystem->cMustChkMAC = 0x01;
				break;
			default:
				break;
			}
		}
	}
	return APP_OK;
}

static int FormISO8583Head(int len, char *head)
{
	int headLen = 0;
	unsigned char headAsc[4 + 1] = {0};

	headLen = len + 11;
	sprintf(headAsc, "%04X", headLen);
	Pub_AscToBcd(headAsc, head, 4);
	memcpy(head + 2, "\x60\x03\x01\x00\x00\x61\x32\x00\x32\x00\x01", 11);
	headLen = 13;
	return headLen;
}
int CheckMsgID(char *pszOld, char *psRecvPacket)
{
	char szReqMsgId[2 + 1];
	char szRecvMsgId[2 + 1];
	char *pszTitle = "���";

	Util_Asc2Bcd(psRecvPacket, szReqMsgId, 4);
	szReqMsgId[1] |= 0x10;
	__TRACEHEX__(szReqMsgId, 2);

	Util_Asc2Bcd(pszOld, szRecvMsgId, 4);
	__TRACEHEX__(szRecvMsgId, 2);
	if (memcmp(szReqMsgId, szRecvMsgId, 2) != 0)
	{	
		//Public_MsgBox(pszTitle, "������Ϣ���Ͳ���", 30);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips("Return message type does not match");
		Sys_Delay_ms(1000);
		return APP_FAIL;
	}
	return APP_OK;
}

/*
 *mod: 0:����MAC 1:У��
 */
int CalcMac(char *outBuf, char *inBuf, int len, int mode)
{
	int iRet = 0;
	if (mode == 0)
	{
		iRet = api_sec_calcMac(3, 1, 10, inBuf, len, outBuf);
		//__TRACEASC__("api_sec_calcMac iRet=%d", iRet);
	}

	return 0;
}

static int PackReversalBag(STSYSTEM *stSystem, char *data)
{

	void *ptr = NULL;
	unsigned char *str = NULL;
	char buf[100] = {0};
	char mac[20] = {0};
	int len, headLen;
	int ret;

	// ��ʼ��������
	ptr = ISO8583_Init();

	//@0:��Ϣ����	0400
	ASSERT_FAIL(ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 0, stSystem->szMsgID, 4));

	//@2:���˺�
	ASSERT_FAIL(ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 2, stSystem->szPan, strlen((const char *)stSystem->szPan)));

	//@3:���״����� 00xxxx 000000
	ASSERT_FAIL(ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 3, "000000", 6));

	//@4:���׽��
	sprintf(buf, "%012lld", atoll(stSystem->szAmount));
	ASSERT_FAIL(ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 4, buf, 12));

	//@11:�ܿ���ϵͳ���ٺ�
	ASSERT_FAIL(ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 11, stSystem->szTrace, 6));

	//@14��Ƭ��Ч��
	ASSERT_FAIL(ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 14, stSystem->szExpDate, 4));

	//@22:��������뷽ʽ
	ASSERT_FAIL(ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 22, "050", 3));

	//@23:�����к�
	// ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 23, stSystem->szCardSerialNo, 3);

	//@25:�����������
	ASSERT_FAIL(ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 25, "00", 2));

	//@35:���ŵ���Ϣ
	ASSERT_FAIL(ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 35, stSystem->szTrack2, strlen(stSystem->szTrack2)));

	//@38:��Ȩ��ʶӦ����
	// ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 38, stSystem->szAuthCode, 6);

	//@39:Ӧ���루����ԭ��
	ASSERT_FAIL(ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 39, stSystem->szResponse, 2));

	//@41:�ն˺�
	ASSERT_FAIL(ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 41, stSystem->szPosID, 8));

	//@42:�̻��� AN15
	ASSERT_FAIL(ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 42, stSystem->szShopID, 15));

	//@49:���׻��Ҵ���
	ASSERT_FAIL(ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 49, stSystem->szMoneyCode, 3));

	//@60:�Զ���
	memset(buf, 0, sizeof(buf));
	strcpy(buf, "22");
	strcat(buf, stSystem->stField60.sBatchNum);
	strcat(buf, "00050");
	ASSERT_FAIL(ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 60, buf, strlen(buf)));

	//@61:ԭʼ������
	memset(buf, 0, sizeof(buf));
	strcpy(buf, stSystem->stField61.sOrigBatchNum);
	strcat(buf, stSystem->stField61.szOrigTrace);
	ASSERT_FAIL(ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 61, buf, strlen(buf)));

	//@64:MAC��
	ASSERT_FAIL(ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 64, "\x00\x00\x00\x00\x00\x00\x00\x00", 8));

	// �������
	str = (unsigned char *)_malloc(ISO8583_GetPackLen(ptr));
	ret = ISO8583_PackData((ISO8583_FieldType *)sg_IsoCfg, ptr, str);
	if (ret < 0)
	{
		ISO8583_Free(&ptr);
		_free(str);
		return -1;
	}
	len = ret;

	// ����MAC
	ret = CalcMac(mac, str, len, 0);
	if (ret != 0)
	{
		//Public_MsgBox("���", "MAC����ʧ��", 10);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips("MAC calculation failed");
		Sys_Delay_ms(1000);
		ISO8583_Free(&ptr);
		_free(str);
		return APP_FAIL;
	}

	memset(buf, 0, sizeof(buf));
	headLen = FormISO8583Head(len, buf);

	memcpy(data, buf, headLen);
	memcpy(data + headLen, str, len);

	len = len + headLen;
	memcpy(data + len - 8, mac, 8);

	ISO8583_Free(&ptr);
	_free(str);
	/************************�������******************/

	return len;
}

static int PackCardSaleBag(STSYSTEM *stSystem, char *data)
{
	void *ptr = NULL;
	unsigned char *str = NULL;
	char buf[100] = {0};
	char mac[20] = {0};
	int len, headLen;
	int ret;

	// ��ʼ��������
	ptr = ISO8583_Init();

	//@0:��Ϣ����	0200
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 0, stSystem->szMsgID, 4);

	//@2:���˺�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 2, stSystem->szPan, strlen((const char *)stSystem->szPan));

	//@3:���״����� 00xxxx 000000
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 3, stSystem->sProcCode, 6);

	//@4:���׽��
	sprintf(buf, "%012lld", atoll(stSystem->szAmount));
	/*Util_Asc2Bcd(stSystem->szAmount, buf, 12);*/
	// sprintf(buf, "%02x%02x%02x%02x%02x%02x", stSystem->szAmount[0], stSystem->szAmount[1], stSystem->szAmount[2], stSystem->szAmount[3], stSystem->szAmount[4], stSystem->szAmount[5]);
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 4, buf, 12);

	//@11:�ܿ���ϵͳ���ٺ�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 11, stSystem->szTrace, 6);

	//@14��Ƭ��Ч��
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 14, stSystem->szExpDate, 4);

	//@22:��������뷽ʽ
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 22, "050", 3);

	//@23:�����к�
	// ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 23, stSystem->szCardSerialNo, 3);

	//@25:�����������
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 25, "00", 2);

	//@35:���ŵ���Ϣ
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 35, stSystem->szTrack2, strlen(stSystem->szTrack2));

	//@41:�ն˺�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 41, stSystem->szPosID, 8);

	//@42:�̻��� AN15
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 42, stSystem->szShopID, 15);

	//@49:���׻��Ҵ���
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 49, stSystem->szMoneyCode, 3);

	//@55:IC��������
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 55, stSystem->psAddField, stSystem->nAddFieldLen);

	//@60:�Զ���
	memset(buf, 0, sizeof(buf));
	strcpy(buf, "22");
	strcat(buf, stSystem->stField60.sBatchNum);
	strcat(buf, "00050");
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 60, buf, strlen(buf));
	//@64:MAC��
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 64, "\x00\x00\x00\x00\x00\x00\x00\x00", 8);

	// �������
	str = (unsigned char *)_malloc(ISO8583_GetPackLen(ptr));
	ret = ISO8583_PackData((ISO8583_FieldType *)sg_IsoCfg, ptr, str);
	if (ret < 0)
	{
		ISO8583_Free(&ptr);
		_free(str);
		return -1;
	}
	len = ret;

	// ����MAC
	ret = CalcMac(mac, str, len, 0);
	if (ret != 0)
	{
		//__TRACEASC__("MAC����ʧ��");
		ISO8583_Free(&ptr);
		_free(str);
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	headLen = FormISO8583Head(len, buf);

	memcpy(data, buf, headLen);
	memcpy(data + headLen, str, len);

	len = len + headLen;
	memcpy(data + len - 8, mac, 8);

	ISO8583_Free(&ptr);
	_free(str);
	/************************�������******************/

	return len;
}
static int PackVoidBag(STSYSTEM *stSystem, char *data)
{
	void *ptr = NULL;
	unsigned char *str = NULL;
	char buf[100] = {0};
	char mac[20] = {0};
	int len, headLen;
	int ret;

	// ��ʼ��������
	ptr = ISO8583_Init();

	//@0:��Ϣ����	0200
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 0, stSystem->szMsgID, 4);

	//@2:���˺�
	// ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 2, stSystem->szPan, strlen((const char*)stSystem->szPan));

	//@3:���״����� 00xxxx 000000
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 3, stSystem->sProcCode, 6);

	//@4:���׽��
	sprintf(buf, "%012d", atoi(stSystem->szAmount));
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 4, buf, 12);

	//@11:�ܿ���ϵͳ���ٺ�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 11, stSystem->szTrace, 6);

	//@14��Ƭ��Ч��
	// ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 14, stSystem->szExpDate, 4);

	//@22:��������뷽ʽ
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 22, "050", 3);

	//@23:�����к�
	// ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 23, stSystem->szCardSerialNo, 3);

	//@25:�����������
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 25, "00", 2);

	//@35:���ŵ���Ϣ
	// ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 35, stSystem->szTrack2, strlen(stSystem->szTrack2));

	//@37:�����ο��ţ�ԭ���ѽ��ײο��ţ�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 37, stSystem->szRefnum, 12);

	//@38:��Ȩ��ʶӦ����
	// ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 38, stSystem->szAuthCode, 6);

	//@41:�ն˺�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 41, stSystem->szPosID, 8);

	//@42:�̻��� AN15
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 42, stSystem->szShopID, 15);

	//@49:���׻��Ҵ���
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 49, stSystem->szMoneyCode, 3);

	//@60:�Զ���
	memset(buf, 0, sizeof(buf));
	strcpy(buf, "23");
	strcat(buf, stSystem->stField60.sBatchNum);
	strcat(buf, "00050");
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 60, buf, strlen(buf));

	//@61:ԭʼ������
	memset(buf, 0, sizeof(buf));
	strcpy(buf, stSystem->stField61.sOrigBatchNum);
	strcat(buf, stSystem->stField61.szOrigTrace);
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 61, buf, strlen(buf));

	//@64:MAC��
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 64, "\x00\x00\x00\x00\x00\x00\x00\x00", 8);

	// �������
	str = (unsigned char *)_malloc(ISO8583_GetPackLen(ptr));
	ret = ISO8583_PackData((ISO8583_FieldType *)sg_IsoCfg, ptr, str);
	if (ret < 0)
	{
		ISO8583_Free(&ptr);
		_free(str);
		return -1;
	}
	len = ret;

	// ����MAC
	ret = CalcMac(mac, str, len, 0);
	if (ret != 0)
	{
		//__TRACEASC__("MAC����ʧ��");
		ISO8583_Free(&ptr);
		_free(str);
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	headLen = FormISO8583Head(len, buf);

	memcpy(data, buf, headLen);
	memcpy(data + headLen, str, len);

	len = len + headLen;
	memcpy(data + len - 8, mac, 8);
	// debughex(data,len);

	ISO8583_Free(&ptr);
	_free(str);
	/************************�������******************/

	return len;
}

static int PackRefundBag(STSYSTEM *stSystem, char *data)
{

	void *ptr = NULL;
	unsigned char *str = NULL;
	char buf[100] = {0};
	char mac[20] = {0};
	int len, headLen;
	int ret;

	// ��ʼ��������
	ptr = ISO8583_Init();

	//@0:��Ϣ����	0200
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 0, stSystem->szMsgID, 4);

	//@2:���˺�
	// ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 2, stSystem->szPan, strlen((const char*)stSystem->szPan));

	//@3:���״����� 00xxxx 000000
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 3, stSystem->sProcCode, 6);

	//@4:���׽��
	sprintf(buf, "%012d", atoi(stSystem->szAmount));
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 4, buf, 12);

	//@11:�ܿ���ϵͳ���ٺ�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 11, stSystem->szTrace, 6);

	//@14��Ƭ��Ч��
	// ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 14, stSystem->szExpDate, 4);

	//@22:��������뷽ʽ
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 22, "050", 3);

	//@23:�����к�
	// ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 23, stSystem->szCardSerialNo, 3);

	//@25:�����������
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 25, "00", 2);

	//@35:���ŵ���Ϣ
	// ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 35, stSystem->szTrack2, strlen(stSystem->szTrack2));

	//@37:�����ο��ţ�ԭ���ѽ��ײο��ţ�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 37, stSystem->szRefnum, 12);

	//@38:��Ȩ��ʶӦ����
	// ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 38, stSystem->szAuthCode, 6);

	//@41:�ն˺�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 41, stSystem->szPosID, 8);

	//@42:�̻��� AN15
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 42, stSystem->szShopID, 15);

	//@49:���׻��Ҵ���
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 49, stSystem->szMoneyCode, 3);

	//@60:�Զ���
	memset(buf, 0, sizeof(buf));
	strcpy(buf, "25");
	strcat(buf, stSystem->stField60.sBatchNum);
	strcat(buf, "00050");
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 60, buf, strlen(buf));

	//@61:ԭʼ������
	memset(buf, 0, sizeof(buf));
	strcpy(buf, stSystem->stField61.sOrigBatchNum);
	strcat(buf, stSystem->stField61.szOrigTrace);
	strcat(buf, stSystem->stField61.szOrigDate);
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 61, buf, strlen(buf));

	//@63:�Զ���
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 63, "000", 3); // �������ÿ���˾���룬����ȫ0

	//@64:MAC��
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 64, "\x00\x00\x00\x00\x00\x00\x00\x00", 8);

	// �������
	str = (unsigned char *)_malloc(ISO8583_GetPackLen(ptr));
	ret = ISO8583_PackData((ISO8583_FieldType *)sg_IsoCfg, ptr, str);
	if (ret < 0)
	{
		ISO8583_Free(&ptr);
		_free(str);
		return -1;
	}
	len = ret;

	// ����MAC
	ret = CalcMac(mac, str, len, 0);
	if (ret != 0)
	{
		//__TRACEASC__("MAC����ʧ��");
		ISO8583_Free(&ptr);
		_free(str);
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	headLen = FormISO8583Head(len, buf);

	memcpy(data, buf, headLen);
	memcpy(data + headLen, str, len);

	len = len + headLen;
	memcpy(data + len - 8, mac, 8);

	ISO8583_Free(&ptr);
	_free(str);
	/************************�������******************/

	return len;
}

static int PackBalanceBag(STSYSTEM *stSystem, char *data)
{
	void *ptr = NULL;
	unsigned char *str = NULL;
	char buf[100] = {0};
	char mac[20] = {0};
	int len, headLen;
	int ret;

	// ��ʼ��������
	ptr = ISO8583_Init();

	//@0:��Ϣ����	0200
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 0, stSystem->szMsgID, 4);

	//@2:���˺�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 2, stSystem->szPan, strlen((const char *)stSystem->szPan));

	//@3:���״����� 00xxxx 000000
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 3, stSystem->sProcCode, 6);

	//@4:���׽��
	sprintf(buf, "%012d", atoll(stSystem->szAmount));
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 4, buf, 12);

	//@11:�ܿ���ϵͳ���ٺ�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 11, stSystem->szTrace, 6);

	//@14��Ƭ��Ч��
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 14, stSystem->szExpDate, 4);

	//@22:��������뷽ʽ
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 22, "050", 3);

	//@23:�����к�
	// ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 23, stSystem->szCardSerialNo, 3);

	//@25:�����������
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 25, "00", 2);

	//@35:���ŵ���Ϣ
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 35, stSystem->szTrack2, strlen(stSystem->szTrack2));

	//@41:�ն˺�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 41, stSystem->szPosID, 8);

	//@42:�̻��� AN15
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 42, stSystem->szShopID, 15);

	//@49:���׻��Ҵ���
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 49, stSystem->szMoneyCode, 3);

	//@55:IC��������
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 55, stSystem->psAddField, stSystem->nAddFieldLen);

	//@60:�Զ���
	memset(buf, 0, sizeof(buf));
	strcpy(buf, stSystem->stField60.sFuncCode);
	strcat(buf, stSystem->stField60.sBatchNum);
	strcat(buf, "00050");
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 60, buf, strlen(buf));
	//@64:MAC��
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 64, "\x00\x00\x00\x00\x00\x00\x00\x00", 8);

	// �������
	str = (unsigned char *)_malloc(ISO8583_GetPackLen(ptr));
	ret = ISO8583_PackData((ISO8583_FieldType *)sg_IsoCfg, ptr, str);
	if (ret < 0)
	{
		ISO8583_Free(&ptr);
		_free(str);
		return -1;
	}
	len = ret;

	// ����MAC
	ret = CalcMac(mac, str, len, 0);
	if (ret != 0)
	{
		//__TRACEASC__("MAC����ʧ��");
		ISO8583_Free(&ptr);
		_free(str);
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	headLen = FormISO8583Head(len, buf);

	memcpy(data, buf, headLen);
	memcpy(data + headLen, str, len);

	len = len + headLen;
	memcpy(data + len - 8, mac, 8);

	ISO8583_Free(&ptr);
	_free(str);
	/************************�������******************/

	return len;
}

static int PackSignBag(STSYSTEM *stSystem, char *data)
{
	void *ptr = NULL;
	unsigned char *str = NULL;
	char buf[100] = {0};
	int len, headLen;
	int ret;

	/************************��ʼ���******************/
	// ��ʼ��������
	ptr = ISO8583_Init();

	//@0:��Ϣ����	0800
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 0, stSystem->szMsgID, 4);

	//@11:��ˮ��
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 11, stSystem->szTrace, 6);

	//@41:�ն˺�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 41, stSystem->szPosID, 8); //"81100005"

	//@42:�̻��� AN15
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 42, stSystem->szShopID, 15);

	//@60:�Զ���
	memset(buf, 0, sizeof(buf));
	strcpy(buf, "00");
	strcat(buf, stSystem->stField60.sBatchNum); //"000001"

	strcat(buf, "004"); // ������001 ˫����003  ���ŵ���Կ 004
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 60, buf, strlen(buf));

	//@63:�Զ���
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 63, "001", 3); // ����Ա����

	// �������
	str = (unsigned char *)_malloc(ISO8583_GetPackLen(ptr));
	ret = ISO8583_PackData((ISO8583_FieldType *)sg_IsoCfg, ptr, str);
	if (ret < 0)
	{
		ISO8583_Free(&ptr);
		_free(str);
		return -1;
	}
	len = ret;

	memset(buf, 0, sizeof(buf));
	headLen = FormISO8583Head(len, buf);

	memcpy(data, buf, headLen);
	memcpy(data + headLen, str, len);

	len = len + headLen;

	ISO8583_Free(&ptr);
	_free(str);

	/************************�������******************/
	return len;
}
static int PackCheckOutBag(STSYSTEM *stSystem, char *data)
{
	void *ptr = NULL;
	unsigned char *str = NULL;
	char buf[100] = {0};
	int len, headLen;
	int ret;
	/************************��ʼ���******************/
	// ��ʼ��������
	ptr = ISO8583_Init();

	//@0:��Ϣ����	0820
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 0, stSystem->szMsgID, 4);

	//@11:��ˮ��
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 11, stSystem->szTrace, 6);

	//@41:�ն˺�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 41, stSystem->szPosID, 8); //"81100005"

	//@42:�̻��� AN15
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 42, stSystem->szShopID, 15);

	//@60:�Զ���
	memset(buf, 0, sizeof(buf));
	strcpy(buf, "00");
	strcat(buf, stSystem->stField60.sBatchNum); //"000001"
	strcat(buf, "002");
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 60, buf, strlen(buf));

	// �������
	str = (unsigned char *)_malloc(ISO8583_GetPackLen(ptr));
	ret = ISO8583_PackData((ISO8583_FieldType *)sg_IsoCfg, ptr, str);
	if (ret < 0)
	{
		ISO8583_Free(&ptr);
		_free(str);
		return -1;
	}
	len = ret;

	memset(buf, 0, sizeof(buf));
	headLen = FormISO8583Head(len, buf);

	memcpy(data, buf, headLen);
	memcpy(data + headLen, str, len);

	len = len + headLen;

	ISO8583_Free(&ptr);
	_free(str);
	/************************�������******************/

	return len;
}
// �������ܶ�
typedef struct
{
	int nDebitSaleNum;			 // ����ܱ���          N3*/
	char szDebitSaleAmt[12 + 1]; // ����ܽ��          N12*/

	int nCreditSaleNum;			  /*�����ܱ���          N3*/
	char szCreditSaleAmt[12 + 1]; /*�����ܽ��          N12*/

} STBATCHTOTAL;

static int PackBatchSettlementBag(STSYSTEM *stSystem, char *data)
{
	STBATCHTOTAL total;
	void *ptr = NULL;
	unsigned char *str = NULL;
	char buf[100] = {0};
	char temp[100] = {0};
	int len, headLen;
	int ret;

	/************************��ʼ���******************/
	// ��ʼ��������
	ptr = ISO8583_Init();

	//@0:��Ϣ����	0800
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 0, stSystem->szMsgID, 4);

	//@11:��ˮ��
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 11, stSystem->szTrace, 6);

	//@41:�ն˺�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 41, stSystem->szPosID, 8); //"81100005"

	//@42:�̻��� AN15
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 42, stSystem->szShopID, 15);

	//@48:��������-�����ܶ�(�ڿ�)
	memset(buf, 0, sizeof(buf));

	memset(&total, 0, sizeof(STBATCHTOTAL));
	// ����ʵ�ʽ��׼�¼����ͳ���ܱ������ܽ��
	total.nCreditSaleNum = 10;
	total.nDebitSaleNum = 5;
	sprintf(total.szCreditSaleAmt, "%012d", 1000);
	sprintf(total.szDebitSaleAmt, "%012d", 100);

	strcpy(buf, total.szDebitSaleAmt);
	sprintf(buf + 12, "%03d", total.nDebitSaleNum);
	strcpy(buf + 15, total.szCreditSaleAmt);
	sprintf(buf + 27, "%03d", total.nCreditSaleNum);
	// strcat(buf,"0"); ģ���̨����Ӧ�����������д
	// debugprint("%s_%d",buf,strlen(buf));

	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 48, buf, 30); //"00000000000100100000000000000000000000000010010000000000000000",62);

	//@49:���׻��Ҵ���-�����
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 49, stSystem->szMoneyCode, 3);

	//@60:�Զ���
	memset(buf, 0, sizeof(buf));
	strcpy(buf, "00");
	strcat(buf, stSystem->stField60.sBatchNum); //"000001"
	strcat(buf, "201");
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 60, buf, strlen(buf));

	//@63:�Զ���
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 63, "001", 3); // ����Ա����

	// �������
	str = (unsigned char *)_malloc(ISO8583_GetPackLen(ptr));
	ret = ISO8583_PackData((ISO8583_FieldType *)sg_IsoCfg, ptr, str);
	if (ret < 0)
	{
		ISO8583_Free(&ptr);
		_free(str);
		return -1;
	}
	len = ret;

	memset(buf, 0, sizeof(buf));
	headLen = FormISO8583Head(len, buf);

	memcpy(data, buf, headLen);
	memcpy(data + headLen, str, len);

	len = len + headLen;

	ISO8583_Free(&ptr);
	_free(str);
	/************************�������******************/
	return len;
}
static int PackParamTransmitBag(STSYSTEM *stSystem, char *data)
{
	EMVCAPUBKEYLIST stCAPubkey;
	EMVAIDLIST stAIDList;
	void *ptr = NULL;
	unsigned char *str = NULL;
	static unsigned int index = 0;
	char buf[100] = {0};
	char temp[100] = {0};
	int len, headLen;
	int ret;
	/************************��ʼ���******************/
	// ��ʼ��������
	ptr = ISO8583_Init();

	//@0:��Ϣ����	0800
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 0, stSystem->szMsgID, 4);

	//@41:�ն˺�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 41, stSystem->szPosID, 8); //"81100005"

	//@42:�̻��� AN15
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 42, stSystem->szShopID, 15);

	//@60:�Զ���
	memset(buf, 0, sizeof(buf));
	strcpy(buf, "00");
	strcat(buf, stSystem->stField60.sBatchNum);	  //"000001"
	strcat(buf, stSystem->stField60.sNetManCode); // IC����Կ370/371 IC������380/381 TMS����364/365(����62��)
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 60, buf, strlen(buf));

	if (strcmp(stSystem->stField60.sNetManCode, "370") == 0)
	{
		memset(buf, 0, sizeof(buf));
		memset(&stCAPubkey, 0, sizeof(stCAPubkey));
		// ret = GetRIDNeedUpdate(&stCAPubkey);
		memcpy(buf, "\x9F\x06\x05", 3);
		memcpy(buf + 3, stCAPubkey.RID, 5);
		memcpy(buf + 8, "\x9F\x22\x01", 3);
		memcpy(buf + 11, &stCAPubkey.CA_PKIndex, 1);
		//@62:�ն˲�����Ϣ
		ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 62, buf, 12);
	}
	else if (strcmp(stSystem->stField60.sNetManCode, "380") == 0)
	{
		memset(buf, 0, sizeof(buf));
		memset(&stAIDList, 0, sizeof(stAIDList));
		// ret = GetAIDNeedUpdate(&stAIDList);
		memcpy(buf, "\x9F\x06", 2);
		memcpy(buf + 2, &stAIDList, sizeof(EMVAIDLIST));
		//@62:�ն˲�����Ϣ
		ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 62, buf, stAIDList.AID_Length + 3);
	}

	// �������
	str = (unsigned char *)_malloc(ISO8583_GetPackLen(ptr));
	ret = ISO8583_PackData((ISO8583_FieldType *)sg_IsoCfg, ptr, str);
	if (ret < 0)
	{
		ISO8583_Free(&ptr);
		_free(str);
		return -1;
	}
	len = ret;

	memset(buf, 0, sizeof(buf));
	headLen = FormISO8583Head(len, buf);

	memcpy(data, buf, headLen);
	memcpy(data + headLen, str, len);

	len = len + headLen;

	ISO8583_Free(&ptr);
	_free(str);
	/************************�������******************/
	return len;
}
static int PackICStatusUploadBag(STSYSTEM *stSystem, char *data)
{
	void *ptr = NULL;
	unsigned char *str = NULL;
	char buf[100] = {0};
	int len, headLen;
	int ret;
	/************************��ʼ���******************/
	// ��ʼ��������
	ptr = ISO8583_Init();

	//@0:��Ϣ����	0820
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 0, stSystem->szMsgID, 4);

	//@41:�ն˺�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 41, stSystem->szPosID, 8); //"81100005"

	//@42:�̻��� AN15
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 42, stSystem->szShopID, 15);

	//@60:�Զ���
	memset(buf, 0, sizeof(buf));
	strcpy(buf, "00");
	strcat(buf, stSystem->stField60.sBatchNum);	  //"000001"
	strcat(buf, stSystem->stField60.sNetManCode); // IC����Կ����372 IC����������382
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 60, buf, strlen(buf));

	//@62:�ն˲�����Ϣ //�״�����Ϊ100
	memset(buf, 0, sizeof(buf));
	strcpy(buf, "100");
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 62, buf, strlen(buf));

	// �������
	str = (unsigned char *)_malloc(ISO8583_GetPackLen(ptr));
	ret = ISO8583_PackData((ISO8583_FieldType *)sg_IsoCfg, ptr, str);
	if (ret < 0)
	{
		ISO8583_Free(&ptr);
		_free(str);
		return -1;
	}
	len = ret;

	memset(buf, 0, sizeof(buf));
	headLen = FormISO8583Head(len, buf);

	memcpy(data, buf, headLen);
	memcpy(data + headLen, str, len);

	len = len + headLen;

	ISO8583_Free(&ptr);
	_free(str);
	/************************�������******************/
	return len;
}

static int PackEchoTestBag(STSYSTEM *stSystem, char *data)
{
	void *ptr = NULL;
	unsigned char *str = NULL;
	char buf[100] = {0};
	int len, headLen;
	int ret;
	/************************��ʼ���******************/
	// ��ʼ��������
	ptr = ISO8583_Init();

	//@0:��Ϣ����	0820
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 0, stSystem->szMsgID, 4);

	//@41:�ն˺�
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 41, stSystem->szPosID, 8); //"81100005"

	//@42:�̻��� AN15
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 42, stSystem->szShopID, 15);

	//@60:�Զ���
	memset(buf, 0, sizeof(buf));
	strcpy(buf, "00");
	strcat(buf, stSystem->stField60.sBatchNum); //"000001"
	strcat(buf, "301");
	ret = ISO8583_SetField((ISO8583_FieldType *)sg_IsoCfg, ptr, 60, buf, strlen(buf));

	// �������
	str = (unsigned char *)_malloc(ISO8583_GetPackLen(ptr));
	ret = ISO8583_PackData((ISO8583_FieldType *)sg_IsoCfg, ptr, str);
	if (ret < 0)
	{
		ISO8583_Free(&ptr);
		_free(str);
		return -1;
	}
	len = ret;

	memset(buf, 0, sizeof(buf));
	headLen = FormISO8583Head(len, buf);

	memcpy(data, buf, headLen);
	memcpy(data + headLen, str, len);

	len = len + headLen;

	ISO8583_Free(&ptr);
	_free(str);
	/************************�������******************/
	return len;
}

int Pack_ISO8583(STSYSTEM *stSystem, char *data)
{
	int ret = 0;

	switch (stSystem->cTransType)
	{
	case TT_CARDSALE:
		ret = PackCardSaleBag(stSystem, data);
		break;
	case TT_VOID:
		ret = PackVoidBag(stSystem, data);
		break;
	case TT_REFUND:
		ret = PackRefundBag(stSystem, data);
		break;
	case TT_CHECKIN:
		ret = PackSignBag(stSystem, data);
		break;
	case TT_CHECKOUT:
		ret = PackCheckOutBag(stSystem, data);
		break;
	case TT_BATCHSETTLE:
		ret = PackBatchSettlementBag(stSystem, data);
		break;
	// case TT_BATCHUPLOAD:
	//	break;
	case TT_PARAM:
		ret = PackParamTransmitBag(stSystem, data);
		break;
	case TT_STATUSUPLOAD:
		ret = PackICStatusUploadBag(stSystem, data);
		break;
	case TT_ECHOTEST:
		ret = PackEchoTestBag(stSystem, data);
		break;
	case TT_REVERSAL:
		ret = PackReversalBag(stSystem, data);
		break;
	case TT_BALANCE:
		ret = PackBalanceBag(stSystem, data);
		break;
	default:
		ret = -1;
		break;
	}
	return ret;
}
static int _UpdateWorkKey(unsigned char KeyType, unsigned char mkindex, unsigned char wkindex, unsigned char *pCipher, unsigned char *kcv)
{
	int sRet;
	sRet = api_sec_updateCipherKey(KeyType, 1, 1, mkindex, wkindex, pCipher, 16, 1, kcv, 4);
	//__TRACEASC__("api_sec_updateCipherKey sRet=%d", sRet);

	return sRet;
}

static int _DealSignBackBag(void *ptr, STSYSTEM *stSystem)
{
	int nRet;
	char dateTime[32] = {0};
	unsigned int paramlen = 0;
	unsigned char kcvIn[4] = {0};
	const unsigned char zero[8] = {0};
	unsigned char Wkey[16] = {0};
	unsigned char mainKey[32];
	memcpy(mainKey, "\x55\x55\x55\x55\x55\x55\x55\x55\x66\x66\x66\x66\x66\x66\x66\x66", 16);

	LNode P;
	// �������ص�62��
	CLRBUF(dateTime);

	// ʱ��ͬ��
	TimeUtil_GetDateTime(dateTime);
	P = ISO8583_GetField(ptr, 13);
	strcpy(dateTime + 4, P->strData);

	P = ISO8583_GetField(ptr, 12);
	// debug_printf("%s", P->strData);
	strcpy(dateTime + 8, P->strData);
	//__TRACEASC__("dateTime:%s", dateTime);
	TimeUtil_SetDateTime(dateTime);

	// ���κ�ͬ��
	P = ISO8583_GetField(ptr, 60);
	strcpy(stSystem->stField60.sBatchNum, P->strData + 2);

	// ����������Կ
	nRet = api_sec_updatePlainKey(0, 0x01, 0, mainKey, 16);
	// api_lcd_clearScreen();

	//Sys_DisplayLine(2, "���ڸ����ն���Կ", 0, ALIGN_CENTER);
	app_lvgl_clearClientBg();
	app_lvgl_msgTips("Updating terminal key");

	// ��Կ����
	P = ISO8583_GetField(ptr, 62);
	memcpy(Wkey, P->strData, 16);
	memcpy(kcvIn, P->strData + 16, 4);
	nRet = _UpdateWorkKey(0x10, 0, 10, Wkey,kcvIn);
	if (nRet == 0)
	{
		//Sys_DisplayLine(3, "����PIN��Կ�ɹ�", 0, ALIGN_CENTER);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips("Successfully updated PIN key");
		Sys_Delay_ms(1000);
	}
	memcpy(Wkey, P->strData + 20, 8);
	memcpy(Wkey + 8, P->strData + 20, 8);
	memcpy(kcvIn, P->strData + 20 + 16, 4);

	nRet = _UpdateWorkKey(0x10, 0, 11, Wkey, kcvIn);
	if (nRet == 0)
	{
		//Sys_DisplayLine(3, "����MAC��Կ�ɹ�", 0, ALIGN_CENTER);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips("Successfully updated MAC key");
		Sys_Delay_ms(1000);
	}

	memcpy(Wkey, P->strData + 20 + 20, 16);
	memcpy(kcvIn, P->strData + 20 + 20 + 16, 4);

	nRet = _UpdateWorkKey(0x10, 0, 12, Wkey, kcvIn);
	if (nRet == 0)
	{
		//Sys_DisplayLine(3, "����TDK�ŵ���Կ�ɹ�", 0, ALIGN_CENTER);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips("Successfully updated TDK track key");
		Sys_Delay_ms(1000);
	}
	return APP_SUCC;
}

typedef enum
{
	DOWN_CAPK = 0x01,
	DOWN_AID = 0x02,
	DOWN_TMS = 0x04,
	END_CAPK = 0x10,
	END_AID = 0x20,
	END_TMS = 0x40
} PARAMTYPE;

static int UnpackParamTransmitBag(STSYSTEM *stSystem, char *data, int dataLen)
{
	PARAMTYPE downType;
	void *ptr = NULL;
	int ret;
	int retMsgID;
	LNode P;
	// ��ʼ��������
	ptr = ISO8583_Init();
	ret = ISO8583_UnPackData((ISO8583_FieldType *)sg_IsoCfg, ptr, data, dataLen, NULL);

	//@0:��Ϣ����
	P = ISO8583_GetField(ptr, 0);
	ret = CheckMsgID(P->strData, stSystem->szMsgID);
	if (ret == -1)
		goto EXIT;
#if 1
	//@39:Ӧ����
	P = ISO8583_GetField(ptr, 39);
	if (NULL != P)
	{
		if (strcmp(P->strData, "00") != 0)
		{
			// DispErrorByMsg(PIC_09, "��������ʧ��");
			ret = APP_FAIL;
			goto EXIT;
		}
	}
#endif

	//@60:�Զ���-���������
	P = ISO8583_GetField(ptr, 60);
	// debug_printf("60��%s",P->strData+P->nDataLen-3);
	if (strcmp(P->strData + P->nDataLen - 3, "370") == 0)
	{
		downType = DOWN_CAPK;
	}
	else if (strcmp(P->strData + P->nDataLen - 3, "380") == 0)
	{
		downType = DOWN_AID;
	}

	// EMV_SetCAPubkeyPara(EMVCAPUBLICKEY * EmvCAPubkeyParam)

	//@62:�ն˲�����Ϣ
	P = ISO8583_GetField(ptr, 62);
	if (NULL != P)
	{
		if (memcmp(P->strData, 0x00, 1) == 0)
		{
			// DispErrorByMsg(PIC_09, "��̨�޸��������");
			ret = APP_FAIL;
			goto EXIT;
		}
		if (downType == DOWN_CAPK)
		{
			// ret = DealCAPKParam(P->strData,P->nDataLen);
			if (ret < 0)
			{
				ISO8583_Free(&ptr);
				return APP_FAIL;
			}
		}
		else if (downType == DOWN_AID)
		{
			// ret = DealAIDParam(P->strData,P->nDataLen);
			if (ret < 0)
			{
				ISO8583_Free(&ptr);
				return APP_FAIL;
			}
		}
	}

	// DispInfoByMsg(PIC_08, "�������ݳɹ�");
	Sys_GetKey(2 * 1000);
	ret = APP_SUCC;
EXIT:
	ISO8583_Free(&ptr);
	return ret;
}
static int UnpackICStatusUploadBag(STSYSTEM *stSystem, char *data, int dataLen)
{
	PARAMTYPE downType;
	void *ptr = NULL;
	int ret, num;
	int retMsgID;
	LNode P;

	// ��ʼ��������
	ptr = ISO8583_Init();
	ret = ISO8583_UnPackData((ISO8583_FieldType *)sg_IsoCfg, ptr, data, dataLen, NULL);

	//@0:��Ϣ����
	P = ISO8583_GetField(ptr, 0);
	// debug_printf("%s", P->strData);
	ret = CheckMsgID(P->strData, stSystem->szMsgID);
	if (ret == APP_FAIL)
		goto EXIT;

	//@39:Ӧ����
	P = ISO8583_GetField(ptr, 39);
	// debug_printf("39��%s", P->strData);
	if (strcmp(P->strData, "00") != 0)
	{
		// DispErrorByMsg(PIC_09, "״̬����ʧ��");
		ret = APP_FAIL;
		goto EXIT;
	}
	// DispInfoByMsg(PIC_08, "״̬���ͳɹ�");

	//@60:�Զ���-���������
	P = ISO8583_GetField(ptr, 60);
	// debug_printf("60��%s",P->strData+P->nDataLen-3);
	if (strcmp(P->strData + P->nDataLen - 3, "372") == 0)
	{
		downType = DOWN_CAPK;
	}
	else if (strcmp(P->strData + P->nDataLen - 3, "382") == 0)
	{
		downType = DOWN_AID;
	}

	//@62:�ն�״̬��Ϣ
	P = ISO8583_GetField(ptr, 62);

	if (downType == DOWN_CAPK)
	{
		// num = DealCAPubkeyRIDList(P->strData,P->nDataLen);
		ISO8583_Free(&ptr);
		// ret = UpdateAllCAPK(num);
		if (ret == APP_FAIL)
			return APP_FAIL;
	}
	else if (downType == DOWN_AID)
	{
		// num = DealAIDList(P->strData,P->nDataLen);
		ISO8583_Free(&ptr);
		// ret = UpdateAllAID(num);
	}

	ret = APP_SUCC;
EXIT:
	// ISO8583_Free(&ptr);
	return ret;
}

int Unpack_ISO8583(STSYSTEM *stSystem, char *data, int datalen)
{
	char *pData;
	int ret;
	int len;
	void *ptr = NULL;
	char mac[20] = {0};
	char szDispAmt[14 + 1];
	int retMsgID;
	LNode P;

	struct
	{
		char sAccount_Type[2];
		char sAmount_Type[2];
		char sCurrency_Code[3];
		char cAmount_Sign;
		char sAmount[12];
	} stBal; /**< ��Ų�ѯ�õ���54�������*/

	pData = data + 11; // ȥ��head������
	len = datalen - 11;

	// ��ʼ��������
	ptr = ISO8583_Init();
	__TRACEHEX__(pData, len);
	ret = ISO8583_UnPackData((ISO8583_FieldType *)sg_IsoCfg, ptr, pData, len, NULL);

	//@64:MAC
	P = ISO8583_GetField(ptr, 64);
	if (P != NULL)
	{
		ret = CalcMac(P->strData, pData, len - 8, 1);
		if (ret != 0)
		{
			// DispErrorByMsg(PIC_09, "MACУ��ʧ��");
			ISO8583_Free(&ptr);
			return -2;
		}
	}
	//@0:��Ϣ����
	P = ISO8583_GetField(ptr, 0);

	ret = CheckMsgID(P->strData, stSystem->szMsgID);
	if (ret == APP_FAIL)
		goto EXIT;

	ret = ChkRespon(ptr, stSystem, pData + 2);
	if (ret == APP_FAIL)
		goto EXIT;
	switch (stSystem->cTransType)
	{
	case TT_CHECKIN:
		ret = _DealSignBackBag(ptr, stSystem);
		goto EXIT;
		break;
	case TT_PARAM:
		ret = UnpackParamTransmitBag(stSystem, pData, len);
		break;
	case TT_STATUSUPLOAD:
		ret = UnpackICStatusUploadBag(stSystem, pData, len);
		break;
	case TT_BALANCE:
		P = ISO8583_GetField(ptr, 54);
		if (P != NULL && P->nDataLen > 0)
		{
			memset((char *)(&stBal), 0, 20);
			memcpy((unsigned char *)(&stBal), P->strData, P->nDataLen);
			memset(szDispAmt, 0, sizeof(szDispAmt));
			if (stBal.cAmount_Sign == 'C' || stBal.cAmount_Sign == 'c')
			{
				szDispAmt[0] = '+';
			}
			else
			{
				szDispAmt[0] = '-';
			}
			__TRACEHEX__(stBal.sAmount, 12);
			PubAmtToDispOrPnt(stBal.sAmount, szDispAmt + 1);
			//__TRACEASC__("szDispAmt:%s", szDispAmt);
			Public_DispBalance(szDispAmt);
			goto EXIT;
		}
	default:
		// ret = -1;
		break;
	}
	Public_DispResp(stSystem->szResponse, 20);
EXIT:
	ISO8583_Free(&ptr);
	return ret;
}
void SetStructInit(STSYSTEM *stSystem)
{
	char buf[100] = {0};
	int len;

	stSystem->psAddField = NULL;
	strcpy(stSystem->sProcCode, "000000"); // 3 ������
	strcpy(stSystem->szTrace, "000001");   // 11 POS��ˮ��

	strcpy(stSystem->szPosID, "12345678"); // 41 �ն˺�

	strcpy(stSystem->szShopID, "123456789012345"); // 42 �̻���
	strcpy(stSystem->szMoneyCode, "156");		   // 49 ���׻��Ҵ���

	memset(buf, 0, sizeof(buf));

	strcpy(stSystem->stField60.sBatchNum, "000001"); // 60.2 ���κ�
	strcpy(stSystem->szAmount, "000000000001");		 // 4 ���׽��
}

int Uapp_Sign(void)
{
	int nRet;
	unsigned int sendbaglen;
	unsigned char sendbag[1024] = {0};
	char szParam[2 + 1];
	char szDate[14 + 1] = {0};
	// int keyindex;
	unsigned char recbag[1024] = {0};
	int recbaglen, nLen;
	char *pszTitle = "ǩ��";
	STSYSTEM stSystem;

	memset(&stSystem, 0x00, sizeof(STSYSTEM));
	strcpy(stSystem.szMsgID, "0800");
	stSystem.cTransType = TT_CHECKIN;
	SetStructInit(&stSystem);

	// �鷢�Ͱ�
	// POSPCMD = 0;

	nRet = Pack_ISO8583(&stSystem, sendbag);

	if (nRet < 0)
	{
		//Public_MsgBox(pszTitle, "�������", 3);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips("Package error");
		Sys_Delay_ms(1000);
		return nRet;
	}
	sendbaglen = nRet;
	app_lvgl_clearClientBg();
	//api_lcd_clearScreen();
	//Sys_DisplayLine(1, pszTitle, 1, ALIGN_CENTER);
	//Sys_DisplayLine(2, (char *)"��������...", 0, ALIGN_CENTER);
	app_lvgl_msgTips("connecting...");
	// nRet = Public_CommConnect(0);

	// if (nRet != 0)
	// {
	// 	//Public_MsgBox(pszTitle, "����ʧ��", 3);
	// 	app_lvgl_clearClientBg();
	// 	app_lvgl_msgTips("connection failed");
	//	Sys_Delay_ms(1000);
	// 	return APP_FAIL;
	// }

	// recbaglen = Public_ConnectFlow(sendbag, sendbaglen, recbag);
	// __TRACEASC__("recbag");
	// __TRACEHEX__(recbag, recbaglen);
	// Public_CommHangUp(0);
	memcpy(recbag, "\x00\x8D\x60\x00\x00\x03\x01\x61\x32\x00\x32\x00\x01\x08\x10\x00\x38\x00\x01\x0A\xC0\x00\x14\x00\x00\x01\x16\x29\x22\x05\x20\x08\x12\x34\x56\x78\x33\x37\x30\x30\x30\x30\x32\x34\x33\x35\x33\x37\x30\x30\x31\x32\x33\x34\x35\x36\x37\x38\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x00\x11\x00\x00\x00\x01\x00\x40\x00\x60\x4A\x2D\xC7\x50\x9E\x20\xFD\x47\x60\xBF\x3F\xEE\xB8\xC8\xE6\x5F\xBB\x02\x56\x9C\x58\x9F\x9E\x0A\xFD\x9F\x4B\xBD\x00\x00\x00\x00\x00\x00\x00\x00\xD9\xA0\x4C\x87\x4A\x2D\xC7\x50\x9E\x20\xFD\x47\x60\xBF\x3F\xEE\xB8\xC8\xE6\x5F\xBB\x02\x56\x9C", 144);

	/************************��ʼ���******************/
	nRet = Unpack_ISO8583(&stSystem, recbag + 2, recbaglen - 2);
	/************************�������******************/

	if (nRet == APP_OK)
	{
		//Public_MsgBox(pszTitle, "ǩ���ɹ�", 3);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips("Sign in successful");
		app_dsp_play(0,"/ext/audio/english/dsp_loginsucc.wav","device login success", 1);
	}
	else
	{
		//Public_MsgBox(pszTitle, "ǩ��ʧ��", 3);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips("Sign in fail");
		app_dsp_play(0,"/ext/audio/english/dsp_loginfail.wav","device login fail", 1);
		
	}
	Sys_Delay_ms(5000);

	return nRet;
}

/*******************************************************************
//��������Upub_CopyWithPoint
//�������ܣ���src�е����ݼ���С���㿽����dst��
            ���srcΪ�մ�����dst="0.00"��
            ���src=x��    ��dst="0.0x"��
            ���src=xx��   ��dst="0.xx"��
            ���src=xxx��  ��dst="x.xx"��
            �Դ�����
//����������src[IN]:������������
            dst[OUT]:����������
//�������أ���
//����������dst�Ļ�������С����Ϊ5��ͬʱҪ����������������֮һ��
            ���ٱ�strlen(src)��2���������ٱ�src�Ļ�������С��1��
*********************************************************************/
void Upub_CopyWithPoint(char *src, char *dst)
{
    int sLen = 0;

    sLen = strlen(src);
    if (sLen == 0)
    {
        sprintf(dst, "0.00");
    }
    else if (sLen == 1)
    {
        sprintf(dst, "0.0%c", src[0]);
    }
    else if (sLen == 2)
    {
        sprintf(dst, "0.%c%c", src[0], src[1]);
    }
    else
    {
        memcpy(dst, src, sLen - 2);
        dst[sLen - 2] = '.';
        memcpy(dst + sLen - 1, src + sLen - 2, 2);
        dst[sLen + 1] = 0;
    }
}

/***********************************************
��  �ܣ��û�������
��  ��: Type[in]:       --������͡�0 ���ѽ�1 ���ֽ��
		Amount[out]:    --����Ľ�12�ֽ�ASCII�Ҷ��룬��0
����ֵ��RETURN_SUCC      �ɹ�
		RETURN_CANCEL    ����ȡ��(�û�����ȡ����)
**********************************************/
int EMV_InputAmount2(char Type)
{
	int numFlag = 1;
	char amountDisplay[50] = {0};

	unsigned char key;
	char dispFlag=0;//��һ����ʾ
    char sAmt[12 + 1] = { 0 };
	char amount[12 + 1] = { 0 };

	int len =0;
	app_lvgl_clearClientBg();
	while (numFlag)
	{
	    if(dispFlag==0||memcmp(sAmt,amount, 12) != 0)
    	{  
    	    memset(sAmt,0,sizeof(sAmt));
    	    strcpy(sAmt,amount);
			memset(amountDisplay,0,sizeof(amountDisplay));
			Upub_CopyWithPoint(sAmt, amountDisplay);
	
			strcpy(cardpayamount,amountDisplay);
			app_lvgl_inputShow("Input Amount", amountDisplay, 0, 0, 0, true);

			app_seg_showMoney(amountDisplay);

			dispFlag=1;
	    }
		key = Sys_GetKey(6000);
		if ((KEY_0 <= key) && (key <= KEY_9))
		{
	
		    if (len == 0 && key != KEY_0)
        	{
            	amount[len] = key;
				len++;
        	}
       	 	else if ((len > 0) && (len < 8))
        	{
           	 	amount[len] = key;
				len++;
        	}
		    else
		    {
		        api_beep_playing(0);
		    }
		}
		else if(key==KEY_ENTER)
		{
		    app_lvgl_clearClientBg();
			break;
		}
		else if(key==KEY_DELETE)
		{
		    if(len!=0)
		    {
		    	len--;
		    }
			amount[len]=0x00;
		}
		else if(key==KEY_CANCEL)
		{
		    cancelFlag=1;
		    return RETURN_CANCEL;
		}
		
	}
	memset(saveAmount, '0', 12 - len);
	memcpy(saveAmount + 12 - len, sAmt, len);
	return RETURN_SUCC;
}
static int CardTask(unsigned char type, int nTimeOut)
{
    int iRet=0;
	u32  startTick;
    iRet = Pub_EMVOpenCard(type);
	startTick=api_sys_tick();

	while (1) {
		if (api_sys_tick() - startTick > nTimeOut*1000) {
			break;
		}
		iRet = Pub_EMVCheckCard(type);
		if(iRet==2||iRet==1)
		{
		   break;
		}
		api_sys_msleep(500);
	}
	return iRet;
	
}

int Uapp_Sale(void)
{
	int nRet;
	unsigned int sendbaglen;
	unsigned char sendbag[1024] = {0};
	char szParam[2 + 1];
	char szDate[14 + 1] = {0};
	char amount[12 + 1] = {0};
	char dspAmt[50 + 1] = {0};
	// int keyindex;
	int TransType=0;
	unsigned char recbag[1024] = {0};
	int recbaglen, nLen;
	char *pszTitle = "����";
	STSYSTEM stSystem;
	APPEMVINFO emvInfo;
	char cardNum[19+1]={0};
    char pinBuf[32]={0};
	memset(&stSystem, 0x00, sizeof(STSYSTEM));
		memset(&emvInfo, 0x00, sizeof(APPEMVINFO));
	strcpy(stSystem.szMsgID, "0200");
	stSystem.cTransType = TT_CARDSALE;
	SetStructInit(&stSystem);

    
	cancelFlag = 0;
	nRet = EMV_InputAmount2(stSystem.cTransType);
	app_seg_showMoney("0.00"); 
	if (cancelFlag == 1)
	{
		return;
	}
	app_lvgl_clearClientBg();
	app_lvgl_msgTips("Please swipe your card");

	card_read_method=CardTask(CHECK_INSERTIC|CHECK_RFCARD,60);

	if(card_read_method==1||card_read_method==2)
	{
	    if (card_read_method == 2)
		{
			TransType = 2;
		}
		if (card_read_method == 1)
		{
			TransType = 3;
		}
		gTraceNo = ++gTraceNo % 10000000;
		sprintf(stSystem.szTrace,"%06d",gTraceNo);
	    if(card_read_method == 1)
			app_beep_play();
	    nRet = Pub_EMVProcessApp(card_read_method, cardNum,&emvInfo);
		API_LOG_DEBUG("EMV_Process ret:%d\r\n", nRet);
		if (nRet < 0 ) 
		{			
			app_dsp_play(0, "/ext/audio/english/dsp_transactioncancel.wav", "Transaction Cancelled", 0);
			Pub_EMVCompleteApp(false,NULL,0);

			return 0;
		}
	}
	else
	{
	    return 0;
	}

    stSystem.nAddFieldLen=emvInfo.cFieldlen;
	stSystem.psAddField=emvInfo.sField55;
	strcpy(stSystem.szTrack2,emvInfo.sztrack2);
	strcpy(stSystem.szPan,emvInfo.szPan);
	strcpy(stSystem.szCardSerialNo,emvInfo.szCardSerialNo);
    memcpy(stSystem.szAmount,saveAmount,12);
	
	// int pinLen = EMV_GetKernelData(TAG_PIN, stSystem.szPin);

	// Util_Bcd2Asc(stSystem.szPin,pinBuf, pinLen * 2);
	// __TRACEASCDATA__("pin : %s", pinBuf);

	// �鷢�Ͱ�
	// POSPCMD = 0;
	nRet = Pack_ISO8583(&stSystem, sendbag);

	if (nRet < 0)
	{
		//Public_MsgBox(pszTitle, "�������", 3);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips("Package error");
		Sys_Delay_ms(1000);
		Pub_EMVCompleteApp(false, NULL, 0);
		// �ͷ��ڴ�
		EMV_Free();
		return nRet;
	}
	sendbaglen = nRet;

	__TRACEHEX__(sendbag, sendbaglen);
	// api_lcd_clearScreen();
	// Sys_DisplayLine(1, pszTitle, 1, ALIGN_CENTER);
	// Sys_DisplayLine(2, (char *)"��������...", 0, ALIGN_CENTER);
	app_lvgl_clearClientBg();
	app_lvgl_msgTips("connecting...");
	// nRet = Public_CommConnect(0);

	// if (nRet != 0)
	// {
	// 	Public_MsgBox(pszTitle, "����ʧ��", 3);
	// 	return APP_FAIL;
	// }

	// recbaglen = Public_ConnectFlow(sendbag, sendbaglen, recbag);

	// Public_CommHangUp(0);
	api_sys_msleep(2000);
	memcpy(recbag, "\x00\x8E\x60\x00\x00\x03\x01\x61\x32\x00\x32\x00\x01\x02\x10\x70\x3E\x00\x01\x0A\xD0\x80\x13\x19\x62\x22\x62\x31\x10\x01\x12\x77\x94\x20\x00\x00\x00\x00\x42\x94\x96\x92\x96\x00\x00\x01\x14\x09\x03\x05\x15\x00\x00\x12\x12\x05\x12\x34\x50\x33\x37\x30\x30\x30\x30\x30\x30\x37\x34\x33\x37\x30\x30\x31\x32\x33\x34\x35\x36\x37\x38\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x22\x34\x34\x30\x31\x30\x30\x30\x30\x30\x30\x30\x34\x34\x30\x32\x30\x30\x30\x30\x30\x30\x30\x31\x35\x36\x00\x13\x22\x00\x00\x01\x00\x05\x00\x00\x03\x43\x55\x50\x46\x44\x35\x46\x44\x35\x31\x41", 145);
	__TRACEHEX__(recbag, recbaglen);
	recbaglen=145;
	/************************��ʼ���******************/
	nRet = Unpack_ISO8583(&stSystem, recbag + 2, recbaglen - 2);
	/************************�������******************/
	
	//__TRACEASC__("nRet = %d", nRet);
	
	sprintf(dspAmt, "%d.%02d", atoi(stSystem.szAmount)/100,atoi(stSystem.szAmount)%100);
	//__TRACEASC__("dspAmt = %s", dspAmt);

	if (nRet == APP_OK)
	{
		//Public_MsgBox(pszTitle, "���ѳɹ�", 3);
		app_lvgl_clearClientBg();
		//app_lvgl_msgTips("Consumption success");
		app_transaction_resultShow(TransType, 1, dspAmt, stSystem.szTrace, NULL);
		app_dsp_play(0,"/ext/audio/english/dsp_transactionsucc.wav","transaction success", 1);
		
	}
	else
	{
		//Public_MsgBox(pszTitle, "����ʧ��", 3);
		app_lvgl_clearClientBg();
		//app_lvgl_msgTips("Consumption fail");
		app_transaction_resultShow(TransType, 2, dspAmt, stSystem.szTrace, NULL);
		app_dsp_play(0,"/ext/audio/english/dsp_transactionfail.wav","transaction fail", 1);
	
	}
	Pub_EMVCompleteApp(true, NULL, 0);
	// �ͷ��ڴ�
	EMV_Free();
	Sys_Delay_ms(5000);

	return nRet;
}

int Uapp_Void(void)
{
	int nRet;
	unsigned int sendbaglen;
	unsigned char sendbag[1024] = {0};
	char szParam[2 + 1];
	char szDate[14 + 1] = {0};
	// int keyindex;
	unsigned char recbag[1024] = {0};
	int recbaglen, nLen;
	char *pszTitle = "����";
	STSYSTEM stSystem;

	memset(&stSystem, 0x00, sizeof(STSYSTEM));
	/**
	 * �ɼ�����
	 */
	strcpy(stSystem.szMsgID, "0200");
	stSystem.cTransType = TT_VOID;
	SetStructInit(&stSystem);
	strcpy(stSystem.sProcCode, "200000"); // 3 ������
	memcpy(stSystem.szServerCode, "00", 2);
	memcpy(stSystem.stField60.sFuncCode, "23", 2);

	// �鷢�Ͱ�
	// POSPCMD = 0;
	strcpy(stSystem.szPan, "1234567890123456");
	nRet = Pack_ISO8583(&stSystem, sendbag);

	if (nRet < 0)
	{
		//Public_MsgBox(pszTitle, "�������", 3);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips("Package error");
		Sys_Delay_ms(1000);
		return nRet;
	}
	sendbaglen = nRet;

	__TRACEHEX__(sendbag, sendbaglen);
	api_lcd_clearScreen();
	// Sys_DisplayLine(1, pszTitle, 1, ALIGN_CENTER);
	// Sys_DisplayLine(2, (char *)"��������...", 0, ALIGN_CENTER);
	app_lvgl_clearClientBg();
	app_lvgl_msgTips("connecting...");
	// nRet = Public_CommConnect(0);

	// if (nRet != 0)
	// {
	// 	//Public_MsgBox(pszTitle, "����ʧ��", 3);
	// 	app_lvgl_clearClientBg();
	// 	app_lvgl_msgTips("connection failed");
	//	Sys_Delay_ms(1000);
	// 	return APP_FAIL;
	// }

	// recbaglen = Public_ConnectFlow(sendbag, sendbaglen, recbag);

	// Public_CommHangUp(0);
	
	// __TRACEHEX__(recbag, recbaglen);

	memcpy(recbag, "\x00\x83\x60\x00\x00\x03\x01\x61\x32\x00\x32\x00\x01\x02\x10\x30\x3A\x00\x81\x0A\xD0\x80\x13\x20\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\x09\x20\x45\x05\x21\x12\x12\x00\x08\x12\x34\x56\x78\x33\x37\x30\x30\x30\x30\x34\x39\x33\x63\x33\x37\x30\x30\x31\x32\x33\x34\x35\x36\x37\x38\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x22\x34\x34\x30\x31\x30\x30\x30\x30\x30\x30\x30\x34\x34\x30\x32\x30\x30\x30\x30\x30\x30\x30\x31\x35\x36\x00\x13\x23\x00\x00\x01\x00\x05\x00\x00\x03\x43\x55\x50\x33\x44\x46\x35\x38\x30\x42\x32",134);
	/************************��ʼ���******************/
	nRet = Unpack_ISO8583(&stSystem, recbag + 2, recbaglen - 2);
	/************************�������******************/

	if (nRet == APP_OK)
	{
		//Public_MsgBox(pszTitle, "�����ɹ�", 3);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips("Revocation successful");
		Sys_Delay_ms(1000);
	}
	else
	{
		//Public_MsgBox(pszTitle, "����ʧ��", 3);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips("Revocation fail");
		Sys_Delay_ms(1000);
	}

	return nRet;
}

int Uapp_Refund(void)
{
	int nRet;
	unsigned int sendbaglen;
	unsigned char sendbag[1024] = {0};
	char szParam[2 + 1];
	char szDate[14 + 1] = {0};
	// int keyindex;
	unsigned char recbag[1024] = {0};
	int recbaglen, nLen;
	char *pszTitle = "�˻�";
	STSYSTEM stSystem;

	memset(&stSystem, 0x00, sizeof(STSYSTEM));
	strcpy(stSystem.szMsgID, "0220");
	stSystem.cTransType = TT_REFUND;
	SetStructInit(&stSystem);
	memcpy(stSystem.sProcCode, "200000", 6);
	memcpy(stSystem.szServerCode, "00", 2);
	memcpy(stSystem.stField60.sFuncCode, "25", 2);

	// �鷢�Ͱ�
	// POSPCMD = 0;
	strcpy(stSystem.szPan, "1234567890123456");
	nRet = Pack_ISO8583(&stSystem, sendbag);

	if (nRet < 0)
	{
		//Public_MsgBox(pszTitle, "�������", 3);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips("Package error");
		Sys_Delay_ms(1000);
		return nRet;
	}
	sendbaglen = nRet;

	__TRACEHEX__(sendbag, sendbaglen);
	api_lcd_clearScreen();
	// Sys_DisplayLine(1, pszTitle, 1, ALIGN_CENTER);
	// Sys_DisplayLine(2, (char *)"��������...", 0, ALIGN_CENTER);
	app_lvgl_clearClientBg();
	app_lvgl_msgTips("connecting...");
	// nRet = Public_CommConnect(0);

	// if (nRet != 0)
	// {
	// 	//Public_MsgBox(pszTitle, "����ʧ��", 3);
	// 	app_lvgl_clearClientBg();
	// 	app_lvgl_msgTips("connection failed");
	//	Sys_Delay_ms(1000);
	// 	return APP_FAIL;
	// }

	// recbaglen = Public_ConnectFlow(sendbag, sendbaglen, recbag);

	// Public_CommHangUp(0);
	
	memcpy(recbag, "\x00\x85\x60\x00\x00\x03\x01\x61\x32\x00\x32\x00\x01\x02\x30\x30\x3A\x02\x81\x0A\xD0\x80\x13\x20\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\x09\x55\x26\x05\x21\x12\x12\x00\x99\x00\x08\x12\x34\x56\x78\x33\x37\x30\x30\x30\x30\x32\x61\x33\x63\x33\x37\x30\x30\x31\x32\x33\x34\x35\x36\x37\x38\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x22\x34\x34\x30\x31\x30\x30\x30\x30\x30\x30\x30\x34\x34\x30\x32\x30\x30\x30\x30\x30\x30\x30\x31\x35\x36\x00\x13\x25\x00\x00\x01\x00\x05\x00\x00\x03\x43\x55\x50\x42\x38\x36\x37\x39\x31\x45\x46", 136);
	__TRACEHEX__(recbag, recbaglen);

	/************************��ʼ���******************/
	nRet = Unpack_ISO8583(&stSystem, recbag + 2, recbaglen - 2);
	/************************�������******************/

	if (nRet == APP_OK)
	{
		//Public_MsgBox(pszTitle, "�˻��ɹ�", 3);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips("Refund successful");
		Sys_Delay_ms(1000);
	}
	else
	{
		//Public_MsgBox(pszTitle, "�˻�ʧ��", 3);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips("Refund fail");
		Sys_Delay_ms(1000);
	}

	return nRet;
}
int Uapp_Balance(void)
{
	int nRet;
	unsigned int sendbaglen;
	unsigned char sendbag[1024] = {0};
	char szParam[2 + 1];
	char szDate[14 + 1] = {0};
	// int keyindex;
	unsigned char recbag[1024] = {0};
	int recbaglen, nLen;
	char *pszTitle = "����ѯ";
	STSYSTEM stSystem;

	memset(&stSystem, 0x00, sizeof(STSYSTEM));
	strcpy(stSystem.szMsgID, "0200");
	stSystem.cTransType = TT_BALANCE;
	SetStructInit(&stSystem);
	memcpy(stSystem.sProcCode, "310000", 6);
	memcpy(stSystem.szServerCode, "00", 2);
	memcpy(stSystem.stField60.sFuncCode, "01", 2);

	// �鷢�Ͱ�
	// POSPCMD = 0;
	strcpy(stSystem.szPan, "1234567890123456");
	nRet = Pack_ISO8583(&stSystem, sendbag);

	if (nRet < 0)
	{
		//Public_MsgBox(pszTitle, "�������", 3);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips("Package error");
		Sys_Delay_ms(1000);
		return nRet;
	}
	sendbaglen = nRet;

	__TRACEHEX__(sendbag, sendbaglen);
	//api_lcd_clearScreen();
	// Sys_DisplayLine(1, pszTitle, 1, ALIGN_CENTER);
	// Sys_DisplayLine(2, (char *)"��������...", 0, ALIGN_CENTER);
	app_lvgl_clearClientBg();
	app_lvgl_msgTips("connecting...");
	// nRet = Public_CommConnect(0);

	// if (nRet != 0)
	// {
	// 	//Public_MsgBox(pszTitle, "����ʧ��", 3);
	// 	app_lvgl_clearClientBg();
	// 	app_lvgl_msgTips("connection failed");
	// 	return APP_FAIL;
	// }

	// recbaglen = Public_ConnectFlow(sendbag, sendbaglen, recbag);

	// Public_CommHangUp(0);
	memcpy(recbag, "\x00\x8D\x60\x00\x00\x03\x01\x61\x32\x00\x32\x00\x01\x02\x10\x60\x3C\x00\x81\x0A\xD0\x84\x11\x16\x12\x34\x56\x78\x90\x12\x34\x56\x31\x00\x00\x00\x00\x01\x10\x08\x26\x05\x21\x00\x00\x00\x08\x12\x34\x56\x78\x33\x37\x30\x30\x30\x30\x36\x39\x33\x33\x33\x37\x30\x30\x31\x32\x33\x34\x35\x36\x37\x38\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x22\x34\x34\x30\x31\x30\x30\x30\x30\x30\x30\x30\x34\x34\x30\x32\x30\x30\x30\x30\x30\x30\x30\x31\x35\x36\x00\x10\x30\x30\x30\x30\x30\x30\x30\x43\x32\x33\x00\x13\x01\x00\x00\x01\x00\x05\x00\x45\x38\x39\x44\x45\x37\x45\x44", 144);
	__TRACEHEX__(recbag, recbaglen);

	/************************��ʼ���******************/
	nRet = Unpack_ISO8583(&stSystem, recbag + 2, recbaglen - 2);
	/************************�������******************/

	/*if (nRet == APP_OK)
	{
		Public_MsgBox(pszTitle ,"���ѳɹ�",3);
	}
	else
	{
		Public_MsgBox(pszTitle ,"����ʧ��",3);
	}*/

	return nRet;
}

int Emqx_Sale(char *amt)
{
	int nRet;
	unsigned int sendbaglen;
	unsigned char sendbag[1024] = {0};
	char szParam[2 + 1];
	char szDate[14 + 1] = {0};
	char amount[12 + 1] = {0};
	char dspAmt[50 + 1] = {0};
	// int keyindex;
	int TransType=0;
	unsigned char recbag[1024] = {0};
	int recbaglen, nLen;
	STSYSTEM stSystem;
	APPEMVINFO emvInfo;
	char cardNum[19+1]={0};
    char pinBuf[32]={0};
	memset(&stSystem, 0x00, sizeof(STSYSTEM));
		memset(&emvInfo, 0x00, sizeof(APPEMVINFO));
	strcpy(stSystem.szMsgID, "0200");
	stSystem.cTransType = TT_CARDSALE;
	SetStructInit(&stSystem);

	cancelFlag = 0;
	// nRet = EMV_InputAmount2(stSystem.cTransType);
	// app_seg_showMoney("0.00"); 
	memset(saveAmount, '0', 12 - strlen(amt));
	memcpy(saveAmount + 12 - strlen(amt), amt, strlen(amt));

	memset(cardpayamount, '0', 12 - strlen(amt));
	memcpy(cardpayamount + 12 - strlen(amt), amt, strlen(amt));
	Upub_CopyWithPoint(amt,dspAmt);
	app_seg_showMoney(dspAmt);

	API_LOG_DEBUG("EMV_Process amt:%s\r\n", amt);
	if (cancelFlag == 1)
	{
		return -1;
	}
	app_lvgl_clearClientBg();
	app_lvgl_msgTips("Please swipe your card");

	card_read_method=CardTask(CHECK_INSERTIC|CHECK_RFCARD,60);

	if(card_read_method==1||card_read_method==2)
	{
	    if (card_read_method == 2)
		{
			TransType = 2;
		}
		if (card_read_method == 1)
		{
			TransType = 3;
		}
		gTraceNo = ++gTraceNo % 10000000;
		sprintf(stSystem.szTrace,"%06d",gTraceNo);
	    if(card_read_method == 1)
			app_beep_play();
	    // nRet = Pub_EMVProcessApp(card_read_method, cardNum,&emvInfo);
		API_LOG_DEBUG("EMV_Process ret:%d\r\n", nRet);
		if (nRet < 0 ) 
		{			
			app_dsp_play(0, "/ext/audio/english/dsp_transactioncancel.wav", "Transaction Cancelled", 0);
			Pub_EMVCompleteApp(false,NULL,0);

			return 0;
		}
	}
	else
	{
	    return 0;
	}

    stSystem.nAddFieldLen=emvInfo.cFieldlen;
	stSystem.psAddField=emvInfo.sField55;
	strcpy(stSystem.szTrack2,emvInfo.sztrack2);
	strcpy(stSystem.szPan,emvInfo.szPan);
	strcpy(stSystem.szCardSerialNo,emvInfo.szCardSerialNo);
    memcpy(stSystem.szAmount,saveAmount,12);
	
	// int pinLen = EMV_GetKernelData(TAG_PIN, stSystem.szPin);

	// Util_Bcd2Asc(stSystem.szPin,pinBuf, pinLen * 2);
	// __TRACEASCDATA__("pin : %s", pinBuf);

	// �鷢�Ͱ�
	// POSPCMD = 0;
	nRet = Pack_ISO8583(&stSystem, sendbag);

	if (nRet < 0)
	{
		//Public_MsgBox(pszTitle, "�������", 3);
		app_lvgl_clearClientBg();
		app_lvgl_msgTips("Package error");
		Sys_Delay_ms(1000);
		Pub_EMVCompleteApp(false, NULL, 0);
		// �ͷ��ڴ�
		EMV_Free();
		return nRet;
	}
	sendbaglen = nRet;

	__TRACEHEX__(sendbag, sendbaglen);
	// api_lcd_clearScreen();
	// Sys_DisplayLine(1, pszTitle, 1, ALIGN_CENTER);
	// Sys_DisplayLine(2, (char *)"��������...", 0, ALIGN_CENTER);
	app_lvgl_clearClientBg();
	app_lvgl_msgTips("connecting...");
	// nRet = Public_CommConnect(0);

	// if (nRet != 0)
	// {
	// 	Public_MsgBox(pszTitle, "����ʧ��", 3);
	// 	return APP_FAIL;
	// }

	// recbaglen = Public_ConnectFlow(sendbag, sendbaglen, recbag);

	// Public_CommHangUp(0);
	api_sys_msleep(2000);
	memcpy(recbag, "\x00\x8E\x60\x00\x00\x03\x01\x61\x32\x00\x32\x00\x01\x02\x10\x70\x3E\x00\x01\x0A\xD0\x80\x13\x19\x62\x22\x62\x31\x10\x01\x12\x77\x94\x20\x00\x00\x00\x00\x42\x94\x96\x92\x96\x00\x00\x01\x14\x09\x03\x05\x15\x00\x00\x12\x12\x05\x12\x34\x50\x33\x37\x30\x30\x30\x30\x30\x30\x37\x34\x33\x37\x30\x30\x31\x32\x33\x34\x35\x36\x37\x38\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x22\x34\x34\x30\x31\x30\x30\x30\x30\x30\x30\x30\x34\x34\x30\x32\x30\x30\x30\x30\x30\x30\x30\x31\x35\x36\x00\x13\x22\x00\x00\x01\x00\x05\x00\x00\x03\x43\x55\x50\x46\x44\x35\x46\x44\x35\x31\x41", 145);
	__TRACEHEX__(recbag, recbaglen);
	recbaglen=145;
	/************************��ʼ���******************/
	nRet = Unpack_ISO8583(&stSystem, recbag + 2, recbaglen - 2);
	/************************�������******************/
	
	//__TRACEASC__("nRet = %d", nRet);
	
	sprintf(dspAmt, "%d.%02d", atoi(stSystem.szAmount)/100,atoi(stSystem.szAmount)%100);
	//__TRACEASC__("dspAmt = %s", dspAmt);

	if (nRet == APP_OK)
	{
		//Public_MsgBox(pszTitle, "���ѳɹ�", 3);
		app_lvgl_clearClientBg();
		//app_lvgl_msgTips("Consumption success");
		app_transaction_resultShow(TransType, 1, dspAmt, stSystem.szTrace, NULL);
		app_dsp_play(0,"/ext/audio/english/dsp_transactionsucc.wav","transaction success", 1);
		
	}
	else
	{
		//Public_MsgBox(pszTitle, "����ʧ��", 3);
		app_lvgl_clearClientBg();
		//app_lvgl_msgTips("Consumption fail");
		app_transaction_resultShow(TransType, 2, dspAmt, stSystem.szTrace, NULL);
		app_dsp_play(0,"/ext/audio/english/dsp_transactionfail.wav","transaction fail", 1);
	
	}
	Pub_EMVCompleteApp(true, NULL, 0);
	// �ͷ��ڴ�
	EMV_Free();
	Sys_Delay_ms(5000);

	return nRet;
}
