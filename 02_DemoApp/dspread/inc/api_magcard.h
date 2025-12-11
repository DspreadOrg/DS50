#ifndef __API_MAGCARD_H
#define __API_MAGCARD_H

#include "api_types.h"

S32 OsMagOpen(void);
S32 OsMagClose(void);
S32 OsMagClear(void);
S32 OsMagRead(u8 *lpTrack1, u8 *lpTrack2, u8 *lpTrack3);
S32 OsMagIoctl(u32 nCmd, u32 lParam, u32 wParam);
#endif
