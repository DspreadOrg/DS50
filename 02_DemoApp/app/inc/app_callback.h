#ifndef __APP_CALLBACK_H__
#define __APP_CALLBACK_H__

int app_check_serverStatus(void);

void showTmsState(int type, void* args);
void showTmsProgress(int cur, int total);


void app_reg_iconEvent();

#endif