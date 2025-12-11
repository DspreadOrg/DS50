#ifndef _API_USB_H
#define _API_USB_H
#include "api_types.h"

 
s32 OsUsbOpen(void);

 
s32 OsUsbClose(void);
 
s32 OsUsbRead(u8 *lpOut, u32 nLe);
 
s32 OsUsbWrite(u8 *lpIn, u32 nLe);
 
s32 OsUsbReset(void);
 
s32 OsUsbIoctl(u32 nCmd, u32 lParam, u32 wParam);


 
s32 OsUsbIsConnected();









#endif
