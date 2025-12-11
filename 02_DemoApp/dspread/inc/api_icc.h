#ifndef __API_ICC_H
#define __API_ICC_H
#include "api_types.h"
#include "api_card.h"


S32 OsIcCardOpen(int slot);
S32 OsIcCardlose(int slot);
S32 OsIcCardPowerOn(int slot,u8 *lpAtr);
S32 OsIcCardPowerOff(int slot);
S32 OsIcCardGetStatus(int slot);
S32 OsIcCardExchangeApdu(int slot,APDU_SEND *ApduSend, APDU_RESP *ApduResp);
S32 OsIcCardIoctl(u32 nCmd, u32 lParam, u32 wParam);
#endif
