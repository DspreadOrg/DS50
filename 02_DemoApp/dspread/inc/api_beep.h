#ifndef __API_BEEP_H
#define __API_BEEP_H
#include "api_types.h"


s32 OsBeepOpen(void);
s32 OsBeepClose(void);
s32 OsBeepIoctl(u32 nCmd,u32 lParam,u32 sParam);
s32 OsBeepPlay(u32 times, u32 ontime, u32 offtime);
s32 OsBeepPlaying(int frequency);

#endif
