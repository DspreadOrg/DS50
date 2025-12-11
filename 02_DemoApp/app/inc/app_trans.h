#ifndef __APP_TRANS_H__
#define __APP_TRANS_H__

#define AUTO_SLEEP_TIME 60*2

int trans_thread_mutexLock(void);
int trans_thread_mutexUnlock(void);

void app_trans_init(void);
void app_trans_process(void *param);
void app_transthread_paycheck(void* params);

void app_hibernate_reset(int value);
void app_hibernate(void* param);

void app_static_qr(void* param);

#endif

