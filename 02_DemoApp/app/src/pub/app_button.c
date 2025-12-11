/**
 * @breif  Monitoring button
 */
#include <stdlib.h>
#include "app_lvgl.h"
#include "app_pub.h"
#include "app_trans.h"
#include "app_button.h"
#include "app_card_payment.h"
#include "app_dsp.h"
#include "api_wifi.h"
#include "app_callback.h"
#include "app_audio.h"
#include "tms_term_mng.h"

extern unsigned char waitflag;

extern unsigned char MainStatus;

//develop
bool gTransThreadFlag = false;
u32 button;
int button_count = 0;
int button_length = 0;
int button_len = 65;
char amount[MAX_AMOUNT_LENGTH + 1] = {0};
char previous_amount[MAX_AMOUNT_LENGTH + 1];
int previous_amount_len = 0;
int total_amount;
char inputBuf[48] = {0};
char currentSsid[64] = {0};
char currentConnectSsid[64] = {0};
int transRecordTotal = -1;
int transRecordcurpage = 1;
int transRecordPageNum = 0;
int transRecordTotalpageSize = 1;
const char *menuItems[] = {"1.Language", "2.Transaction", "3.Version",
						   "4.Network", "5.System", "6.Card Pay", "7.LogSwitch"};
const char *languageMenuItems[] = {"1.English"};
const char *networkMenuItems[] = {"1.4G", "2.Wifi", "3.Smartphone Input", "4.WiFi firmware update"};
const char *systemMenuItems[] = {"1.Information", "2.Params Setting", "3.Sign In", "4.Synchronize Time"};
const char *ISO8583MenuItems[] = {"1.Sign", "2.Sale", "3.Void", "4.Refund", "5.Balance"};
const char *logMenuItems[] = {"1.OFF", "2.0N"};
dsp_tradeInfo_t tradeInfo[5] = {0};
dsp_tradeInfo_t *plastTradeInfo = NULL;
static ST_AP_LIST pstApList[24] = {0};
ST_AP_INFO apInfo = {0};
int hotspot_totalCnt = 0;
int hotspot_page_perSize = LIST_ITEM_SHOW_MAX_CNT;
int hotspot_current_page = 0;
int hotspot_total_page = 0;
int keyPadMode = 0;
AppMode current_mode = MODE_NORMAL;

static u32 m_button_sem = 0;
extern char m_codeData[12];
char cardpayamount[64]={0};
char g_dynamic_amountFormat[64]={0};
char pinpassward[32]={0};
bool device_Status = true;



static const KeyHandler key_handlers[] = {
	{KEYPAD_POWER_LONG, MODE_ANY, handle_power},
	{KEYPAD_VOLUME_INC, MODE_ANY, handle_volume_inc},
	{KEYPAD_VOLUME_DEC, MODE_ANY, handle_volume_dec},
	{CLEAR, MODE_ANY, handle_clear},
	{CANCEL, MODE_ANY, handle_cancel},
	{PLUS, MODE_ANY, handle_plus},
	{FUNCTION, MODE_ANY, handle_func},
	{MENU, MODE_ANY, handle_menu},
	{ENTER, MODE_ANY, handle_enter},
	{PGUP, MODE_ANY, handle_pgup},
	{PGDOWN, MODE_ANY, handle_pgdown},

	{DIGITAL1, MODE_MENU, handle_language},
	{DIGITAL2, MODE_MENU, handle_trans_query},
	{DIGITAL3, MODE_MENU, handle_version_query},
	{DIGITAL4, MODE_MENU, handle_network_setting},
	{DIGITAL5, MODE_MENU, handle_system_param},
	{DIGITAL6, MODE_MENU, app_cardpay_show},
	{DIGITAL7, MODE_MENU, handle_log_switch},


	{DIGITAL0, MODE_CARDPAY_SHOW, app_handle_cardpay},
	{DIGITAL1, MODE_CARDPAY_SHOW, app_handle_cardpay},
	{DIGITAL2, MODE_CARDPAY_SHOW, app_handle_cardpay},
	{DIGITAL3, MODE_CARDPAY_SHOW, app_handle_cardpay},
	{DIGITAL4, MODE_CARDPAY_SHOW, app_handle_cardpay},
	{DIGITAL5, MODE_CARDPAY_SHOW, app_handle_cardpay},
	{DIGITAL6, MODE_CARDPAY_SHOW, app_handle_cardpay},
	{DIGITAL7, MODE_CARDPAY_SHOW, app_handle_cardpay},
	{DIGITAL8, MODE_CARDPAY_SHOW, app_handle_cardpay},
	{DIGITAL9, MODE_CARDPAY_SHOW, app_handle_cardpay},
	{CANCEL, MODE_CARDPAY_SHOW, app_handle_cardpay},
	{CLEAR, MODE_CARDPAY_SHOW, app_handle_cardpay},
	{ENTER, MODE_CARDPAY_SHOW, app_handle_cardpay},


	{DIGITAL1, MODE_LOGSWITCH_SET, handle_log_switch_set},
	{DIGITAL2, MODE_LOGSWITCH_SET, handle_log_switch_set},

	{DIGITAL1, MODE_LANGUAGE_SET, handle_detail_language},

	{DIGITAL1, MODE_NETWORK_SET, handle_sim_setting},
	{DIGITAL2, MODE_NETWORK_SET, handle_wifi_device_setting},
	{DIGITAL3, MODE_NETWORK_SET, handle_wifi_phone_setting},
	{DIGITAL4, MODE_NETWORK_SET, handle_wifi_firmware_update},

	{DIGITAL1, MODE_SYSTEM_SETTING, handle_infomation},
	{DIGITAL2, MODE_SYSTEM_SETTING, handle_param_settings},
	{DIGITAL3, MODE_SYSTEM_SETTING, handle_sign_in},
	{DIGITAL4, MODE_SYSTEM_SETTING, handle_sync_time},

	{DIGITAL0, MODE_ANY, handle_digit},
	{DIGITAL1, MODE_ANY, handle_digit},
	{DIGITAL2, MODE_ANY, handle_digit},
	{DIGITAL3, MODE_ANY, handle_digit},
	{DIGITAL4, MODE_ANY, handle_digit},
	{DIGITAL5, MODE_ANY, handle_digit},
	{DIGITAL6, MODE_ANY, handle_digit},
	{DIGITAL7, MODE_ANY, handle_digit},
	{DIGITAL8, MODE_ANY, handle_digit},
	{DIGITAL9, MODE_ANY, handle_digit},

};


int app_button_semWait()
{

	if (m_button_sem == 0)
	{
		m_button_sem = OsSysSemNew(0);
	}
	if (m_button_sem != 0)
		OsSysSemWait(m_button_sem);
	return m_button_sem == 0 ? -1 : 0;
}

int app_button_semSig()
{

	if (m_button_sem == 0)
	{
		m_button_sem = OsSysSemNew(0);
	}
	if (m_button_sem != 0)
		OsSysSemSignal(m_button_sem);
	return m_button_sem == 0 ? -1 : 0;
}

static void VolumeOper(char type)
{
	char buf[12];
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d", OsAudioGetVolume(1));
	app_seg_showMoney(buf);

	if (type == 0)
	{
		API_LOG_DEBUG("audio_level_INC=[%d]", OsAudioGetVolume(1));
		if (OsAudioGetVolume(1) == MAX_AUDIO_VOLUME)
			app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_maxvolume.wav", "this is maximum volume", 0);
		else
			app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_volumeup.wav", "volume up", 0);
	}
	else if (type == 1)
	{
		API_LOG_DEBUG("audio_level_DEC=[%d]", OsAudioGetVolume(1));
		if (OsAudioGetVolume(1) == MIN_AUDIO_VOLUME)
			app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_minvolume.wav", "this is minimum volume", 0);
		else
			app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_volumedown.wav", "volume down", 0);
	}
}

void dsp_termianalInit(void)
{
	char outdataKey[64] = {0};
	char outpinKey[64] = {0};
	char outmerchantId[64] = {0};
	char outtid[64] = {0};
	char outmerchantName[64] = {0};
	char msgOut[64] = {0};
	int ret = 0;

	app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "waiting...", LV_COLOR_DSP_BLUE);
	ret = app_dsp_terminalInit(DSP_URL,
								  outdataKey, outpinKey,
								  outmerchantId, outtid,
								  outmerchantName, msgOut);
	if (ret != 0)
	{

		app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100, msgOut, LV_COLOR_DSP_RED);
	}
	else
	{
		app_lvgl_merchantInfoShow(
			outdataKey, outpinKey,
			outmerchantId, outtid,
			outmerchantName);
	}
}

extern u32 thread_buttonID;

void app_mon_button(void *param)
{
	API_LOG_DEBUG("app_mon_button......");
	u32 i;
	app_button_semWait();
	while (1)
	{
		button = app_getkey();	
		if (button != KEYPAD_NO_KEY) {
			hibernate_startTick = OsSysTick();
			API_LOG_DEBUG("button=%d\r\n",button);
		}
		for (i = 0; i < sizeof(key_handlers) / sizeof(KeyHandler); i++)
		{
		    if(MainStatus==0)
		    {
		         if(button!=KEYPAD_POWER_LONG)
		         {
		             break;
		         }
		    }
			if (button == key_handlers[i].key && (key_handlers[i].mode == current_mode || key_handlers[i].mode == MODE_ANY))
			{
				API_LOG_DEBUG("button=%d,i=%d,mode=%d\r\n",button,i,current_mode);
				key_handlers[i].handler(button);
				break;
			}
		}
		OsSysMsleep(50);
	}
	OsSysThreadDelete(thread_buttonID);
}
static void handle_power(void)
{
	API_LOG_DEBUG("Power off");
	app_lvgl_poweroff();
	return;
}
static void handle_volume_inc(void)
{
	if (gTransThreadFlag == false)
	{

		VolumeOper(0);
	}
}
static void handle_volume_dec(void)
{
	if (gTransThreadFlag == false)
	{
		VolumeOper(1);
	}
}

static void handle_clear(void)
{
	int ret = -1;
	char buf[12];
	int input_mode = 0;
	if (current_mode == MODE_TRANS_INFO)
	{

		API_LOG_DEBUG("app start revoke trans\r\n");
		API_LOG_DEBUG("current trans info:%s,%s,%s,%s,%s,%s,%s,%s\r\n",
					  tradeInfo[0].traceNo,
					  tradeInfo[0].batchNo,
					  tradeInfo[0].transStatus,
					  tradeInfo[0].transTypeId,
					  tradeInfo[0].origOutTradeId,
					  tradeInfo[0].transDatetime,
					  tradeInfo[0].money,
					  tradeInfo[0].cardNo);
		int transState = -1;
		char amountFormat[12] = {0};
		char msgOut[128] = {0};
		if (
			(strcmp(tradeInfo[0].transTypeId, "1102") == 0 || strcmp(tradeInfo[0].transTypeId, "1103") == 0 || strcmp(tradeInfo[0].transTypeId, "1101") == 0) &&
			(strcmp(tradeInfo[0].transStatus, "02") == 0) &&
			strlen(tradeInfo[0].origOutTradeId))
		{
			API_LOG_DEBUG("this is scan code refund\r\n");
			// sprintf(amountFormat,"%d.%d",atoi(tradeInfo[0].money)/100, atoi(tradeInfo[0].money) % 100);
			ret = app_dsp_qrcodePayRefund(
				MCHNTCD, DEVICEID,
				DSP_URL, tradeInfo[0].money,
				tradeInfo[0].traceNo, tradeInfo[0].batchNo,
				tradeInfo[0].origOutTradeId, &transState,
				msgOut);
			if (ret == 0)
				app_lvgl_mainShow(DSP_SUCC_IMG, 100, 100,
								  "Refund success", LV_COLOR_DSP_GREEN);
			else
				app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100,
								  "Refund fail", LV_COLOR_DSP_RED);
		}
		else if ((strcmp(tradeInfo[0].transTypeId, "1202") == 0) && (strcmp(tradeInfo[0].transStatus, "02") == 0))
		{

			API_LOG_DEBUG("this is cardpay refund\r\n");
			// sprintf(amountFormat, "%d.%d", atoi(tradeInfo[0].money) / 100, atoi(tradeInfo[0].money) % 100);
			ret = app_dsp_cardPayRefund(
				MCHNTCD, DEVICEID, DSP_URL,
				tradeInfo[0].money, tradeInfo[0].cardNo, serialID,
				tradeInfo[0].traceNo, tradeInfo[0].batchNo, tradeInfo[0].origOutTradeId, msgOut);

			if (ret != 0)
				app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100,
								  "Refund fail", LV_COLOR_DSP_RED);
			else
				app_lvgl_mainShow(DSP_SUCC_IMG, 100, 100,
								  "Refund success", LV_COLOR_DSP_GREEN);
		}
		else
		{
			app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100,
							  "type error", LV_COLOR_DSP_RED);
		}
		current_mode = MODE_TRANS_TIPS;
	}
	else if (
		(current_mode == MODE_SYS_PARAMS_SET_TIME) ||
		(current_mode == MODE_NETWORK_WIFI_INPUTPWD) )
	{
		if (strlen(inputBuf) > 0)
		{
			inputBuf[strlen(inputBuf) - 1] = 0;
			API_LOG_DEBUG("del currenr inputBuf:%s\r\n", inputBuf);
			if (current_mode == MODE_NETWORK_WIFI_INPUTPWD)
				app_lvgl_inputShow(currentSsid, inputBuf, input_mode, 0, keyPadMode, true);
			if (current_mode == MODE_SYS_PARAMS_SET_TIME)
				app_lvgl_inputShow("SET MACHINE TIME", inputBuf, input_mode, 0, keyPadMode, true);
		}
	}
	else
	{
		if (gTransThreadFlag == false)
		{
			if (button_length > 0)
			{
				amount[--button_length] = '\0';
				memset(g_dynamic_amountFormat, 0, sizeof(g_dynamic_amountFormat));
				app_amount_formatting(amount, g_dynamic_amountFormat);
				app_seg_showMoney(g_dynamic_amountFormat);
				if (current_mode == MODE_CARDPAY_SHOW) {
					app_lvgl_updateInputShow(g_dynamic_amountFormat,0);
				}
			}
		}
		else
		{
			if (inputpinpassward == true)
			{
				if (button_count > 0)
				{
					button_count--;
					button_len = button_len - 20;
					amount[--button_length] = '\0';
					memset(buf, 0, sizeof(buf));
					app_generate_dashes(button_count, buf);
					OsSegShowChar(buf);
					app_lvgl_updateInputpinShow(button_count);
					app_lcd_brush_screen();
				}
			}
		}
	}
}

static void handle_digit(u32 button)
{
	char buf[64];
	u32 pressKeyTick = 0;
	u32 tmpChar;
	u32 last_button;
	int input_mode = 0;

	if (current_mode == MODE_NORMAL || current_mode == MODE_PLUS)
	{
		if (gTransThreadFlag == false)
		{
			if (button_length < MAX_AMOUNT_LENGTH)
			{
				if (button_length == 0 && button == DIGITAL0)
				{
					return;
				}
				amount[button_length++] = (char)button;
				amount[button_length] = '\0';
				memset(g_dynamic_amountFormat, 0, sizeof(g_dynamic_amountFormat));
				app_amount_formatting(amount, g_dynamic_amountFormat);
				app_seg_showMoney(g_dynamic_amountFormat);
			}
		}
		else if (gTransThreadFlag == true && inputpinpassward == true)
		{
			if (button_length < 6)
			{
				amount[button_length++] = (char)button;
				amount[button_length] = '\0';
				memset(g_dynamic_amountFormat, 0, sizeof(g_dynamic_amountFormat));
				app_pan_formatting(amount, g_dynamic_amountFormat);
				API_LOG_DEBUG("amount:%s,amount_format:%s\r\n", amount, g_dynamic_amountFormat);
				memset(buf, 0, sizeof(buf));
				app_generate_dashes(button_count + 1, buf);
				OsSegShowChar(buf);
				button_count++;
				app_lvgl_updateInputpinShow(button_count);
				app_lcd_brush_screen();
				button_len = button_len + 20;
			}
		}
	}
	else if (
		(current_mode == MODE_NETWORK_WIFI_INPUTPWD) ||
		(current_mode == MODE_SYS_PARAMS_SET_TIME))
	{
		if (strlen(inputBuf) < sizeof(inputBuf) - 2)
		{
			if (current_mode == MODE_SYS_PARAMS_SET_TIME && strlen(inputBuf) > 13)
				return;
			tmpChar = button;
			if (keyPadMode == 0)
			{
				tmpChar = keymap_get_curChar(keyPadMode, tmpChar);
			}
			else if (keyPadMode == 1 || keyPadMode == 2)
			{
				if (((OsSysTick() - pressKeyTick) < 1500) && (button != PGDOWN) && (last_button == button))
				{
					if (strlen(inputBuf) > 0)
					{
						inputBuf[strlen(inputBuf) - 1] = 0;
					}
					tmpChar = keymap_get_nextChar(keyPadMode, tmpChar);
					API_LOG_DEBUG("next tmpChar:%d,%c\r\n", tmpChar, tmpChar);
				}
				else
				{
					keymap_reset_step();
					tmpChar = keymap_get_curChar(keyPadMode, tmpChar);
					API_LOG_DEBUG("tmpChar:%d,%c\r\n", tmpChar, tmpChar);
				}
				pressKeyTick = OsSysTick();
			}
			if (strlen(inputBuf) < (sizeof(inputBuf) - 2))
			{
				inputBuf[strlen(inputBuf)] = tmpChar;
				if (current_mode == MODE_NETWORK_WIFI_INPUTPWD)
				{
					app_lvgl_inputShow(currentSsid, inputBuf, input_mode, 0, keyPadMode, true);
				}
				else if (current_mode == MODE_SYS_PARAMS_SET_TIME)
				{
					app_lvgl_inputShow("SET MACHINE TIME", inputBuf, input_mode, 0, keyPadMode, true);
				}
			}
			last_button = button;
		}
		API_LOG_DEBUG("add inputBuf:%s\r\n", inputBuf);
	}

}

static void handle_enter(void)
{
	int i = 0;
	int ret = -1;
	int input_mode = 0;
	int keyPadMode = 0;
	int showCnt = 0;
	char *ssidMenus[32] = {0};

	if (current_mode == MODE_NORMAL || current_mode == MODE_PLUS || current_mode==MODE_CARDPAY_SHOW)
	{
		if (app_check_serverStatus())
		{
			app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100,
							  "tms task is running", LV_COLOR_DSP_RED);
		}
		if (gTransThreadFlag == true && inputpinpassward == true)
		{
			if (4 <= button_count && button_count <= 12)
			{
				strcpy(pinpassward, amount);
				API_LOG_DEBUG("pinpassward=[%s]", pinpassward);
				button_count = 0;
				button_len = 65;
				strcpy(amount, "0.00");
				button_length = 0;
				app_seg_showMoney(cardpayamount);
				inputpinpassward = false;
			}
		}
		else if (gTransThreadFlag == false)
		{
			trans_thread_mutexLock();
			if (current_mode == MODE_PLUS)
			{
				if (atoi(amount) != 0 && atoi(previous_amount) != 0)
				{
					sprintf(amount, "%d", atoi(amount) + atoi(previous_amount));
				}
				else
				{
					memcpy(amount, previous_amount, strlen(previous_amount));
				}
				memset(g_dynamic_amountFormat, 0, sizeof(g_dynamic_amountFormat));
				app_amount_formatting(amount, g_dynamic_amountFormat); // 111->1.11
				button_length = strlen(amount);
			}
			if (atoi(amount) != 0 && button_length > 0)
			{
				app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_qrcodescan.wav", "Please present your card", 0);
				gTransThreadFlag = true;
				OsSegShowMoney(g_dynamic_amountFormat);
				strcpy(cardpayamount, g_dynamic_amountFormat);
				memset(amount, 0, sizeof(amount));
				memset(previous_amount, 0, sizeof(previous_amount));
				memset(&point, 0, sizeof(point));
				button_length = 0;
			}
			trans_thread_mutexUnlock();
		}
	}
	else if (current_mode == MODE_NETWORK_WIFI_INPUTPWD)
	{
		if (strlen(inputBuf) > 0 && strlen(currentSsid) != 0)
		{
			app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "connecting...", LV_COLOR_DSP_BLUE);
			API_LOG_DEBUG("enter inputBuf:%s\r\n", inputBuf);
			ret = OsWifiConnectHotSpot(currentSsid, inputBuf);
			API_LOG_DEBUG("OsWifiConnectHotSpot ret:%d\r\n", ret);
			if (ret == 0)
			{
				app_lvgl_mainShow(DSP_SUCC_IMG, 100, 100, "wifi connect success", LV_COLOR_DSP_GREEN);
				app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_wifisucc.wav", "wifi connect success", 1);
				if (OsParamsGetNetMode() == 1)
				{
					OsParamsSetNetMode(0);
					OsSysMsleep(1000);
					OsSysReboot();
				}
			}
			else
			{
				app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100, "wifi connect fail", LV_COLOR_DSP_RED);
				app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_wififail.wav", "wifi connect fail", 1);
			}
			{
				showCnt = 0;
				memset(ssidMenus, 0, sizeof(ssidMenus));
				API_LOG_DEBUG("hotspot_current_page:%d,hotspot_page_totalSize:%d,%d\r\n",
							  hotspot_current_page, hotspot_totalCnt, hotspot_page_perSize);
				if (hotspot_current_page == hotspot_total_page)
				{
					if ((hotspot_totalCnt % hotspot_page_perSize) == 0)
					{
						showCnt = hotspot_page_perSize;
					}
					else
					{
						showCnt = hotspot_totalCnt % hotspot_page_perSize;
					}
				}
				else
				{
					showCnt = hotspot_page_perSize;
				}
				for (i = 0; i < showCnt; i++)
				{
					ssidMenus[i] = pstApList[i + (hotspot_current_page - 1) * hotspot_page_perSize].cSsid;
				}
				app_lvgl_listShow("hotspot", ssidMenus, showCnt, currentConnectSsid);
				current_mode = MODE_NETWORK_WIFI_CONFIG;
			}
		}
		else
		{
			API_LOG_DEBUG(" inputBuf empty\r\n");
		}
	}
	else if (current_mode == MODE_SYS_PARAMS_SET_TIME)
	{

		if (strlen(inputBuf) == 14)
		{
			int ret = -1;
			keyPadMode = 0;
			API_LOG_DEBUG("MODE_SYS_PARAMS_SET_TIME inputBuf =[%s]", inputBuf);
			ret = OsSysSetRtcTime(inputBuf);
			if (ret != 0)
			{
				app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100, "set time fail", LV_COLOR_DSP_RED);
				return;
			}
			app_lvgl_mainShow(DSP_SUCC_IMG, 100, 100, "set time success", LV_COLOR_DSP_GREEN);
			OsSysMsleep(1500);
			memset(inputBuf, 0, sizeof(inputBuf));
			trans_thread_mutexLock();
			app_lvgl_menuShow("Menu", menuItems, sizeof(menuItems) / sizeof(menuItems[0]));
			trans_thread_mutexUnlock();
			current_mode = MODE_MENU;
		}
	}
}

static void handle_plus(void)
{
	if (button_length > 0 && inputpinpassward == false)
	{
		current_mode = MODE_PLUS;
		API_LOG_DEBUG("amount:%s\r\n", amount);
		if (atof(amount) != 0 && atof(previous_amount) != 0)
		{
			sprintf(amount, "%d", atoi(amount) + atoi(previous_amount));
		}
		API_LOG_DEBUG("cur total money=[%s]", amount);
		memcpy(previous_amount, amount, strlen(amount));
		previous_amount[strlen(amount)] = 0;

		memset(g_dynamic_amountFormat, 0, sizeof(g_dynamic_amountFormat));
		app_amount_formatting(amount, g_dynamic_amountFormat); // 111->1.11

		app_seg_showMoney(g_dynamic_amountFormat);
		memset(amount, 0, sizeof(amount));
		button_length = 0;
	}
}

static void handle_cancel(void)
{
	int i = 0;
	int input_mode = 0;
	int showCnt = 0;
	char *ssidMenus[32] = {0};
	if (TMS_IsBusy())
	{
		return;
	}
	if (gTransThreadFlag)
	{
		gTransThreadFlag = false;
		API_LOG_DEBUG("button cancel gTransThreadFlag:%d\r\n", gTransThreadFlag);
		return;
	}
	if (current_mode == MODE_MENU || current_mode == MODE_NORMAL || current_mode == MODE_PLUS)
	{
		current_mode = MODE_MENU;
		app_lvgl_menuShow("Menu", menuItems, sizeof(menuItems) / sizeof(menuItems[0]));
	}
	else if (current_mode == MODE_TRANS_INFO)
	{
		current_mode = MODE_MENU;
		app_lvgl_menuShow("Menu", menuItems, sizeof(menuItems) / sizeof(menuItems[0]));
	}
	else if (
		current_mode == MODE_LANGUAGE_SET ||
		current_mode == MODE_VERSION_INFO ||
		current_mode == MODE_NETWORK_SET ||
		current_mode == MODE_PARAM_QUERY ||
		current_mode == MODE_SYSTEM_SETTING ||
		current_mode == MODE_TRANS_INFO ||
		current_mode == MODE_ISO8583 ||
		current_mode == MODE_WIFI_SETTING||
		current_mode ==MODE_CARDPAY_SHOW)
	{
		trans_thread_mutexLock();
		app_lvgl_menuShow("Menu", menuItems, sizeof(menuItems) / sizeof(menuItems[0]));
		trans_thread_mutexUnlock();
		transRecordTotal = 0;
		transRecordcurpage = 1;
		transRecordPageNum = 0;
		transRecordTotalpageSize = 1;
		button_length = 0;
		memset(amount, 0, sizeof(amount));
		OsSegShowMoney("0.00");
		current_mode = MODE_MENU;
	}
	else if (
		current_mode == MODE_TMS_UPDATE_VERSION ||
		current_mode == MODE_SYS_INFORMATION ||
		current_mode == MODE_PASSWORD_SETTING ||
		current_mode == MODE_ENVIRONMENT)
	{
		current_mode = MODE_SYSTEM_SETTING;
		app_lvgl_menuShow("System Settings", systemMenuItems, sizeof(systemMenuItems) / sizeof(systemMenuItems[0]));
	}
	else if (current_mode == MODE_NETWORK_WIFI_CONFIG)
	{
		current_mode = MODE_NETWORK_SET;
		app_lvgl_menuShow("Network Set", networkMenuItems, sizeof(networkMenuItems) / sizeof(networkMenuItems[0]));
	}
	else if (current_mode == MODE_NETWORK_WIFI_INPUTPWD)
	{
		showCnt = 0;
		memset(ssidMenus, 0, sizeof(ssidMenus));
		if (hotspot_current_page == hotspot_total_page)
		{
			if ((hotspot_totalCnt % hotspot_page_perSize) == 0)
			{
				showCnt = hotspot_page_perSize;
			}
			else
			{
				showCnt = hotspot_totalCnt % hotspot_page_perSize;
			}
		}
		else
		{
			showCnt = hotspot_page_perSize;
		}
		for (i = 0; i < showCnt; i++)
		{
			ssidMenus[i] = pstApList[i + (hotspot_current_page - 1) * hotspot_page_perSize].cSsid;
		}

		app_lvgl_listShow("hotspot", ssidMenus, showCnt, currentConnectSsid);
		current_mode = MODE_NETWORK_WIFI_CONFIG;
	}
	else if (current_mode == MODE_TRANS_TIPS)
	{
		if (transRecordTotal > 0)
		{
			app_transaction_recordShow(plastTradeInfo, 1);
			current_mode = MODE_TRANS_INFO;
		}
		else
		{
			current_mode = MODE_MENU;
			app_lvgl_menuShow("Menu", menuItems, sizeof(menuItems) / sizeof(menuItems[0]));
		}
	}
	else if (current_mode == MODE_NETWORK_WIRELESS_TIP)
	{
		current_mode = MODE_NETWORK_SET;
		app_lvgl_menuShow("Network Set", networkMenuItems, sizeof(networkMenuItems) / sizeof(networkMenuItems[0]));
	}
	else if (current_mode == MODE_SYS_PARAMS_SIGNIN_TIP)
	{
		app_lvgl_menuShow("System Settings", systemMenuItems, sizeof(systemMenuItems) / sizeof(systemMenuItems[0]));
		current_mode = MODE_SYSTEM_SETTING;
	}
	else if (current_mode == MODE_SYS_PARAMS_SET_TIME)
	{
		current_mode = MODE_SYSTEM_SETTING;
		app_lvgl_menuShow("System Settings", systemMenuItems, sizeof(systemMenuItems) / sizeof(systemMenuItems[0]));
	}
	else if (current_mode == MODE_SYS_PARAMS_SYNC_TMS_TASKS)
	{
		current_mode = MODE_SYSTEM_SETTING;
		app_lvgl_menuShow("System Settings", systemMenuItems, sizeof(systemMenuItems) / sizeof(systemMenuItems[0]));
	}
	else
	{
		trans_thread_mutexLock();
		app_lvgl_menuShow("Menu", menuItems, sizeof(menuItems) / sizeof(menuItems[0]));
		trans_thread_mutexUnlock();
		current_mode = MODE_NORMAL;
	}
}
void handle_menu(u32 button)
{
	API_LOG_DEBUG("menu show,mode=%d\r\n",current_mode);
	//if ((current_mode == MODE_NORMAL || current_mode == MODE_MENU || current_mode==MODE_CARDPAY_SHOW) && gTransThreadFlag == false ){
	if (gTransThreadFlag == false )
	{
		current_mode = MODE_MENU;
		app_lvgl_menuShow("Menu", menuItems, sizeof(menuItems) / sizeof(menuItems[0]));
	}
}

static void handle_func(void)
{
	int input_mode = 0;
	if (current_mode == MODE_NETWORK_WIFI_INPUTPWD)
	{
		app_lvgl_inputShow(currentSsid, inputBuf, input_mode, 0, keyPadMode, true);
	}
	else
	{

		if (OsParamsGetNetMode() == 1)
		{
			if (OsWlGetNetState())
			{
				app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_netnormal.wav", "network connected", 0);
			}
			else
			{
				app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_netabnormal.wav", "network disconnected", 0);
			}
		}
		else if (OsParamsGetNetMode() == 0)
		{
			if (OsWifiGetConnectStatus() == 2)
			{
				app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_netnormal.wav", "network connected", 0);
			}
			else
			{
				app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_netabnormal.wav", "network disconnected", 0);
			}
		}
		if (app_check_serverStatus())
		{
			app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_serversucc.wav", "server connected", 0);
		}
		else
		{
			app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_serverfail.wav", "server disconnected", 0);
		}
	}
}

static void handle_pgup(void)
{
	int i = 0;
	int showCnt = 0;
	char *ssidMenus[32] = {0};
	if (current_mode == MODE_TRANS_INFO)
	{
		char msgOut[32] = {0};
		if (transRecordcurpage > 1)
		{
			memset(tradeInfo, 0, sizeof(tradeInfo));
			transRecordcurpage--;
			int ret = app_dsp_transactionFlow(
				MCHNTCD, DEVICEID,
				DSP_URL, transRecordcurpage,
				transRecordTotalpageSize, &transRecordTotal, tradeInfo,
				msgOut);

			API_LOG_DEBUG("total:%d,ret:%d\r\n", transRecordTotal, ret);
			if (ret > 0 && transRecordTotal > 0)
			{

				app_transaction_recordShow(tradeInfo, 1);
				plastTradeInfo = &tradeInfo[0];
			}
			else
			{
				app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100,
								  "get record info error!", LV_COLOR_DSP_RED);
			}
		}
		else
		{
			transRecordcurpage = 0;
			app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100,
							  "It's already the first page!", LV_COLOR_DSP_RED);
		}
	}
	else if (current_mode == MODE_NETWORK_WIFI_CONFIG)
	{
		showCnt = 0;
		memset(ssidMenus, 0, sizeof(ssidMenus));
		if (hotspot_current_page > 1)
		{
			hotspot_current_page--;

			if (hotspot_current_page == hotspot_total_page)
			{
				if ((hotspot_totalCnt % hotspot_page_perSize) == 0)
				{
					showCnt = hotspot_page_perSize;
				}
				else
				{
					showCnt = hotspot_totalCnt % hotspot_page_perSize;
				}
			}
			else
			{
				showCnt = hotspot_page_perSize;
			}
			for (i = 0; i < showCnt; i++)
			{
				ssidMenus[i] = pstApList[i + (hotspot_current_page - 1) * hotspot_page_perSize].cSsid;
			}
			trans_thread_mutexLock();
			app_lvgl_listShow("hotspot", ssidMenus, showCnt, currentConnectSsid);
			trans_thread_mutexUnlock();
		}
	}
}
static void handle_pgdown(void)
{
	int i = 0;
	int showCnt = 0;
	int input_mode = 0;
	char *ssidMenus[32] = {0};
	if (current_mode == MODE_TRANS_INFO)
	{

		char msgOut[32] = {0};
		if (transRecordcurpage < transRecordTotal)
		{
			memset(tradeInfo, 0, sizeof(tradeInfo));
			transRecordcurpage++;
			int ret = app_dsp_transactionFlow(
				MCHNTCD, DEVICEID,
				DSP_URL, transRecordcurpage,
				transRecordTotalpageSize, &transRecordTotal, tradeInfo,
				msgOut);

			API_LOG_DEBUG("total:%d,ret:%d\r\n", transRecordTotal, ret);
			if (ret > 0 && transRecordTotal > 0)
			{

				app_transaction_recordShow(tradeInfo, 1);
				plastTradeInfo = &tradeInfo[0];
			}
			else
			{
				app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100,
								  "get record info error!", LV_COLOR_DSP_RED);
			}
		}
		else
		{
			transRecordcurpage=transRecordTotal +1;
			app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100,
							  "It's already the last page!", LV_COLOR_DSP_RED);
		}
	}
	else if (current_mode == MODE_NETWORK_WIFI_CONFIG)
	{
		showCnt = 0;
		memset(ssidMenus, 0, sizeof(ssidMenus));
		if (hotspot_current_page < hotspot_total_page)
		{
			hotspot_current_page++;

			API_LOG_DEBUG("hotspot_current_page:%d,hotspot_page_totalSize:%d,%d\r\n",
						  hotspot_current_page, hotspot_totalCnt, hotspot_page_perSize);
			if (hotspot_current_page == hotspot_total_page)
			{
				if ((hotspot_totalCnt % hotspot_page_perSize) == 0)
				{
					showCnt = hotspot_page_perSize;
				}
				else
				{
					showCnt = hotspot_totalCnt % hotspot_page_perSize;
				}
			}
			else
			{
				showCnt = hotspot_page_perSize;
			}
			API_LOG_DEBUG("wifi next showCnt:%d,hotspot_current_page:%d\r\n",
						  showCnt, hotspot_current_page);
			for (i = 0; i < showCnt; i++)
			{
				ssidMenus[i] = pstApList[i + (hotspot_current_page - 1) * hotspot_page_perSize].cSsid;
			}
			app_lvgl_listShow("hotspot", ssidMenus, showCnt, currentConnectSsid);
		}
	}
	else if ((current_mode == MODE_NETWORK_WIFI_INPUTPWD) )
	{
		if (strlen(inputBuf) < (sizeof(inputBuf) - 2))
		{
			inputBuf[strlen(inputBuf)] = '.';
			app_lvgl_inputShow(currentSsid, inputBuf, input_mode, 0, keyPadMode, true);
		}
	}
}

static void handle_language(void)
{
	app_lvgl_menuShow("Language Set", languageMenuItems, sizeof(languageMenuItems) / sizeof(languageMenuItems[0]));
	current_mode = MODE_LANGUAGE_SET;
}
static void handle_detail_language(void)
{
	API_LOG_DEBUG("english set\r\n");
}

static void handle_trans_query(void)
{
	char msgOut[32] = {0};
	memset(tradeInfo, 0, sizeof(tradeInfo));
	transRecordTotal = -1;
	transRecordcurpage = 1;
	int ret = app_dsp_transactionFlow(
		MCHNTCD, DEVICEID,
		DSP_URL, transRecordcurpage,
		transRecordTotalpageSize, &transRecordTotal, tradeInfo,
		msgOut);

	API_LOG_DEBUG("total:%d,ret:%d\r\n", transRecordTotal, ret);
	if (ret > 0 && transRecordTotal > 0)
	{
		transRecordPageNum = ((transRecordTotal % transRecordTotalpageSize) == 0) ? transRecordTotal / transRecordTotalpageSize : transRecordTotal / transRecordTotalpageSize + 1;
		app_transaction_recordShow(tradeInfo, 1);
		plastTradeInfo = &tradeInfo[0];
		current_mode = MODE_TRANS_INFO;
	}
	else if (transRecordTotal == 0 || ret == 0)
	{
		app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100,
						  "no records", LV_COLOR_DSP_RED);
		current_mode = MODE_TRANS_TIPS;
	}
	else
	{
		app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100,
						  msgOut, LV_COLOR_DSP_RED);
		current_mode = MODE_TRANS_TIPS;
	}
}
static void handle_version_query(void)
{
	current_mode = MODE_VERSION_INFO;
	app_lvgl_versionInfoShow();
}
static void handle_network_setting(void)
{
	current_mode = MODE_NETWORK_SET;
	app_lvgl_menuShow("Network Set", networkMenuItems, sizeof(networkMenuItems) / sizeof(networkMenuItems[0]));
}
static void handle_sim_setting(void)
{
	API_LOG_DEBUG("4g set\r\n");
	if (OsParamsGetNetMode() == 0)
	{
		OsParamsSetNetMode(1);
		app_lvgl_mainShow(DSP_NETWORKSUCC_IMG, 100, 100,
						  "network set 4g mode success!", LV_COLOR_DSP_BLUE);
		OsSysMsleep(1000);
		OsSysReboot();
	}
	else
	{
		app_lvgl_mainShow(DSP_NETWORKSUCC_IMG, 100, 100,
						  "It's already 4g mode!", LV_COLOR_DSP_BLUE);
		current_mode = MODE_NETWORK_WIRELESS_TIP;
	}
}
static void handle_wifi_device_setting(void)
{
	int i = 0;
	int ret = -1;
	char *ssidMenus[32] = {0};
	u32 startTick = 0;
	int showCnt = 0;
	u32 lastTick;
	u32 buttonwifi;
	u32 last_buttonwifi;
	u32 sleepstate;
	int trytimes = 0;
	int currentMaxcnt = 0;
	unsigned int tmpChar;
	int input_mode = 0;
	int keyPadMode = 0;
	unsigned int pressKeyTick = 0;	

	startTick = OsSysTick();
	trytimes = 0;
	waitflag = 1;
	while (1)
	{
		OsSysMsleep(100);
		if (!device_Status|| trytimes == 300)
		{
			memset(amount, 0, sizeof(amount));
			memset(previous_amount, 0, sizeof(previous_amount));
			memset(&point, 0, sizeof(point));
			strcpy(amount, "0.00");
			button_length = 0;
			app_seg_showMoney(amount);
			trans_thread_mutexLock();

			app_static_qrcode_show(gQrUrl, gMchntcnName);
			trans_thread_mutexUnlock();
			current_mode = MODE_NORMAL;
			break;
		}
		lastTick = OsSysTick();
		app_hibernate_reset(60);

		if ((lastTick - startTick > 15000 && current_mode == MODE_NETWORK_WIFI_CONFIG) ||
			current_mode == MODE_NETWORK_SET)
		{
			sleepstate = OsGetSleepState();
			if (sleepstate == 1)
			{
				API_LOG_DEBUG("OsWifiGetScanResults  wake......");
				OsApWakeUpSp();
				OsSysMsleep(500);
			}
			current_mode = MODE_NETWORK_WIFI_CONFIG;
			API_LOG_DEBUG("start config wifi\r\n");
			app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "Waiting...", LV_COLOR_DSP_BLUE);
			hotspot_totalCnt = 0;
			hotspot_current_page = 0;
			memset(pstApList, 0, sizeof(pstApList));
			if (OsEspCheckApAsync(&apInfo) == 0)
			{
				API_LOG_DEBUG("OsEspCheckApAsync :[%s],[%d]\r\n", apInfo.cSsid, strlen(apInfo.cSsid));
				memset(currentConnectSsid, 0, sizeof(currentConnectSsid));

				memcpy(currentConnectSsid, apInfo.cSsid, strlen(apInfo.cSsid));
			}

			hotspot_totalCnt = OsWifiGetScanResults(pstApList, sizeof(pstApList) / sizeof(pstApList[0]));
			trytimes++;

			API_LOG_DEBUG("OsWifiGetScanResults hotspot_cnt:%d\r\n", hotspot_totalCnt);
			if (hotspot_totalCnt > 0)
			{
				hotspot_total_page = ((hotspot_totalCnt % hotspot_page_perSize) == 0) ? hotspot_totalCnt / hotspot_page_perSize : hotspot_totalCnt / hotspot_page_perSize + 1;
				hotspot_current_page = 1;
				API_LOG_DEBUG("hotspot_total_page:%d, hotspot_current_page:%d\r\n", hotspot_total_page,
							  hotspot_current_page);
				memset(ssidMenus, 0, sizeof(ssidMenus));
				showCnt = 0;
				if (hotspot_current_page == hotspot_total_page)
				{
					if ((hotspot_totalCnt % hotspot_page_perSize) == 0)
					{
						showCnt = hotspot_page_perSize;
					}
					else
					{
						showCnt = hotspot_totalCnt % hotspot_page_perSize;
					}
				}
				else
				{
					showCnt = hotspot_page_perSize;
				}
				API_LOG_DEBUG("showCnt :%d\r\n", showCnt);
				for (i = 0; i < showCnt; i++)
				{
					ssidMenus[i] = pstApList[i + (hotspot_current_page - 1) * hotspot_page_perSize].cSsid;
				}
				app_lvgl_listShow("hotspot", ssidMenus, showCnt, currentConnectSsid);
			}
			else
			{
				app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100,
								  "scan hotspot error", LV_COLOR_DSP_RED);
			}
			startTick = lastTick;
		}

		buttonwifi = OsKeypadGetKey();
		if ((buttonwifi >= DIGITAL0 && buttonwifi <= DIGITAL9))
		{
			if (current_mode == MODE_NETWORK_WIFI_INPUTPWD)
			{
				if (strlen(inputBuf) < sizeof(inputBuf) - 2)
				{
					if (current_mode == MODE_SYS_PARAMS_SET_TIME && strlen(inputBuf) > 13)
						continue;
					tmpChar = buttonwifi;
					if (keyPadMode == 0)
					{
						tmpChar = keymap_get_curChar(keyPadMode, tmpChar);
					}
					else if (keyPadMode == 1 || keyPadMode == 2)
					{
						if (((OsSysTick() - pressKeyTick) < 1500) && (buttonwifi != PGDOWN) && (last_buttonwifi == buttonwifi))
						{
							if (strlen(inputBuf) > 0)
							{
								inputBuf[strlen(inputBuf) - 1] = 0;
							}
							tmpChar = keymap_get_nextChar(keyPadMode, tmpChar);
							API_LOG_DEBUG("next tmpChar:%d,%c\r\n", tmpChar, tmpChar);
						}
						else
						{
							keymap_reset_step();
							tmpChar = keymap_get_curChar(keyPadMode, tmpChar);
							API_LOG_DEBUG("tmpChar:%d,%c\r\n", tmpChar, tmpChar);
						}
						pressKeyTick = OsSysTick();
					}
					if (strlen(inputBuf) < (sizeof(inputBuf) - 2))
					{
						inputBuf[strlen(inputBuf)] = tmpChar;
						app_lvgl_inputShow(currentSsid, inputBuf, input_mode, 0, keyPadMode, true);
					}
					last_buttonwifi = buttonwifi;
				}
				API_LOG_DEBUG("add inputBuf:%s\r\n", inputBuf);
			}
			else if (current_mode == MODE_NETWORK_WIFI_CONFIG)
			{
				API_LOG_DEBUG("cur:%d,total:%d\r\n", hotspot_current_page, hotspot_total_page);
				memset(currentSsid, 0, sizeof(currentSsid));
				keyPadMode = 0;
				if (hotspot_total_page > 0)
				{
					currentMaxcnt = 0;
					if (hotspot_current_page == hotspot_total_page)
					{
						if (hotspot_totalCnt % hotspot_page_perSize == 0)
						{

							currentMaxcnt = hotspot_page_perSize;
						}
						else
						{
							currentMaxcnt = hotspot_totalCnt % hotspot_page_perSize;
						}
					}
					else
					{
						currentMaxcnt = hotspot_page_perSize;
					}
					if ((buttonwifi != DIGITAL0) && ((buttonwifi - DIGITAL0) <= currentMaxcnt))
					{
						strcpy(currentSsid, pstApList[(buttonwifi - DIGITAL0 - 1) + (hotspot_current_page - 1) * hotspot_page_perSize].cSsid);
						API_LOG_DEBUG("currentSsid:%s\r\n", currentSsid);

						current_mode = MODE_NETWORK_WIFI_INPUTPWD;
						memset(inputBuf, 0, sizeof(inputBuf));
						input_mode = 0;
						app_lvgl_inputShow(currentSsid, inputBuf, input_mode, 0, keyPadMode, true);
					}
				}
			}
		}
		else if (buttonwifi == ENTER)
		{
			if (current_mode == MODE_NETWORK_WIFI_INPUTPWD)
			{

				if (strlen(inputBuf) > 0 && strlen(currentSsid) != 0)
				{
					app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "connecting...", LV_COLOR_DSP_BLUE);
					API_LOG_DEBUG("enter inputBuf:%s\r\n", inputBuf);
					ret = OsWifiConnectHotSpot(currentSsid, inputBuf);
					API_LOG_DEBUG("OsWifiConnectHotSpot ret:%d\r\n", ret);
					if (ret == 0)
					{
						app_lvgl_mainShow(DSP_SUCC_IMG, 100, 100, "wifi connect success", LV_COLOR_DSP_GREEN);
						app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_wifisucc.wav", "wifi connect success", 1);
						memcpy(currentConnectSsid, currentSsid, strlen(currentSsid));
						if (OsParamsGetNetMode() == 1)
						{
							OsParamsSetNetMode(0);
							OsSysMsleep(1000);
							OsSysReboot();
						}
						OsSysMsleep(2000);
						current_mode = MODE_NETWORK_SET;
						app_lvgl_menuShow("Network Set", networkMenuItems, sizeof(networkMenuItems) / sizeof(networkMenuItems[0]));
						break;
					}
					else
					{
						app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100, "wifi connect fail", LV_COLOR_DSP_RED);
						app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_wififail.wav", "wifi connect fail", 1);
						memset(currentConnectSsid, 0, sizeof(currentConnectSsid));
						
						showCnt = 0;
						memset(ssidMenus, 0, sizeof(ssidMenus));
						API_LOG_DEBUG("hotspot_current_page:%d,hotspot_page_totalSize:%d,%d\r\n",
									  hotspot_current_page, hotspot_totalCnt, hotspot_page_perSize);
						if (hotspot_current_page == hotspot_total_page)
						{
							if ((hotspot_totalCnt % hotspot_page_perSize) == 0)
							{
								showCnt = hotspot_page_perSize;
							}
							else
							{
								showCnt = hotspot_totalCnt % hotspot_page_perSize;
							}
						}
						else
						{
							showCnt = hotspot_page_perSize;
						}
						for (i = 0; i < showCnt; i++)
						{
							ssidMenus[i] = pstApList[i + (hotspot_current_page - 1) * hotspot_page_perSize].cSsid;
						}
						OsSysMsleep(2000);
						app_lvgl_listShow("hotspot", ssidMenus, showCnt, currentConnectSsid);
						current_mode = MODE_NETWORK_WIFI_CONFIG;
						startTick = OsSysTick();
						
					}
				}
				else
				{
					API_LOG_DEBUG(" inputBuf empty\r\n");
				}
			}
		}
		else if (buttonwifi == CLEAR)
		{
			if (current_mode == MODE_NETWORK_WIFI_INPUTPWD)
			{

				if (strlen(inputBuf) > 0)
				{
					inputBuf[strlen(inputBuf) - 1] = 0;
					API_LOG_DEBUG("del currenr inputBuf:%s\r\n", inputBuf);
					app_lvgl_inputShow(currentSsid, inputBuf, input_mode, 0, keyPadMode, true);
				}
			}
		}
		else if (buttonwifi == CANCEL)
		{
			if (current_mode == MODE_NETWORK_WIFI_CONFIG)
			{
				current_mode = MODE_NETWORK_SET;
				app_lvgl_menuShow("Network Set", networkMenuItems, sizeof(networkMenuItems) / sizeof(networkMenuItems[0]));
				break;
			}
			else if (current_mode == MODE_NETWORK_WIFI_INPUTPWD)
			{
				showCnt = 0;
				memset(ssidMenus, 0, sizeof(ssidMenus));
				if (hotspot_current_page == hotspot_total_page)
				{
					if ((hotspot_totalCnt % hotspot_page_perSize) == 0)
					{
						showCnt = hotspot_page_perSize;
					}
					else
					{
						showCnt = hotspot_totalCnt % hotspot_page_perSize;
					}
				}
				else
				{
					showCnt = hotspot_page_perSize;
				}
				for (i = 0; i < showCnt; i++)
				{
					ssidMenus[i] = pstApList[i + (hotspot_current_page - 1) * hotspot_page_perSize].cSsid;
				}

				app_lvgl_listShow("hotspot", ssidMenus, showCnt, currentConnectSsid);
				current_mode = MODE_NETWORK_WIFI_CONFIG;
				startTick = OsSysTick();
			}
		}
		else if (buttonwifi == PGDOWN)
		{
			showCnt = 0;
			memset(ssidMenus, 0, sizeof(ssidMenus));
			if (current_mode == MODE_NETWORK_WIFI_CONFIG)
			{
				if (hotspot_current_page < hotspot_total_page)
				{
					hotspot_current_page++;

					API_LOG_DEBUG("hotspot_current_page:%d,hotspot_page_totalSize:%d,%d\r\n",
								  hotspot_current_page, hotspot_totalCnt, hotspot_page_perSize);
					if (hotspot_current_page == hotspot_total_page)
					{
						if ((hotspot_totalCnt % hotspot_page_perSize) == 0)
						{
							showCnt = hotspot_page_perSize;
						}
						else
						{
							showCnt = hotspot_totalCnt % hotspot_page_perSize;
						}
					}
					else
					{
						showCnt = hotspot_page_perSize;
					}
					API_LOG_DEBUG("wifi next showCnt:%d,hotspot_current_page:%d\r\n",
								  showCnt, hotspot_current_page);
					for (i = 0; i < showCnt; i++)
					{
						ssidMenus[i] = pstApList[i + (hotspot_current_page - 1) * hotspot_page_perSize].cSsid;
					}
					app_lvgl_listShow("hotspot", ssidMenus, showCnt, currentConnectSsid);
				}
			}
		}
		else if (buttonwifi == PGUP)
		{
			showCnt = 0;
			memset(ssidMenus, 0, sizeof(ssidMenus));
			if (current_mode == MODE_NETWORK_WIFI_CONFIG)
			{
				if (hotspot_current_page > 1)
				{
					hotspot_current_page--;

					if (hotspot_current_page == hotspot_total_page)
					{
						if ((hotspot_totalCnt % hotspot_page_perSize) == 0)
						{
							showCnt = hotspot_page_perSize;
						}
						else
						{
							showCnt = hotspot_totalCnt % hotspot_page_perSize;
						}
					}
					else
					{
						showCnt = hotspot_page_perSize;
					}
					for (i = 0; i < showCnt; i++)
					{
						ssidMenus[i] = pstApList[i + (hotspot_current_page - 1) * hotspot_page_perSize].cSsid;
					}
					app_lvgl_listShow("hotspot", ssidMenus, showCnt, currentConnectSsid);
				}
			}
		}
		else if (buttonwifi == FUNCTION)
		{
			if (current_mode == MODE_NETWORK_WIFI_INPUTPWD)
			{
				keyPadMode = ++keyPadMode % 3;
				if (current_mode == MODE_NETWORK_WIFI_INPUTPWD)
					app_lvgl_inputShow(currentSsid, inputBuf, input_mode, 0, keyPadMode, true);
			}
		}
	}
	
	waitflag = 0;
}
static void handle_wifi_phone_setting(void)
{
	int ret = -1;
	char version[64];
	u32 startTick;
	u8 buttonbuf;
	char showname[128];
	current_mode = MODE_WIFI_SETTING;
	memset(showname, 0, sizeof(showname));
	sprintf(showname, "Please use your phone to set up WiFi hotspot :%s", serialID);

	app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, showname, LV_COLOR_DSP_BLUE);

	if (OsParamsGetNetMode() == 1)
	{
		OsWifiOpen();
		OsWifiWake(1);
	}
	else
		OsWifiDisConnectHotSpot();

	ret = OsWifiWebNetConnect(serialID, "", 1000 * 30);
	API_LOG_DEBUG("OsWifiWebNetConnect=[%d]", ret);

	startTick = OsSysTick();
	app_hibernate_reset(1000 * 600);
	waitflag = 1;
	while (1)
	{
		buttonbuf = OsKeypadGetKey();
		API_LOG_DEBUG("app_getkey 22222button=[%d]", buttonbuf);
		if (button == KEYPAD_POWER_LONG)
		{
			API_LOG_DEBUG("Power off");
			app_lvgl_poweroff();
		}
		else if (buttonbuf == CANCEL)
		{
			API_LOG_DEBUG("OsWifiWebNetConnect CANCEL");
			ret = -1;
			break;
		}
		if(current_mode == MODE_NETWORK_SET)
        {
			API_LOG_DEBUG("OsWifiWebNetConnect CANCEL");
			ret = 0;
			break;
		}
		if (OsSysTick() - startTick > 1000 * 300)
		{
			API_LOG_DEBUG("OsWifiWebNetConnect timeout");
			ret = -1;
			break;
		}
		ret = OsWifiGetConnectStatus();
		API_LOG_DEBUG("OsWifiGetConnectStatus =[%d]", ret);
		if (ret == 2)
			break;
		OsSysMsleep(300);
	}
	waitflag = 0;
	if (ret == 2)
	{
		app_lvgl_mainShow(DSP_SUCC_IMG, 100, 100, "WiFi setup successful about to restart", LV_COLOR_DSP_BLUE);
		OsParamsSetNetMode(0);
		// OsWifiClose();
		OsSysMsleep(8000);
		OsSysReboot();
	}
	else if (ret == -1)
	{
		app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100, "WiFi setup failed", LV_COLOR_DSP_RED);
		OsSysMsleep(1500);
		app_hibernate_reset(1000 * 60);
		OsWifiWebNetworkClose();
	}
}
#define WIFIUPDATEURL "https://suntms.sydtechcloud.com:9002/zip/esp-at_V1.00.05.bin.xz.packed"
static void handle_wifi_firmware_update(void)
{
	char version[64] = {0};
	int ret = -1;
	u32 startTick = 0;
	current_mode = MODE_WIFI_SETTING;
	if (OsParamsGetNetMode() == 1)
	{
		app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100, "WiFi firmware upgrade is not supported in 4G mode", LV_COLOR_DSP_RED);
		return;
	}
	app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "Start updating WiFi firmware", LV_COLOR_DSP_BLUE);
	startTick = OsSysTick();
	while (1)
	{
		if (OsSysTick() - startTick > 1000 * 180)
		{
			API_LOG_DEBUG("wifi firmware update timeout");
			app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "wifi firmware update timeout", LV_COLOR_DSP_RED);
			OsSysMsleep(3000);
			break;
		}
	
		ret = OsWifiUserRota(WIFIUPDATEURL, strlen(WIFIUPDATEURL), 180000);
		API_LOG_DEBUG("OsWifiUserRota=[%d]", ret);
		if (ret == 0)
			break;
		OsSysMsleep(2000);
	}
	if (ret == 0)
	{
		app_lvgl_mainShow(DSP_SUCC_IMG, 100, 100, "WiFi firmware update successful please wait for 60 seconds for the device to automatically restart", LV_COLOR_DSP_BLUE);
		OsSysMsleep(1000 * 60);
		OsSysReboot();
	}
}

static void handle_system_param(void)
{
	app_lvgl_menuShow("System Settings", systemMenuItems, sizeof(systemMenuItems) / sizeof(systemMenuItems[0]));
	current_mode = MODE_SYSTEM_SETTING;
}
static void handle_infomation(void)
{
	dsp_termianalInit();
	current_mode = MODE_SYS_INFORMATION;
}

static void handle_param_settings(void)
{

}
static void handle_sign_in(void)
{
	int ret = -1;
	char errorMsg[32] = {0};
	app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "Connecting To Server", LV_COLOR_DSP_BLUE);
	ret = app_dsp_login(MCHNTCD, DEVICEID, DSP_URL, gShopId, gStaffId, errorMsg);
	current_mode = MODE_SYS_PARAMS_SIGNIN_TIP;
	if (ret != 0)
	{
		if (strlen(errorMsg))
		{
			app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100,
							  errorMsg, LV_COLOR_DSP_RED);
		}
		else
		{
			app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100,
							  "device signin fail", LV_COLOR_DSP_RED);
		}
	}
	else
	{

		char msgOut[64] = {0};
		ret = app_dsp_getStaticQrcode(
			MCHNTCD, DEVICEID,
			DSP_URL, gQrUrl, gMchntcnName,
			msgOut);
		API_LOG_DEBUG("app_dsp_getStaticQrcode ret:%d,gQrUrl:%s,mchntcnNameStr:%s\r\n",
					  ret, gQrUrl, gMchntcnName);
		if (ret != 0)
		{
			memset(gQrUrl, 0, sizeof(gQrUrl));
			strcpy(gQrUrl, "https://sun.sydtechcloud.com/payPageKj/html/entry.html?qrCode=/202408260000001953");
		}
		API_LOG_DEBUG("res gQrUrl:%s,mchntcnNameStr:%s\r\n", gQrUrl, gMchntcnName);

		app_lvgl_mainShow(DSP_SUCC_IMG, 100, 100,
						  "device signin success", LV_COLOR_DSP_GREEN);
	}
}
static void handle_sync_time(void)
{
	int ret = -1;
	char rtcTimeStrBcd[14+1] = { 0 };
	char rtcTimeStr[14+1] = { 0 };
	current_mode = MODE_SYS_PARAMS_SET_TIME;
	if (OsParamsGetNetMode() == 0)
	{
		app_lvgl_mainShow(DSP_WAITTING_IMG, 100, 100, "Start Synchronizing", LV_COLOR_DSP_BLUE);
		ret = OsWifiUptime();
		OsSysMsleep(1000);
		if (ret == 7)
		{
			pub_getRtcTime(rtcTimeStr);
			API_LOG_DEBUG_HEX(rtcTimeStr, 14);
			app_hex_to_ascii(rtcTimeStr, rtcTimeStrBcd);
			API_LOG_DEBUG_HEX(rtcTimeStrBcd, 14);
			OsSysSetSpTime(&rtcTimeStrBcd[1]);
			app_lvgl_mainShow(DSP_SUCC_IMG, 100, 100,
							  "time sync success", LV_COLOR_DSP_GREEN);
		}
		else
		{
			app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100,
							  "time sync fail", LV_COLOR_DSP_RED);
		}
	}
	else
		app_lvgl_mainShow(DSP_SUCC_IMG, 100, 100,
			"4G does not require synchronization", LV_COLOR_DSP_GREEN);

}

static void handle_ios_8583(void)
{
	app_lvgl_menuShow("Show 8583", ISO8583MenuItems, sizeof(ISO8583MenuItems) / sizeof(ISO8583MenuItems[0]));
	current_mode = MODE_ISO8583;
}

static void handle_ios_8583_trans(u32 button)
{
	if(button == DIGITAL1)
	{
		API_LOG_DEBUG("Sign set\r\n");
		Uapp_Sign();
		app_lvgl_menuShow("Show 8583", ISO8583MenuItems, sizeof(ISO8583MenuItems) / sizeof(ISO8583MenuItems[0]));
	}
	else if (button == DIGITAL2)
	{
		API_LOG_DEBUG("Sale set\r\n");
		Uapp_Sale();
		app_lvgl_menuShow("Show ISO8583", ISO8583MenuItems, sizeof(ISO8583MenuItems) / sizeof(ISO8583MenuItems[0]));
	}
	else if (button == DIGITAL3)
	{
		API_LOG_DEBUG("Void set\r\n");
		Uapp_Void();
		app_lvgl_menuShow("Show 8583", ISO8583MenuItems, sizeof(ISO8583MenuItems) / sizeof(ISO8583MenuItems[0]));
	}
	else if (button == DIGITAL4)
	{
		API_LOG_DEBUG("Refund set\r\n");
		Uapp_Refund();
		app_lvgl_menuShow("Show 8583", ISO8583MenuItems, sizeof(ISO8583MenuItems) / sizeof(ISO8583MenuItems[0]));
	}
	else if (button == DIGITAL5)
	{
		API_LOG_DEBUG("Balance set\r\n");
		Uapp_Balance();
		app_lvgl_menuShow("Show 8583", ISO8583MenuItems, sizeof(ISO8583MenuItems) / sizeof(ISO8583MenuItems[0]));
	}
}

static void handle_log_switch(void)
{
	app_lvgl_menuShow("Log Switch", logMenuItems, sizeof(logMenuItems) / sizeof(logMenuItems[0]));
	current_mode = MODE_LOGSWITCH_SET;
}
static void handle_log_switch_set(u32 button)
{
	char logSwitch;
	if(button == DIGITAL1)
	{
		API_LOG_DEBUG("LOG set off\r\n");
		OsLogSwitch(-1);
		logSwitch = 0;
	}
	else if (button == DIGITAL2)
	{
		API_LOG_DEBUG("LOG set ON\r\n");
		OsLogSwitch(0);
		logSwitch = 1;
	}
	current_mode = MODE_MENU;
	app_lvgl_menuShow("Menu", menuItems, sizeof(menuItems) / sizeof(menuItems[0]));
}

void  show_sale_menu()
{
	app_lvgl_clearClientBg();
	app_lvgl_menuShow("Show ISO8583", ISO8583MenuItems, sizeof(ISO8583MenuItems) / sizeof(ISO8583MenuItems[0]));
	current_mode = MODE_ISO8583;
	// inputpinpassward = false;
	gTransThreadFlag = false;
}


void app_handle_cardpay(u32 button) {
	API_LOG_DEBUG("app_handle_cardpay show\r\n");
	if (button_length < MAX_AMOUNT_LENGTH)
	{
		if (button_length == 0 && button == DIGITAL0)
		{
			return;
		}
		amount[button_length++] = (char)button;
		amount[button_length] = '\0';
		memset(g_dynamic_amountFormat, 0, sizeof(g_dynamic_amountFormat));
		app_amount_formatting(amount, g_dynamic_amountFormat);
		app_seg_showMoney(g_dynamic_amountFormat);
		app_lvgl_updateInputShow(g_dynamic_amountFormat,0);
	}
}


void app_cardpay_show(u32 button) {

	app_lvgl_inputShow("Amout($)","0.00",0,0,0,false);
	current_mode = MODE_CARDPAY_SHOW;

}