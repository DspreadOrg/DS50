#ifndef __API_SEG_LED_H
#define __API_SEG_LED_H
#include "api_types.h"


typedef enum tagCsqType
{
	CSQ_TYPE_MOBILE,
	CSQ_TYPE_WIFI,
}CSQ_TYPE;

int OsSegInit(void);

int OsSegClear(void);

/*
*@Brief:		��ʾ/���С����
*@Param IN:		isShow: true - ��ʾС����; false - ���С����
*/
int OsSegShowDot(bool isShow);
/*
*@Brief:		��ʾ��ص���
*@Param IN:		level��ȡֵ0~4��
*/
int OsSegShowSoc(u8 level);
 
int OsSegShowCsq(CSQ_TYPE type, u8 level);

 
int OsSegShowMoney(char* str);
int OsSegShowChar(char *str);


void OsSegStatusbarSigReg(void (*fun)(int type, int value));
#endif

