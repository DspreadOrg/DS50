#ifndef _SDK_H_
#define _SDK_H_
#include "api.h"
#include "app_param.h"

#ifndef RETURN_SUCC
#define RETURN_SUCC       0   
#endif
#ifndef RETURN_FAIL
#define RETURN_FAIL      (-1)   
#endif
#ifndef RETURN_ERR_PARAM
#define RETURN_ERR_PARAM (-2)   
#endif
#ifndef RETURN_CANCEL
#define RETURN_CANCEL    (-3)   
#endif
#ifndef RETURN_MAC_ERR
#define RETURN_MAC_ERR   (-4)   
#endif
#ifndef RETURN_TIMEOUT
#define RETURN_TIMEOUT   (-5)    
#endif
#ifndef RETURN_MEMFAIL
#define RETURN_MEMFAIL   (-6)    
#endif
#ifndef RETURN_BYPASSPIN
#define RETURN_BYPASSPIN (-7)   
#endif

#define IC_CARD_NO      0
#define SAM1_CARD_NO    1
#define SAM2_CARD_NO    2
#define SAM3_CARD_NO    3
#define SAM4_CARD_NO    4

#define PICC_TYPE_A		0x01//type a card
#define PICC_TYPE_B		0x02//type b card
#define PICC_TYPE_AB	0x03//type a card and type b card simultaneously
#define PICC_TYPE_M1	0x04//M1card

typedef struct
{
	int ATR_DataLen;
	unsigned char ATR_Data[100];
} SMART_ATR;

typedef struct
{
	unsigned char cardType;	    
	unsigned char uidLen;       
	unsigned char uidBuf[16];	 
} CARD_UID;

typedef struct _Node
{
	unsigned int nKey;
	struct _Node *next;
	unsigned char *strData;
	unsigned short nDataLen;
}*LNode, *LinkList;

 
#endif










