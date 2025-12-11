#include "app_lvgl.h"
#include "app_pub.h"
#include "app_trans.h"
#include "app_button.h"

#include "app_card_payment.h"
#include "app_dsp.h"
#include "app_initResource.h"
#include "app_callback.h"
#include "app_mutex.h"
#include "tms_term_mng.h"
#include "app_emqx.h"

#include "dsp_tms.h"
#include "app_button.h"

u32 thread_mainID = 0;
u32 thread_buttonID = 0;
u32 thread_qrCheckID = 0;
u32 thread_hibernateID = 0;
u32 thread_dynamicqrID = 0;
u32 thread_emqxID = 0;

u8  tsn[64] = { 0 };
u8  tusn[64] = { 0 };
u32 tusnlen = 0;
u32 tsnlen = 0;


Language currentLanguage = LANGUAGE_ENGLISH;
int  g_boxReady = 0;
unsigned char MainStatus=0;

static void prvInvokeGlobalCtors(void)
{
	extern void (*__init_array_start[])();
	extern void (*__init_array_end[])();
	size_t count = __init_array_end - __init_array_start;
	for (size_t i = 0; i < count; ++i)
		__init_array_start[i]();
}



static int create_thread(const char* name, void (*entry)(void*), 
                        int stack_size, int priority, void* thread_id) {
                        int iRet=0;
						iRet=OsSysThreadCreate(entry, name, stack_size, NULL, priority, thread_id);
						
    return iRet;
}

#define TMS_FW_HEART_CUSTOM_URL               "https://www.dspreadser.net:9011/terminal"  
void app_main(void* param)
{
	int ret = -1;
	int i   = -1;
	char rtcTimeStrBcd[14+1] = { 0 };
	char rtcTimeStr[14+1] = { 0 };
	u8 sim_status = 0;
    char logSwitch=0;
	char errorMsg[64] = {0};
	u32 startTick;
	int ilen=0;
	int Tms_status=0;
	hwc_list_t hwcdata;
	lcdType = true;
	memset(serialID,0,sizeof(serialID));
	memset(tsn,0,sizeof(tsn));
	memset(tusn,0,sizeof(tusn));	
	app_reg_iconEvent();
	app_trans_init();
	EmvL2_Init();

	App_PosParamInit();
	
	loadResource();
	ret = OsSysReadSn(tsn,&tsnlen,tusn,&tusnlen);
	API_LOG_DEBUG("tms tsn=[%s],tusn=[%s]",tsn,tusn);
	if(strlen(tusn) == 0)
	{
		app_lvgl_mainShow(DSP_SYSINIT_IMG, 100, 100, "Device serial number is empty", LV_COLOR_DSP_BLUE);
		OsSysMsleep(3000);
	}	
    strncpy(serialID,tsn,strlen(tsn));
	API_LOG_DEBUG("SN=[%s]",serialID);
    memset(&hwcdata,0,sizeof(hwc_list_t));
    ret = OsSysGetHwc(&hwcdata);
	if(ret != 0 )
	{
		app_lvgl_mainShow(DSP_SYSINIT_IMG, 100, 100, "System Initialize Failed", LV_COLOR_DSP_BLUE);
		OsSysMsleep(3000);
	}
	API_LOG_DEBUG("LCD=[%u]",hwcdata.LCD);
	if(hwcdata.LCD != 0 && hwcdata.LCD != 1 && hwcdata.LCD != 2 && hwcdata.LCD != 3 )
		lcdType = false;
	app_dsp_play(0, "/ext/mp3/welcome.mp3", "Welcome", 1);
	app_lvgl_mainShow(DSP_WELCOME_IMG, 100, 100, "Welcome", LV_COLOR_DSP_BLUE);	
	ret=pub_GetNetWorkStatus();
	if (ret != 0) {
		OsSysGetSpTime(rtcTimeStrBcd);
		sprintf(rtcTimeStr,"20%02x%02x%02x%02x%02x%02x",rtcTimeStrBcd[0], rtcTimeStrBcd[1], rtcTimeStrBcd[2], rtcTimeStrBcd[3],
			rtcTimeStrBcd[4], rtcTimeStrBcd[5]);
		API_LOG_DEBUG( "setRtc ret:%d\r\n",OsSysSetRtcTime(rtcTimeStr));
		app_dsp_play(0,"/ext/audio/english/dsp_netabnormal.wav","The network failed", 1);
		app_lvgl_mainShow(DSP_NETWORKFAIL_IMG, 100, 100, "The network failed", LV_COLOR_DSP_BLUE);
		OsSysMsleep(500);
		app_dsp_play(0,"/ext/audio/english/dsp_connectfail.wav","Network Nonnection failed", 1);
		app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100, "Network connection failed", LV_COLOR_DSP_BLUE);
		OsSysMsleep(500);
	}
	else {
		if(OsParamsGetNetMode() == 0)
		{
			API_LOG_DEBUG("OsWifiSntpCfg 8");
			OsWifiSntpCfg(1,8,"0.pool.ntp.org","time.google.com", NULL);
		}
		app_dsp_play(0, "/ext/audio/english/dsp_netnormal.wav", "Network Connected", 1);
		app_lvgl_mainShow(DSP_NETWORKSUCC_IMG, 100, 100, "Network Connected", LV_COLOR_DSP_BLUE);
	}
	larktms_service_start(showTmsState,TMS_FW_HEART_CUSTOM_URL,APP_VERSION);
	u32 btn_thread_id = 0;
	OsSysThreadCreate(app_trans_process, "app_trans",
		24*1024, NULL,
		API_PRIORITY_NORMAL,
		&btn_thread_id);

	OsSysThreadCreate(app_hibernate, "app_hibernate", 1024 * 4, NULL, API_PRIORITY_NORMAL-4, &thread_hibernateID);

	g_boxReady = 1;
	app_button_semSig();
	app_standby_wait();	
	handle_menu(0);
exit:
	MainStatus=1;
	OsSysThreadDelete(thread_mainID);
	return;
}

void* appimg_enter(void* param)
{
	int* p = NULL;
    int Tms_status=0;
	char logSwitch=0;
	int ilen=0;
	int ret;
	OsSysWatchDogDisable();
	prvInvokeGlobalCtors();
	OsSysMsleep(1000);
	p = OsSysInit(APP_VERSION);
	OsSysMsleep(1000);
	OsLogSwitch(0);
	OsKeypadSetMode(1);
	lv_drv_init();
	OsAudioTtsSetSpeed(5000);
	rpc_protocol_task();
	mutex_lvgl = OsSysMutexCreate();
	MainStatus=0;
	if(create_thread("app_main", app_main, 1024 * 100, 
                API_PRIORITY_NORMAL-4, &thread_mainID) != 0) {
   		API_LOG_DEBUG("[ERROR] Failed to create app_main thread");
	}
	if(create_thread("app_button", app_mon_button, 1024 * 32, 
                API_PRIORITY_NORMAL-1, &thread_buttonID) != 0) {
   		API_LOG_DEBUG("[ERROR] Failed to create app_button thread");
	}
	return p;
}
void appimg_exit(void)	
{

}
