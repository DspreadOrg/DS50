#ifndef __API_RFC_H
#define __API_RFC_H
#include "api_types.h"
#include "api_card.h"

s32 OsRfOpen(void);
s32 OsRfClose(void);
s32 OsRfPownOn(u32 nType);
s32 OsRfPowerOff(void);
s32 OsRfGetStatus(void);
s32 OsRfActivate(void);
s32 OsRfExchangeApdu(APDU_SEND *ApduSend, APDU_RESP *ApduResp);
s32 OsRfRemove(void);
s32 OsRfIoctl(u32 nCmd, u32 lParam,u32 wParam);
s32 OsRfAuthority(u8 Type,u8 BlkNo,u8 *Pwd,u8 *SerialNo);
s32 OsRfReadBlock(u8 BlkNo,u8 *BlkValue);
s32 OsRfWriteBlock(u8 BlkNo,u8 *BlkValue);
#endif
