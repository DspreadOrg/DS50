#ifndef _API_TIMER_H
#define _API_TIMER_H
#include "api_types.h"
 
u32 OsTimerCreate(unsigned int timer_ms, bool single, void (*fn)(void* arg), void* arg);

 
bool OsTimerStop(unsigned int timerid);

 
int OsTimerFree(unsigned int timerid);



#endif