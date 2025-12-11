#include "lvgl.h"
#include "app_callback.h"
#include "app_pub.h"
#include "app_lvgl.h"
#include "dsp_tms.h"

u32 appInitTimer = 0;
static int m_server_status = 0;

extern unsigned char MainStatus;
extern u8 tsn[64];
//develop
int app_check_serverStatus(void) {
	return m_server_status;
}

void showTmsState(int msgId, void* args) {

	int i = 0;
	char filename[64] = { 0 };
	char ttsMoney[24] = { 0 };
	int feq[]={1,2,4,8,16,32};
	static int icount=0;
	static int index=0;

	API_LOG_DEBUG("Dspread:msgId:%d\r\n", msgId);
	switch (msgId)
	{
	case TMS_DISP_START_DOWNLOAD:
		if (MainStatus == 1)
		{
			app_lvgl_clearClientBg();
			app_lvgl_downloadShow(DSP_DOWNLOADING_IMG, 100, 100, "start downloading file", LV_COLOR_DSP_BLUE);
		}
		app_dsp_play(0, "/ext/audio/english/dsp_startdownloading.wav", "start downloading file", 1);
		break;
	case TMS_DISP_DOWNLOADING_PROGRESS:
		if (MainStatus == 1)
		{
			app_lvgl_clearClientBg();
			app_lvgl_downloadShow(DSP_DOWNLOADING_IMG, 100, 100, args, LV_COLOR_DSP_BLUE);
		}
		break;
	case TMS_DISP_DOWNLOAD_SUCCESS:
		if (MainStatus == 1)
		{
			app_lvgl_clearClientBg();
			app_lvgl_mainShow(DSP_DOWNCOMPLETE_IMG, 100, 100, "Download completed", LV_COLOR_DSP_BLUE);	
		}
		app_dsp_play(0, "/ext/audio/english/dsp_downloadcomplete.wav", "Download completed", 1);
		OsSysMsleep(1000);
		break;
	case TMS_DISP_DOWNLOAD_FAIL:
		if (MainStatus == 1)
		{
			app_lvgl_clearClientBg();
			app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100, "Updated failed", LV_COLOR_DSP_RED);
		}
		API_LOG_DEBUG("TMS Updated failed\r\n");
		app_dsp_play(0, "/ext/audio/english/dsp_updatefail.wav", "Updated failed", 1);
		OsSysMsleep(1000);
		break;
	case TMS_DISP_UPGRADE_SUCCESS:
		if (MainStatus == 1)
		{
			app_lvgl_clearClientBg();
			app_lvgl_mainShow(DSP_UPDATESUCC_IMG, 100, 100, "Updated successfully", LV_COLOR_DSP_GREEN);
		}
		API_LOG_DEBUG("TMS Updated successfully\r\n");
		app_dsp_play(0, "/ext/audio/english/dsp_updatesucc.wav", "Updated successfully", 1);
		// OsSysMsleep(1000);
		break;
	case TMS_DISP_UPGRADING:
		if (MainStatus == 1)
		{
			app_lvgl_clearClientBg();
			app_lvgl_mainShow(DSP_DOWNCOMPLETE_IMG, 100, 100, "Upgrading", LV_COLOR_DSP_RED);
		}
		API_LOG_DEBUG("TMS Upgrading\r\n");
		app_dsp_play(0, "/ext/audio/english/dsp_upgrading.wav", "Upgrading", 1);
		OsSysMsleep(1000);
		break;
	case TMS_DISP_UPGRADE_FAIL:
		if (MainStatus == 1)
		{
			app_lvgl_clearClientBg();
			app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100, "Updated failed", LV_COLOR_DSP_RED);
		}
		API_LOG_DEBUG("TMS Updated failed\r\n");
		app_dsp_play(0, "/ext/audio/english/dsp_updatefail.wav", "Updated failed", 1);
		OsSysMsleep(1000);
		break;

	case TMS_DISP_VERIFY_FAIL:
		if (MainStatus == 1)
		{
			app_lvgl_clearClientBg();
			app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100, "application verify error", LV_COLOR_DSP_RED);
		}
		app_dsp_play(0, "/ext/audio/english/dsp_verifyerror.wav", "application verify error", 1);
		OsSysMsleep(1000);
		break;
	case TMS_DISP_HAVE_UPDATE_TASE:
		API_LOG_DEBUG("TMS update\r\n");
		if (MainStatus == 1)
		{
			app_lvgl_clearClientBg();
			app_lvgl_mainShow(DSP_DOWNLOADING_IMG, 100, 100, "New version to update", LV_COLOR_DSP_BLUE);
		}
		app_dsp_play(0, "/ext/audio/english/dsp_newversion.wav", "New version to update", 1);
		OsSysMsleep(1000);
		break;
	default:
		break;
	}
}


void app_reg_iconEvent() {
	OsSegStatusbarSigReg(app_show_iconStatus);
}