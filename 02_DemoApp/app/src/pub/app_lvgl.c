#include "app_lvgl.h"
#include "app_dsp.h"
#include "app_mutex.h"
#include "app_audio.h"
#include "app_trans.h"
#include "app_button.h"
extern int g_boxReady;
extern AppMode current_mode;
static lv_obj_t *parent = NULL;

static lv_obj_t *wire_img = NULL;
static lv_obj_t *wifi_img = NULL;
static lv_obj_t *soc_img = NULL;
static lv_obj_t *lab_time = NULL;

static lv_obj_t *dynamicqQr = NULL;
static lv_obj_t *staticQr = NULL;
static lv_obj_t *pwd_label = NULL;
static lv_obj_t *param_label = NULL;
extern bool device_Status;

void app_lcd_brush_screen()
{
	api_sys_mutextLock(mutex_lvgl);
	lv_task_handler();
	api_sys_mutextUnlock(mutex_lvgl);
}

const char *wire_filenames[] = {
	"/ext/ui/wire_0.png", "/ext/ui/wire_1.png",
	"/ext/ui/wire_2.png", "/ext/ui/wire_3.png",
	"/ext/ui/wire_4.png", "/ext/ui/wire_5.png"};

const char *wifi_filenames[] = {
	"/ext/ui/wifi_0.png", "/ext/ui/wifi_1.png", "/ext/ui/wifi_2.png",
	"/ext/ui/wifi_3.png", "/ext/ui/wifi_4.png"};

const char *soc_filenames[] = {
	"/ext/ui/soc_0.png", "/ext/ui/soc_1.png",
	"/ext/ui/soc_2.png", "/ext/ui/soc_3.png",
	"/ext/ui/soc_4.png", "/ext/ui/soc_5.png",
	"/ext/ui/soc_6.png", "/ext/ui/soc_7.png"};



void app_show_iconStatus(int type, int value)
{
	static int flag = 0;
	char rtcTimeStr[16] = {0};
	static char rtcTimeStrDisp[16] = {0};
	static int freshflag=0;
	static int freshentick=0;
	int i = 0;
	if (g_boxReady != 1 || lv_get_drv_flag() == 0)
	{
		return;
	}
	api_sys_mutextLock(mutex_lvgl);
	if (flag == 0)
	{
		flag = 1;
		for (i = 0; i < sizeof(wire_filenames) / sizeof(wire_filenames[0]); i++)
			lv_load_png_file(wire_filenames[i]);
		for (i = 0; i < sizeof(wifi_filenames) / sizeof(wifi_filenames[0]); i++)
			lv_load_png_file(wifi_filenames[i]);
		for (i = 0; i < sizeof(soc_filenames) / sizeof(soc_filenames[0]); i++)
			lv_load_png_file(soc_filenames[i]);
		lv_obj_t *statusBarBg = lv_obj_create(lv_scr_act(), NULL);
		lv_obj_set_size(statusBarBg, 240, STATUSBAR_HEIGT);
		lv_obj_set_style_local_bg_color(statusBarBg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xF3, 0xF3, 0xF3));
		lv_obj_set_style_local_bg_opa(statusBarBg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 254);
		lv_obj_set_style_local_radius(statusBarBg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
		lv_obj_set_style_local_border_width(statusBarBg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
		lv_obj_set_style_local_border_color(statusBarBg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
		lv_obj_t *parent = app_lvgl_clientBg();
		lv_obj_t *standByObj = lv_obj_create(parent, NULL);
		lv_obj_set_size(standByObj, lv_obj_get_width(parent), lv_obj_get_height(parent) - STATUSBAR_HEIGT);
		lv_obj_set_style_local_bg_color(standByObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
		lv_obj_set_style_local_radius(standByObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
		lv_obj_set_style_local_border_width(standByObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
		lv_obj_set_style_local_border_color(standByObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
		lv_obj_align(standByObj, parent, 0, LV_ALIGN_IN_TOP_LEFT, STATUSBAR_HEIGT);
	}
	if (type == 0)
	{
		if (wifi_img)
		{
			lv_obj_del(wifi_img);
			wifi_img = 0;
		}
		if (value >= 0 && value <= 5)
		{
			if (value == 0)
			{
				if (wire_img)
				{
					lv_obj_del(wire_img);
					wire_img = 0;
				}
			}
			else
			{
				if (wire_img == 0 && (value != 0))
				{
					API_LOG_DEBUG("%s file create\r\n", wire_filenames[value]);
					api_lcd_showPng(6, 5, WIRE_IMG_WIDTH, WIRE_IMG_HEIGHT,
									wire_filenames[value], &wire_img);
				}
				else
				{
					if (wire_img)
						lv_img_set_src(wire_img, wire_filenames[value]);
				}
			}
		}
	}
	else if (type == 1 && (api_params_getNetMode() == 0))
	{
		if (value >= 1 && value <= 4)
		{
			if (wifi_img == 0)
			{
				api_lcd_showPng(40, 5, WIFI_IMG_WIDTH, WIFI_IMG_HEIGHT,
								wifi_filenames[value], &wifi_img);
			}
			else
			{
				lv_img_set_src(wifi_img, wifi_filenames[value]);
			}
		}
		else
		{
			if (wifi_img)
			{
				lv_obj_del(wifi_img);
				wifi_img = 0;
			}
		}
	}
	else if (type == 2)
	{
		if (value >= 0 && value <= 7)
		{
			if (soc_img == 0)
			{
				api_lcd_showPng(207, 8, SOC_IMG_WIDTH, SOC_IMG_HEIGT,
								soc_filenames[value], &soc_img);
			}
			else
			{
				lv_img_set_src(soc_img, soc_filenames[value]);
			}
		}
		else
		{
			API_LOG_DEBUG("wifi value=%d is error\r\n", value);
		}
	}
	if (lab_time == NULL)
	{
		if (type != 2)
		{
			api_sys_mutextUnlock(mutex_lvgl);
			return;
		}
		
		lab_time = lv_label_create(lv_scr_act(), NULL);
		lv_label_set_long_mode(lab_time, LV_LABEL_LONG_BREAK);
		lv_obj_set_width(lab_time, 100);
		lv_obj_set_pos(lab_time, 93, 3);
		lv_obj_set_style_local_text_color(lab_time, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
		lv_obj_set_style_local_radius(lab_time, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
		lv_obj_set_style_local_border_width(lab_time, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
		lv_obj_set_style_local_border_color(lab_time, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
		lv_obj_set_style_local_text_font(lab_time, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_20);
		app_standby_signal();
	}
    if(freshflag==0)
	{
	   pub_getRtcTime(rtcTimeStr);
	   memset(rtcTimeStrDisp,0,sizeof(rtcTimeStrDisp));
	   strncpy(rtcTimeStrDisp, rtcTimeStr + 8, 2);
	   strcat(rtcTimeStrDisp, ":");
	   strncpy(rtcTimeStrDisp + strlen(rtcTimeStrDisp), rtcTimeStr + 10, 2);
	   freshentick=api_sys_tick();
       freshflag=1;
	}
	if(api_sys_tick()-freshentick > 1000)
	{
		freshflag=0;
	}
	lv_label_set_text(lab_time, rtcTimeStrDisp);
	api_sys_mutextUnlock(mutex_lvgl);
	app_lcd_brush_screen();
}

lv_obj_t *app_lvgl_clientBg()
{

	if (parent == NULL)
	{
		parent = lv_obj_create(lv_scr_act(), NULL);
		lv_obj_set_size(parent, 240, 320);
		lv_obj_set_style_local_bg_color(parent, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
		lv_obj_set_style_local_radius(parent, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
		lv_obj_set_style_local_border_width(parent, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
		lv_obj_set_style_local_border_color(parent, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
	}
	// API_LOG_DEBUG("parent=%p",parent);
	return parent;
}
void app_lvgl_iconClearAll()
{
	for (int i = 0; i < MAX_ICON_OBJS; i++)
	{
		if (g_icons[i].img_obj)
		{
			lv_obj_del(g_icons[i].img_obj);
			lv_img_cache_invalidate_src(g_icons[i].img_path);
			lv_free_png_file(g_icons[i].img_path);
			g_icons[i].img_obj = NULL;
			memset(g_icons[i].img_path, 0, sizeof(g_icons[i].img_path));
		}
	}
}

void _lvgl_clearAllClientObj()
{
	app_lvgl_iconClearAll();
	if (dynamicqQr)
	{
		lv_qrcode_delete(dynamicqQr);
		dynamicqQr = NULL;
	}
	if (staticQr)
	{
		lv_qrcode_delete(staticQr);
		staticQr = NULL;
	}
	lv_obj_t *obj = app_lvgl_clientBg();

	lv_obj_t *child = lv_obj_get_child(obj, NULL);
	while (child != NULL)
	{
		lv_obj_t *next = lv_obj_get_child(obj, child);
		lv_obj_del(child);
		child = next;
	}
	if (pwd_label)
	{
		pwd_label = NULL;
	}
	if (param_label)
	{
		param_label = NULL;
	}
}

void app_lvgl_clearClientBg()
{
	api_sys_mutextLock(mutex_lvgl);
	_lvgl_clearAllClientObj();
	api_sys_mutextUnlock(mutex_lvgl);
}

void app_lvgl_iconShow(lv_obj_t *parent, char *imgSrc, lv_align_t align, int x_offset, int y_offset)
{
	if (!imgSrc || strlen(imgSrc) == 0)
	{
		return;
	}

	int slot = -1;
	for (int i = 0; i < MAX_ICON_OBJS; i++)
	{
		if (g_icons[i].img_obj == NULL)
		{
			slot = i;
			break;
		}
	}

	if (slot == -1)
	{
		return;
	}

	lv_img_cache_invalidate_src(imgSrc);
	lv_load_png_file(imgSrc);

	if (!parent)
		parent = lv_scr_act();
	lv_obj_t *img = lv_img_create(parent, NULL);
	lv_img_set_src(img, imgSrc);
	lv_obj_align(img, parent, align, x_offset, y_offset);

	g_icons[slot].img_obj = img;
	strncpy(g_icons[slot].img_path, imgSrc, sizeof(g_icons[slot].img_path) - 1);
	return;
}

char gadvinfo[513] = {0};
void app_static_qrcode_show(char *qrurl, char *merchantName)
{
	app_lvgl_clearClientBg();
	API_LOG_DEBUG("app_static_qrcode_show\r\n");
	api_sys_mutextLock(mutex_lvgl);
	lv_obj_t *parent = app_lvgl_clientBg();
	lv_obj_t *logo_img = NULL, *merchant_lab = NULL, *lineObj = NULL;
	API_LOG_DEBUG("parent=%p,qrurl:%s\r\n", parent, qrurl);

	logo_img = lv_label_create(parent, NULL);
	lv_label_set_long_mode(logo_img, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(logo_img, lv_obj_get_width(parent));
	lv_label_set_align(logo_img, LV_LABEL_ALIGN_CENTER);
	lv_label_set_text(logo_img, "QR Pay");
	lv_obj_align(logo_img, parent, LV_ALIGN_IN_TOP_LEFT, 0, 36);
	lv_obj_set_style_local_text_color(logo_img, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_radius(logo_img, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_width(logo_img, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_color(logo_img, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
	lv_obj_set_style_local_text_font(logo_img, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_20);
	staticQr = lv_qrcode_create(parent, 220, LV_COLOR_BLACK, LV_COLOR_WHITE);
	lv_qrcode_update(staticQr, qrurl, strlen(qrurl));
	lv_obj_align(staticQr, NULL, LV_ALIGN_IN_TOP_MID, 0, 65);

	lineObj = lv_label_create(parent, logo_img);
	lv_label_set_long_mode(lineObj, LV_LABEL_LONG_EXPAND);
	lv_obj_set_size(lineObj, 240, 2);
	lv_label_set_align(lineObj, LV_LABEL_ALIGN_LEFT);
	lv_obj_align(lineObj, parent, LV_ALIGN_IN_TOP_LEFT, 0, 275);
	lv_label_set_text_fmt(lineObj, "%s", "__________________________________");
	lv_obj_set_style_local_text_color(lineObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
	lv_obj_set_style_local_text_font(lineObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_14);

	merchant_lab = lv_label_create(parent, logo_img);
	lv_label_set_long_mode(merchant_lab, LV_LABEL_LONG_SROLL_CIRC);
	lv_obj_set_width(merchant_lab, lv_obj_get_width(parent));
	lv_label_set_align(merchant_lab, LV_LABEL_ALIGN_CENTER);
	lv_label_set_anim_speed(merchant_lab, 10);
	if (strlen(gadvinfo) != 0)
		lv_label_set_text(merchant_lab, gadvinfo);
	else
		lv_label_set_text(merchant_lab, merchantName);

	lv_obj_set_style_local_text_font(merchant_lab, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_20);
	lv_obj_align(merchant_lab, parent, LV_ALIGN_IN_TOP_LEFT, 0, 291);
	API_LOG_DEBUG("app_static_qrcode_show end\r\n");
	api_sys_mutextUnlock(mutex_lvgl);

	app_lcd_brush_screen();
}

void app_lvgl_dynamicQrShow(char *amount, char *qrcode)
{
	app_lvgl_clearClientBg();
	api_sys_mutextLock(mutex_lvgl);
	lv_obj_t *parent = app_lvgl_clientBg();

	lv_obj_t *labTitle = lv_label_create(parent, NULL);
	lv_label_set_long_mode(labTitle, LV_LABEL_LONG_BREAK);
	lv_label_set_align(labTitle, LV_LABEL_ALIGN_CENTER);
	lv_label_set_text(labTitle, "QR&Card Pay");
	lv_obj_set_width(labTitle, 240);
	lv_obj_align(labTitle, parent, LV_ALIGN_IN_TOP_LEFT, 0, 36);
	lv_obj_set_style_local_text_color(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_radius(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_width(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_color(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
	lv_obj_set_style_local_text_font(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_16);

	lv_obj_t *amountObj = lv_label_create(parent, NULL);
	lv_label_set_long_mode(amountObj, LV_LABEL_LONG_BREAK);
	lv_obj_set_size(amountObj, 240, 24);
	lv_label_set_align(amountObj, LV_LABEL_ALIGN_CENTER);
	lv_obj_align(amountObj, parent, LV_ALIGN_IN_TOP_MID, 0, 60);
	lv_label_set_text_fmt(amountObj, "$%s", amount);
	lv_obj_set_style_local_text_color(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
	lv_obj_set_style_local_radius(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_width(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_color(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
	lv_obj_set_style_local_text_font(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);

	dynamicqQr = lv_qrcode_create(parent, 200, LV_COLOR_BLACK, LV_COLOR_WHITE);
	lv_qrcode_update(dynamicqQr, qrcode, strlen(qrcode));
	lv_obj_align(dynamicqQr, parent, LV_ALIGN_IN_BOTTOM_MID, 0, -20);
	api_sys_mutextUnlock(mutex_lvgl);
	app_lcd_brush_screen();
}



void app_lvgl_cardpayShow(char* amount)
{
	lv_load_png_file("/ext/ui/rfcard.png");
	app_lvgl_clearClientBg();
	api_sys_mutextLock(mutex_lvgl);
	lv_obj_t* parent = app_lvgl_clientBg();
	lv_obj_t* labTitle = lv_label_create(parent, NULL);
	lv_label_set_long_mode(labTitle, LV_LABEL_LONG_BREAK);
	lv_label_set_align(labTitle, LV_LABEL_ALIGN_CENTER);
	lv_label_set_text(labTitle, "Card Pay");
	lv_obj_set_width(labTitle, 240);
	lv_obj_align(labTitle, parent, LV_ALIGN_IN_TOP_LEFT, 0, 36);
	lv_obj_set_style_local_text_color(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_radius(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_width(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_color(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
	lv_obj_set_style_local_text_font(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_16);

	lv_obj_t* amountObj = lv_label_create(parent, NULL);
	lv_label_set_long_mode(amountObj, LV_LABEL_LONG_BREAK);
	lv_obj_set_size(amountObj, 240, 24);
	lv_label_set_align(amountObj, LV_LABEL_ALIGN_CENTER);
	lv_obj_align(amountObj, parent, LV_ALIGN_IN_TOP_MID, 0, 60);
	lv_label_set_text_fmt(amountObj, "$%s", amount);
	lv_obj_set_style_local_text_color(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
	lv_obj_set_style_local_radius(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_width(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_color(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
	lv_obj_set_style_local_text_font(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);

	lv_obj_t  *img = lv_img_create(parent, NULL);
	lv_obj_set_size(img, 184, 111);
	lv_img_set_src(img, "/ext/ui/rfcard.png");
	lv_obj_align(img, parent, LV_ALIGN_CENTER, 0, 50);
	api_sys_mutextUnlock(mutex_lvgl);
	app_lcd_brush_screen();
}

void app_lvgl_card_retap()
{
	lv_load_png_file("/ext/ui/rfcard.png");
	app_lvgl_clearClientBg();
	api_sys_mutextLock(mutex_lvgl);
	lv_obj_t* parent = app_lvgl_clientBg();
	lv_obj_t* labTitle = lv_label_create(parent, NULL);
	lv_label_set_long_mode(labTitle, LV_LABEL_LONG_BREAK);
	lv_label_set_align(labTitle, LV_LABEL_ALIGN_CENTER);
	lv_label_set_text(labTitle, "Card Pay");
	lv_obj_set_width(labTitle, 240);
	lv_obj_align(labTitle, parent, LV_ALIGN_IN_TOP_LEFT, 0, 36);
	lv_obj_set_style_local_text_color(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_radius(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_width(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_color(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
	lv_obj_set_style_local_text_font(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_16);

	lv_obj_t* amountObj = lv_label_create(parent, NULL);
	lv_label_set_long_mode(amountObj, LV_LABEL_LONG_BREAK);
	lv_obj_set_size(amountObj, 240, 24);
	lv_label_set_align(amountObj, LV_LABEL_ALIGN_CENTER);
	lv_obj_align(amountObj, parent, LV_ALIGN_IN_TOP_MID, 0, 60);
	lv_label_set_text_fmt(amountObj, "%s", "Please tap card again");
	lv_obj_set_style_local_text_color(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
	lv_obj_set_style_local_radius(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_width(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_color(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
	lv_obj_set_style_local_text_font(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);

	lv_obj_t  *img = lv_img_create(parent, NULL);
	lv_obj_set_size(img, 184, 111);
	lv_img_set_src(img, "/ext/ui/rfcard.png");
	lv_obj_align(img, parent, LV_ALIGN_CENTER, 0, 50);
	api_sys_mutextUnlock(mutex_lvgl);
	app_lcd_brush_screen();
}

void app_lvgl_inputPinShow(char *title)
{
	app_lvgl_clearClientBg();
	api_sys_mutextLock(mutex_lvgl);
	lv_obj_t *parent = app_lvgl_clientBg();
	lv_obj_t *blueBlank_img = NULL;

	lv_obj_t *labTitle = lv_label_create(parent, NULL);
	lv_label_set_long_mode(labTitle, LV_LABEL_LONG_BREAK);
	lv_label_set_align(labTitle, LV_LABEL_ALIGN_CENTER);
	lv_label_set_text(labTitle, title);
	lv_obj_set_width(labTitle, 240);
	lv_obj_align(labTitle, parent, LV_ALIGN_IN_TOP_LEFT, 0, STATUSBAR_HEIGT);

	lv_obj_set_style_local_text_color(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_radius(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_width(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_color(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
	lv_obj_set_style_local_text_font(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);

	static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(210, 34)];
	lv_obj_t *canvas = lv_canvas_create(parent, NULL);
	lv_canvas_set_buffer(canvas, cbuf, 210, 34, LV_IMG_CF_TRUE_COLOR);

	lv_canvas_fill_bg(canvas, LV_COLOR_WHITE, LV_OPA_TRANSP);
	lv_obj_set_style_local_bg_color(canvas, LV_CANVAS_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_obj_set_style_local_border_width(canvas, LV_CANVAS_PART_MAIN, LV_STATE_DEFAULT, 0);

	lv_draw_rect_dsc_t rect_dsc;
	lv_draw_rect_dsc_init(&rect_dsc);
	rect_dsc.radius = 9;
	rect_dsc.bg_color = LV_COLOR_WHITE;
	rect_dsc.bg_opa = LV_OPA_COVER;
	rect_dsc.border_width = 1;
	lv_canvas_draw_rect(canvas, 0, 0, 210, 34, &rect_dsc);

	lv_obj_align(canvas, parent, LV_ALIGN_IN_TOP_MID, 0, 143);

	pwd_label = lv_label_create(parent, NULL);
	lv_label_set_long_mode(pwd_label, LV_LABEL_LONG_BREAK);
	lv_obj_set_size(pwd_label, 210, 34);
	lv_label_set_align(pwd_label, LV_LABEL_ALIGN_CENTER);
	lv_label_set_text(pwd_label, "");
	lv_obj_set_style_local_radius(pwd_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_width(pwd_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_color(pwd_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
	lv_obj_set_style_local_text_font(pwd_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);
	lv_obj_set_style_local_bg_color(pwd_label, LV_CANVAS_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_obj_set_style_local_bg_opa(pwd_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_text_letter_space(pwd_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 5);
	lv_obj_align(pwd_label, parent, LV_ALIGN_IN_TOP_MID, 0, 153);
	api_sys_mutextUnlock(mutex_lvgl);

	app_lcd_brush_screen();
}

void app_lvgl_inputShow(char *title, char *text, int inputMode, int plainMode, char dispMode,bool showtips)
{
	int i = 0;
	char tempBuf[24] = {0};

	api_sys_mutextLock(mutex_lvgl);
	_lvgl_clearAllClientObj();
	lv_obj_t *parent = app_lvgl_clientBg();
	lv_obj_t *blueBlank_img = NULL;
	lv_obj_t *labTitle = lv_label_create(parent, NULL);
	lv_label_set_long_mode(labTitle, LV_LABEL_LONG_BREAK);
	lv_label_set_align(labTitle, LV_LABEL_ALIGN_CENTER);
	lv_label_set_text(labTitle, title);
	lv_obj_set_width(labTitle, 240);
	lv_obj_align(labTitle, parent, LV_ALIGN_IN_TOP_LEFT, 0, STATUSBAR_HEIGT);
	lv_obj_set_style_local_text_color(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_radius(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_width(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_color(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
	lv_obj_set_style_local_text_font(labTitle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);

	lv_obj_t *label_bj = lv_obj_create(parent, NULL);

	pwd_label = lv_label_create(label_bj, NULL);

	lv_label_set_long_mode(pwd_label, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(pwd_label, 210);
	lv_label_set_align(pwd_label, LV_LABEL_ALIGN_CENTER);
	if (plainMode == 0)
	{
		lv_label_set_text(pwd_label, text);
	}
	else
	{
		for (i = 0; i < strlen(text); i++)
		{
			strcat(tempBuf, "*");
		}
		lv_label_set_text(pwd_label, tempBuf);
	}

	lv_obj_set_style_local_radius(pwd_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_width(pwd_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_color(pwd_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_text_font(pwd_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_14);
	lv_obj_set_style_local_bg_color(pwd_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_obj_set_style_local_bg_opa(pwd_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_text_letter_space(pwd_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 5);

	if (lv_obj_get_height(pwd_label) < 34)
	{
		lv_obj_set_size(label_bj, 210, 34);
	}
	else
	{
		lv_obj_set_size(label_bj, 210, lv_obj_get_height(pwd_label) + 2);
	}

	lv_obj_set_style_local_radius(label_bj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 9);
	lv_obj_set_style_local_border_width(label_bj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 1);
	lv_obj_set_style_local_border_color(label_bj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_bg_color(label_bj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_obj_set_style_local_bg_opa(label_bj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

	lv_obj_align(pwd_label, label_bj, LV_ALIGN_CENTER, 0, 0);

	lv_obj_align(label_bj, parent, LV_ALIGN_IN_TOP_MID, 0, 153);

	if (strcmp(title, "SET MACHINE TIME") == 0 || strcmp(title, "TMS MQTT TLS") == 0 || strcmp(title, "TMS HTTP TLS") == 0)
	{
		lv_obj_t *timeFormat = lv_label_create(parent, NULL);
		lv_label_set_long_mode(timeFormat, LV_LABEL_LONG_BREAK);
		lv_obj_set_size(timeFormat, 240, 10);
		lv_label_set_align(timeFormat, LV_LABEL_ALIGN_CENTER);
		if (strcmp(title, "SET MACHINE TIME") == 0)
			lv_label_set_text_fmt(timeFormat, "format:%s", "YYYYMMDDHHMMSS");
		else if (strcmp(title, "TMS MQTT TLS") == 0 || strcmp(title, "TMS HTTP TLS") == 0)
			lv_label_set_text_fmt(timeFormat, "0-not encrypted 1-Encryption");
		lv_obj_set_style_local_text_font(timeFormat, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_16);

		lv_obj_set_style_local_radius(timeFormat, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
		lv_obj_set_style_local_border_width(timeFormat, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
		lv_obj_set_style_local_border_color(timeFormat, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
		lv_obj_set_style_local_bg_color(timeFormat, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
		lv_obj_set_style_local_bg_opa(timeFormat, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

		lv_obj_align(timeFormat, parent, LV_ALIGN_IN_TOP_LEFT, 0, 200);
	}

	if (showtips) {
		lv_obj_t* keypadTips = lv_label_create(parent, NULL);
		lv_label_set_long_mode(keypadTips, LV_LABEL_LONG_BREAK);
		lv_obj_set_size(keypadTips, 150, 10);
		lv_label_set_align(keypadTips, LV_LABEL_ALIGN_CENTER);
		lv_label_set_text_fmt(keypadTips, "%s", "function key change");
		lv_obj_set_style_local_text_font(keypadTips, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_12);
		lv_obj_set_style_local_radius(keypadTips, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
		lv_obj_set_style_local_border_width(keypadTips, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
		lv_obj_set_style_local_border_color(keypadTips, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
		lv_obj_set_style_local_bg_color(keypadTips, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
		lv_obj_set_style_local_bg_opa(keypadTips, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
		lv_obj_align(keypadTips, parent, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
		lv_obj_t* inputModeObj = lv_label_create(parent, NULL);
		lv_label_set_long_mode(inputModeObj, LV_LABEL_LONG_BREAK);
		lv_obj_set_size(inputModeObj, 80, 10);
		lv_label_set_align(inputModeObj, LV_LABEL_ALIGN_RIGHT);
		if (dispMode == 0)
			lv_label_set_text_fmt(inputModeObj, "%s", "0-9");
		if (dispMode == 1)
			lv_label_set_text_fmt(inputModeObj, "%s", "a-z");
		if (dispMode == 2)
			lv_label_set_text_fmt(inputModeObj, "%s", "A-Z");
		lv_obj_set_style_local_text_font(inputModeObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_12);
		lv_obj_set_style_local_radius(inputModeObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
		lv_obj_set_style_local_border_width(inputModeObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
		lv_obj_set_style_local_border_color(inputModeObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
		lv_obj_set_style_local_bg_color(inputModeObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
		lv_obj_set_style_local_bg_opa(inputModeObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
		lv_obj_align(inputModeObj, parent, LV_ALIGN_IN_BOTTOM_RIGHT, -2, -2);
	}
	api_sys_mutextUnlock(mutex_lvgl);
	app_lcd_brush_screen();
}

void app_lvgl_updateInputShow(char *text, int plainMode)
{

	char tempBuf[24] = {0};
	api_sys_mutextLock(mutex_lvgl);
	if (pwd_label != NULL)
	{
		if (plainMode == 0)
		{
			lv_label_set_text(pwd_label, text);
		}
		else
		{
			for (int i = 0; i < strlen(text); i++)
			{
				strcat(tempBuf, "*");
			}
			lv_label_set_text(pwd_label, tempBuf);
		}
	}
	api_sys_mutextUnlock(mutex_lvgl);
}

void app_lvgl_updateInputpinShow(int pwdlen)
{

	char maxPwdBuf[13] = {0};
	for (int i = 0; i < pwdlen; i++)
	{
		strcat(maxPwdBuf, "*");
	}
	api_sys_mutextLock(mutex_lvgl);
	if (pwd_label != NULL)
	{
		lv_label_set_text(pwd_label, maxPwdBuf);
	}
	api_sys_mutextUnlock(mutex_lvgl);
}

void app_transaction_resultShow(int tradetype, int result, char *amount, char *transactionId, char *orderID)
{
	char dateStr[15] = {0};
	char dateStrFormat[64] = {0};
	pub_getRtcTime(dateStr);
	Pub_DateConvert(dateStr, "YYYY/MM/DD hh:mm:ss", dateStrFormat);
	api_sys_mutextLock(mutex_lvgl);
	_lvgl_clearAllClientObj();
	lv_obj_t *parent = app_lvgl_clientBg();
	if (result == 0)
		app_lvgl_iconShow(parent, DSP_PENDINGSYMBOL6060_IMG, LV_ALIGN_IN_TOP_MID, 0, 40);
	else if (result == 1)
		app_lvgl_iconShow(parent, DSP_APPROVESYMBOL6060_IMG, LV_ALIGN_IN_TOP_MID, 0, 40);
	else if (result == 2)
		app_lvgl_iconShow(parent, DSP_DECLINESYMBOL6060_IMG, LV_ALIGN_IN_TOP_MID, 0, 40);

	lv_obj_t *amountObj = lv_label_create(parent, NULL);
	lv_label_set_long_mode(amountObj, LV_LABEL_LONG_BREAK);
	lv_obj_set_size(amountObj, 240, 24);
	lv_label_set_align(amountObj, LV_LABEL_ALIGN_CENTER);
	lv_obj_align(amountObj, parent, LV_ALIGN_IN_TOP_MID, 0, 110);
	lv_label_set_text_fmt(amountObj, "$%s", amount);
	if (result == 0)
		lv_obj_set_style_local_text_color(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_DSP_BLUE);
	else if (result == 1)
		lv_obj_set_style_local_text_color(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_DSP_GREEN);
	else if (result == 2)
		lv_obj_set_style_local_text_color(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_DSP_RED);
	lv_obj_set_style_local_radius(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_width(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_color(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
	lv_obj_set_style_local_text_font(amountObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_22);

	lv_obj_t *trantypeLab = lv_label_create(parent, NULL);
	lv_label_set_long_mode(trantypeLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_size(trantypeLab, 200, 20);
	lv_obj_align(trantypeLab, parent, LV_ALIGN_IN_TOP_LEFT, 10, 178);
	lv_label_set_text_fmt(trantypeLab, "Trans Type:%s", "SALE");
	lv_obj_set_style_local_text_color(trantypeLab, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_radius(trantypeLab, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_width(trantypeLab, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_color(trantypeLab, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TRANSP);
	lv_obj_set_style_local_text_font(trantypeLab, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_14);

	lv_obj_t *transResultLab = lv_label_create(parent, trantypeLab);
	lv_label_set_long_mode(transResultLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_size(transResultLab, 200, 20);
	lv_obj_align(transResultLab, parent, LV_ALIGN_IN_TOP_LEFT, 10, 178 + (20 + 12));
	if (result == 0)
	{
		lv_label_set_text(transResultLab, "Trans Result:Pending");
	}
	else if (result == 1)
	{
		lv_label_set_text(transResultLab, "Trans Result:Approved");
	}
	else if (result == 2)
	{
		lv_label_set_text(transResultLab, "Trans Result:Declined");
	}
	lv_obj_t *orderIdLab = lv_label_create(parent, trantypeLab);
	lv_label_set_long_mode(orderIdLab, LV_LABEL_LONG_BREAK);
	lv_obj_align(orderIdLab, parent, LV_ALIGN_IN_TOP_LEFT, 10, 178 + (20 + 12) * 2);
	lv_label_set_text_fmt(orderIdLab, "Order Id:%s", transactionId);

	lv_obj_t *dateLab = lv_label_create(parent, trantypeLab);
	lv_label_set_long_mode(dateLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_size(dateLab, 240, 20);
	lv_obj_align(dateLab, parent, LV_ALIGN_IN_TOP_LEFT, 10, 178 + (20 + 12) * 3);
	lv_label_set_text_fmt(dateLab, "Trans Time:%s", dateStrFormat);
	api_sys_mutextUnlock(mutex_lvgl);
	app_lcd_brush_screen();
}

void app_lvgl_menuShow(char *title, char *menuItems[], int itemSize)
{

	if (!lcdType)
		return;
	int i;
	app_lvgl_clearClientBg();
	api_sys_mutextLock(mutex_lvgl);
	lv_obj_t *parent = app_lvgl_clientBg();
	for (i = 0; i < itemSize; i++)
	{
		lv_obj_t *cont = lv_cont_create(parent, NULL);
		lv_obj_set_size(cont, MENU_ITEM_WIDTH, MENU_ITEM_HEIGHT);

		lv_obj_set_pos(cont, 10, (STATUSBAR_HEIGT + MENU_ITEM_SPACE) * (i + 1));
		lv_cont_set_fit(cont, LV_FIT_NONE);
		lv_cont_set_layout(cont, LV_LAYOUT_OFF);
		lv_obj_t *label = lv_label_create(cont, NULL);
		lv_label_set_text(label, menuItems[i]);
		lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
		lv_obj_set_size(label, MENU_ITEM_WIDTH - 8, MENU_ITEM_HEIGHT);
		lv_obj_set_style_local_text_color(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
		lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_arial_14);
		lv_obj_set_style_local_border_side(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_FULL);
		lv_obj_set_style_local_border_width(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 1);
		lv_obj_set_style_local_border_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
		lv_obj_set_style_local_radius(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 8);
		lv_obj_set_style_local_bg_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_DSP_ASH);

		lv_obj_align(label, cont, LV_ALIGN_IN_LEFT_MID, 2, 0);
	}
	api_sys_mutextUnlock(mutex_lvgl);
	app_lcd_brush_screen();
	API_LOG_DEBUG("app_lvgl_menuShow brush\r\n");
}

void app_lvgl_listShow(char *title, char *menuItems[], int itemSize, char *selectItemName)
{
	if (!lcdType)
		return;
	int i;

	app_lvgl_clearClientBg();
	api_sys_mutextLock(mutex_lvgl);
	lv_obj_t *parent = app_lvgl_clientBg();

	int block_height = floor((lv_obj_get_height(parent) - 32) / LIST_ITEM_SHOW_MAX_CNT);

	for (i = 0; i < itemSize; i++)
	{
		lv_obj_t *cont = lv_cont_create(parent, NULL);
		lv_obj_set_size(cont, LIST_ITEM_WIDTH, block_height - LIST_ITEM_SPACE);
		lv_obj_set_pos(cont, 5, (STATUSBAR_HEIGT + 2) + (block_height) * (i));
		lv_cont_set_fit(cont, LV_FIT_NONE);
		lv_cont_set_layout(cont, LV_LAYOUT_OFF);
		lv_obj_t *label = lv_label_create(cont, NULL);
		lv_label_set_text_fmt(label, "%d.%s", i + 1, menuItems[i]);
		lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
		lv_obj_set_size(label, LIST_ITEM_WIDTH - 8, LIST_ITEM_HEIGHT);
		lv_obj_set_style_local_text_color(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
		lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_14);
		lv_obj_set_style_local_border_side(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_FULL);
		lv_obj_set_style_local_border_width(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 1);
		lv_obj_set_style_local_border_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
		lv_obj_set_style_local_radius(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 2);
		API_LOG_DEBUG("selectItemName=[%d] ,[%s],[%d]\r\n", i, selectItemName, strlen(selectItemName));
		API_LOG_DEBUG("menuItems[i]=[%d] ,[%s],[%d]\r\n", i, menuItems[i], strlen(menuItems[i]));
		if (strncmp(selectItemName, menuItems[i], strlen(selectItemName)) == 0 && strlen(selectItemName) != 0)
		{
			lv_obj_set_style_local_bg_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_DSP_BLUE);
		}
		else
			lv_obj_set_style_local_bg_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_DSP_ASH);
		lv_obj_align(label, cont, LV_ALIGN_IN_LEFT_MID, 2, 0);
	}
	api_sys_mutextUnlock(mutex_lvgl);
}

lv_obj_t *app_lvgl_createTitle(char *title)
{
	lv_obj_t *titleLab = lv_label_create(app_lvgl_clientBg(), NULL);
	lv_label_set_align(titleLab, LV_LABEL_ALIGN_CENTER);
	lv_obj_set_size(titleLab, 240, TITLE_HEIGHT);
	lv_label_set_text(titleLab, title);
	lv_obj_align(titleLab, app_lvgl_clientBg(), LV_ALIGN_IN_TOP_MID, 0, STATUSBAR_HEIGT + 2);
	lv_obj_set_style_local_text_color(titleLab, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_text_font(titleLab, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_16);
	return titleLab;
}

void app_lvgl_versionInfoShow()
{
	if (!lcdType)
		return;
	char buf[128];
	u8 version[64];
	app_lvgl_clearClientBg();
	api_sys_mutextLock(mutex_lvgl);

	lv_obj_t *parent = app_lvgl_clientBg();
	lv_obj_t *titleLab = app_lvgl_createTitle("Version");
	lv_obj_t *bgObj = lv_obj_create(parent, NULL);
	lv_obj_set_size(bgObj, 240, 260);
	lv_obj_set_style_local_bg_color(bgObj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_DSP_ASH);
	// lv_obj_set_style_local_bg_opa(bgObj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_40);
	lv_obj_set_style_local_border_width(bgObj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_color(bgObj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_obj_set_style_local_radius(bgObj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_align(bgObj, parent, LV_ALIGN_IN_TOP_LEFT, 0, STATUSBAR_HEIGT + TITLE_HEIGHT);
	//////////////////////////////////////
	lv_obj_t *app = lv_label_create(bgObj, NULL);
	lv_label_set_long_mode(app, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(app, 80);
	lv_label_set_text(app, "APP:");
	lv_obj_set_style_local_text_color(app, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_text_font(app, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_12);
	lv_obj_align(app, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14);

	memset(version, 0, sizeof(version));
	api_sys_getFirmVersion(version, APP_VER);
	lv_obj_t *appVer = lv_label_create(bgObj, app);
	lv_label_set_long_mode(appVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(appVer, 155);
	lv_label_set_text(appVer, version);
	lv_label_set_align(appVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(appVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 80, 14);
	///////////////////////////////////////

	lv_obj_t *spbootLab = lv_label_create(bgObj, app);
	lv_label_set_long_mode(spbootLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(spbootLab, 80);
	lv_label_set_text(spbootLab, "WIFI_VER:");
	lv_obj_align(spbootLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24);

	memset(version, 0, sizeof(version));
	api_wifi_getVersion(version, sizeof(version));
	lv_obj_t *spbootVer = lv_label_create(bgObj, app);
	lv_label_set_long_mode(spbootVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(spbootVer, 155);
	lv_label_set_text(spbootVer, version);
	lv_label_set_align(spbootVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(spbootVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 80, 14 + 24);

	///////////////////
	lv_obj_t *spcoreLab = lv_label_create(bgObj, app);
	lv_label_set_long_mode(spcoreLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(spcoreLab, 80);
	lv_label_set_text(spcoreLab, "SP_CORE:");
	lv_obj_align(spcoreLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24 * 2);

	memset(version, 0, sizeof(version));
	api_sys_getFirmVersion(version, SP_CORE_VER);
	lv_obj_t *spcoreVer = lv_label_create(bgObj, app);
	lv_label_set_long_mode(spcoreVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(spcoreVer, 155);
	lv_label_set_text(spcoreVer, version);
	lv_label_set_align(spcoreVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(spcoreVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 80, 14 + 24 * 2);

	///////////////////
	lv_obj_t *applibLab = lv_label_create(bgObj, app);
	lv_label_set_long_mode(applibLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(applibLab, 80);
	lv_label_set_text(applibLab, "APP_LIB:");
	lv_obj_align(applibLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24 * 3);

	memset(version, 0, sizeof(version));
	api_sys_getFirmVersion(version, LIB_VER);
	lv_obj_t *applibVer = lv_label_create(bgObj, app);
	lv_label_set_long_mode(applibVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(applibVer, 155);
	lv_label_set_text(applibVer, version);
	lv_label_set_align(applibVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(applibVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 80, 14 + 24 * 3);


	///////////////////
	lv_obj_t *osLab = lv_label_create(bgObj, app);
	lv_label_set_long_mode(osLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(osLab, 30);
	lv_label_set_text(osLab, "OS:");
	lv_obj_align(osLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24 * 4);

	memset(version, 0, sizeof(version));
	api_sys_getFirmVersion(version, OS_VER);
	lv_obj_t *osVer = lv_label_create(bgObj, app);
	lv_label_set_long_mode(osVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(osVer, 200);
	lv_label_set_text(osVer, version);
	lv_label_set_align(osVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(osVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 35, 14 + 24 * 4);

	///////////////////
	lv_obj_t *deviceIdLab = lv_label_create(bgObj, app);
	lv_label_set_long_mode(deviceIdLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(deviceIdLab, 80);
	lv_label_set_text(deviceIdLab, "Device Id:");
	lv_obj_align(deviceIdLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24 * 5);

	lv_obj_t *deviceidVer = lv_label_create(bgObj, app);
	lv_label_set_long_mode(deviceidVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(deviceidVer, 155);
	lv_label_set_text(deviceidVer, serialID);
	lv_label_set_align(deviceidVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(deviceidVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 80, 14 + 24 * 5);

	///////////////////
	lv_obj_t *chargeLab = lv_label_create(bgObj, app);
	lv_label_set_long_mode(chargeLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(chargeLab, 120);
	lv_label_set_text(chargeLab, "Charging Status:");
	lv_obj_align(chargeLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24 * 6);

	lv_obj_t *chargeVer = lv_label_create(bgObj, app);
	lv_label_set_long_mode(chargeVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(chargeVer, 105);
	lv_label_set_text_fmt(chargeVer, "%s", api_battery_getChargeState() == 1 ? "Charging" : (api_battery_getChargeState() == 2) ? "Full"
																																: "Not Charging");
	lv_label_set_align(chargeVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(chargeVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 130, 14 + 24 * 6);

	///////////////////

	if (api_battery_getChargeState() != 1)
	{
		lv_obj_t *batteryPercentLab = lv_label_create(bgObj, app);
		lv_label_set_long_mode(batteryPercentLab, LV_LABEL_LONG_BREAK);
		lv_obj_set_width(batteryPercentLab, 175);
		lv_label_set_text(batteryPercentLab, "Battery Percentage:");
		lv_obj_align(batteryPercentLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24 * 7);

		lv_obj_t *batteryPercentVer = lv_label_create(bgObj, app);
		lv_label_set_long_mode(batteryPercentVer, LV_LABEL_LONG_BREAK);
		lv_obj_set_width(batteryPercentVer, 50);
		lv_label_set_text_fmt(batteryPercentVer, "%d%%", api_battery_getPercent());
		lv_label_set_align(batteryPercentVer, LV_LABEL_ALIGN_RIGHT);
		lv_obj_align(batteryPercentVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 185, 14 + 24 * 7);
	}

	api_sys_mutextUnlock(mutex_lvgl);
	app_lcd_brush_screen();
}

void app_lvgl_paramQueryShow()
{
	if (!lcdType)
		return;
	int ret = -1;
	char buf[128] = {0};
	app_lvgl_clearClientBg();
	api_sys_mutextLock(mutex_lvgl);

	lv_obj_t *parent = app_lvgl_clientBg();
	lv_obj_t *titleLab = app_lvgl_createTitle("Parameter");
	lv_obj_t *bgObj = lv_obj_create(parent, NULL);
	lv_obj_set_size(bgObj, 240, 260);
	lv_obj_set_style_local_bg_color(bgObj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_DSP_ASH);
	lv_obj_set_style_local_border_width(bgObj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_color(bgObj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_obj_set_style_local_radius(bgObj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_align(bgObj, parent, LV_ALIGN_IN_TOP_LEFT, 0, STATUSBAR_HEIGT + TITLE_HEIGHT);
	//////////////////////////////////////
	lv_obj_t *signalLab = lv_label_create(bgObj, NULL);
	lv_label_set_long_mode(signalLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(signalLab, 120);
	lv_label_set_text(signalLab, "Signal Strength:");
	lv_obj_set_style_local_text_color(signalLab, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_text_font(signalLab, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_12);
	lv_obj_align(signalLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14);

	memset(buf, 0, sizeof(buf));
	app_wireless_get_csq(buf);
	lv_obj_t *signalVer = lv_label_create(bgObj, signalLab);
	lv_label_set_long_mode(signalVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(signalVer, 115);
	lv_label_set_text(signalVer, buf);
	lv_label_set_align(signalVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(signalVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 120, 14);
	///////////////////////////////////////

	lv_obj_t *audioLab = lv_label_create(bgObj, signalLab);
	lv_label_set_long_mode(audioLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(audioLab, 120);
	lv_label_set_text(audioLab, "Volume Level:");
	lv_obj_align(audioLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "Volume Level: %d", api_audio_getVolume(1));
	lv_obj_t *audioVer = lv_label_create(bgObj, signalLab);
	lv_label_set_long_mode(audioVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(audioVer, 115);
	lv_label_set_text(audioVer, buf);
	lv_label_set_align(audioVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(audioVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 120, 14 + 24);

	///////////////////////////////////////
	lv_obj_t *networkLab = lv_label_create(bgObj, signalLab);
	lv_label_set_long_mode(networkLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(networkLab, 120);
	lv_label_set_text(networkLab, " Network Status:");
	lv_obj_align(networkLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24 * 2);

	memset(buf, 0, sizeof(buf));
	if (api_params_getNetMode() == 1)
	{
		ret = api_wireless_getNetState();
		if (ret == 1)
			sprintf(buf, "Connected");
		else
			sprintf(buf, "Disconnected");
	}
	else
	{
		ret = api_wifi_get_connect_status();
		if (ret == 1)
			sprintf(buf, "Connected");
		else
			sprintf(buf, "Disconnected");
	}
	lv_obj_t *networkVer = lv_label_create(bgObj, signalLab);
	lv_label_set_long_mode(networkVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(networkVer, 115);
	lv_label_set_text(networkVer, buf);
	lv_label_set_align(networkVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(networkVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 120, 14 + 24 * 2);

	///////////////////////////////////////
	lv_obj_t *chargingLab = lv_label_create(bgObj, signalLab);
	lv_label_set_long_mode(chargingLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(chargingLab, 120);
	lv_label_set_text(chargingLab, " Charging Status:");
	lv_obj_align(chargingLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24 * 3);
	memset(buf, 0, sizeof(buf));
	ret = api_battery_getChargeState();
	if (ret == 0)
		sprintf(buf, "Not charging");
	else if (ret == 1)
		sprintf(buf, "Charging");
	else
		sprintf(buf, "Full battery");
	lv_obj_t *chargingVer = lv_label_create(bgObj, signalLab);
	lv_label_set_long_mode(chargingVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(chargingVer, 115);
	lv_label_set_text(chargingVer, buf);
	lv_label_set_align(chargingVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(chargingVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 120, 14 + 24 * 3);

	api_sys_mutextUnlock(mutex_lvgl);
}

void app_lvgl_mainShow(char *imgSrc, int imgwidth, int imgheight, char *content, lv_color_t textColor)
{
	api_sys_mutextLock(mutex_lvgl);
	_lvgl_clearAllClientObj();
	lv_obj_t *parent = app_lvgl_clientBg();
	app_lvgl_iconShow(parent, imgSrc, LV_ALIGN_IN_TOP_MID, 0, 86);
	if (strlen(content))
	{
		lv_obj_t *lab = lv_label_create(parent, NULL);
		lv_label_set_long_mode(lab, LV_LABEL_LONG_BREAK);
		lv_label_set_align(lab, LV_LABEL_ALIGN_CENTER);
		lv_obj_set_size(lab, 240, 32);
		lv_label_set_text(lab, content);
		lv_obj_set_style_local_text_color(lab, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, textColor);
		lv_obj_set_style_local_text_font(lab, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_16);
		lv_obj_align(lab, parent, LV_ALIGN_IN_TOP_MID, 0, 216);
	}
	api_sys_mutextUnlock(mutex_lvgl);
	app_lcd_brush_screen();
}

void app_lvgl_downloadShow(char *imgSrc, int imgwidth, int imgheight, char *content, lv_color_t textColor)
{
	api_sys_mutextLock(mutex_lvgl);
	_lvgl_clearAllClientObj();
	lv_obj_t *parent = app_lvgl_clientBg();
	app_lvgl_iconShow(parent, imgSrc, LV_ALIGN_IN_TOP_MID, 0, 86);
	if (strlen(content))
	{
		lv_obj_t *lab = lv_label_create(parent, NULL);
		lv_label_set_long_mode(lab, LV_LABEL_LONG_BREAK);
		lv_label_set_align(lab, LV_LABEL_ALIGN_CENTER);
		lv_obj_set_size(lab, 240, 32);
		lv_label_set_text(lab, content);
		lv_obj_set_style_local_text_color(lab, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, textColor);
		lv_obj_set_style_local_text_font(lab, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_16);
		lv_obj_align(lab, parent, LV_ALIGN_IN_TOP_MID, 0, 216);
	}
	api_sys_mutextUnlock(mutex_lvgl);
	app_lcd_brush_screen();
}

void app_transaction_recordShow(dsp_tradeInfo_t *tradeInfo, int size)
{
	if (!lcdType)
		return;
	app_lvgl_clearClientBg();
	dsp_tradeInfo_t *ptradeInfo = tradeInfo;
	char msgBuf[256] = {0};
	int i = 0;
	int ret = -1;
	int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0;
	char buf[128] = {0};
	api_sys_mutextLock(mutex_lvgl);

	lv_obj_t *parent = app_lvgl_clientBg();
	lv_obj_t *titleLab = app_lvgl_createTitle("Transaction");
	lv_obj_t *bgObj = lv_obj_create(parent, NULL);
	lv_obj_set_size(bgObj, 240, 260);
	lv_obj_set_style_local_bg_color(bgObj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_DSP_ASH);
	lv_obj_set_style_local_border_width(bgObj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_color(bgObj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_obj_set_style_local_radius(bgObj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_align(bgObj, parent, LV_ALIGN_IN_TOP_LEFT, 0, STATUSBAR_HEIGT + TITLE_HEIGHT);
	//////////////////////////////////////

	lv_obj_t *tarceNoLab = lv_label_create(bgObj, NULL);
	lv_label_set_long_mode(tarceNoLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(tarceNoLab, 120);
	lv_label_set_text(tarceNoLab, "TraceNo:");
	lv_obj_set_style_local_text_color(tarceNoLab, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_text_font(tarceNoLab, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_12);
	lv_obj_align(tarceNoLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14);

	lv_obj_t *tarceNoVer = lv_label_create(bgObj, tarceNoLab);
	lv_label_set_long_mode(tarceNoVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(tarceNoVer, 115);
	lv_label_set_text(tarceNoVer, ptradeInfo->traceNo);
	lv_label_set_align(tarceNoVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(tarceNoVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 120, 14);
	///////////////////////////////////////
	lv_obj_t *batchNoLab = lv_label_create(bgObj, tarceNoLab);
	lv_label_set_long_mode(batchNoLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(batchNoLab, 120);
	lv_label_set_text(batchNoLab, "BatchNo:");
	lv_obj_align(batchNoLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24);
	lv_obj_t *batchNoVer = lv_label_create(bgObj, tarceNoLab);
	lv_label_set_long_mode(batchNoVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(batchNoVer, 115);
	lv_label_set_text(batchNoVer, ptradeInfo->batchNo);
	lv_label_set_align(batchNoVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(batchNoVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 120, 14 + 24);
	///////////////////////////////////////
	lv_obj_t *transStatusLab = lv_label_create(bgObj, tarceNoLab);
	lv_label_set_long_mode(transStatusLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(transStatusLab, 120);
	lv_label_set_text(transStatusLab, "TransStatus:");
	lv_obj_align(transStatusLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24 * 2);

	lv_obj_t *transStatusVer = lv_label_create(bgObj, tarceNoLab);
	lv_label_set_long_mode(transStatusVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(transStatusVer, 115);
	lv_label_set_text(transStatusVer, ptradeInfo->transStatus);
	lv_label_set_align(transStatusVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(transStatusVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 120, 14 + 24 * 2);

	///////////////////////////////////////
	lv_obj_t *transTypeldLab = lv_label_create(bgObj, tarceNoLab);
	lv_label_set_long_mode(transTypeldLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(transTypeldLab, 120);
	lv_label_set_text(transTypeldLab, "TransType:");
	lv_obj_align(transTypeldLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24 * 3);

	lv_obj_t *transTypeldVer = lv_label_create(bgObj, tarceNoLab);
	lv_label_set_long_mode(transTypeldVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(transTypeldVer, 115);
	lv_label_set_text(transTypeldVer, ptradeInfo->transTypeId);
	lv_label_set_align(transTypeldVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(transTypeldVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 120, 14 + 24 * 3);

	///////////////////////////////////////
	lv_obj_t *outTradeldLab = lv_label_create(bgObj, tarceNoLab);
	lv_label_set_long_mode(outTradeldLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(outTradeldLab, 120);
	lv_label_set_text(outTradeldLab, "OutTradeld:");
	lv_obj_align(outTradeldLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24 * 4);
	lv_obj_t *outTradeldVer = lv_label_create(bgObj, tarceNoLab);
	lv_label_set_long_mode(outTradeldVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(outTradeldVer, 115);
	lv_label_set_text(outTradeldVer, ptradeInfo->origOutTradeId);
	lv_label_set_align(outTradeldVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(outTradeldVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 120, 14 + 24 * 4);
	///////////////////////////////////////
	lv_obj_t *transDatetimeLab = lv_label_create(bgObj, tarceNoLab);
	lv_label_set_long_mode(transDatetimeLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(transDatetimeLab, 100);
	lv_label_set_text(transDatetimeLab, "TransDatetime:");
	lv_obj_align(transDatetimeLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24 * 6);
	lv_obj_t *transDatetimeVer = lv_label_create(bgObj, tarceNoLab);
	lv_label_set_long_mode(transDatetimeVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(transDatetimeVer, 130);
	ret = sscanf(ptradeInfo->transDatetime, "%04d-%02d-%02d %02d:%02d:%02d", &year, &month, &day, &hour, &min, &sec);
	if (ret == 6)
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%04d/%02d/%02d %02d:%02d", year, month, day, hour, min);
	}
	lv_label_set_text(transDatetimeVer, buf);
	lv_label_set_align(transDatetimeVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(transDatetimeVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 105, 14 + 24 * 6);

	///////////////////////////////////////
	lv_obj_t *transMoneyLab = lv_label_create(bgObj, tarceNoLab);
	lv_label_set_long_mode(transMoneyLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(transMoneyLab, 120);
	lv_label_set_text(transMoneyLab, "TransMoney:");
	lv_obj_align(transMoneyLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24 * 7);

	lv_obj_t *transMoneyVer = lv_label_create(bgObj, tarceNoLab);
	lv_label_set_long_mode(transMoneyVer, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(transMoneyVer, 115);
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d.%02d", atoi(ptradeInfo->money) / 100, atoi(ptradeInfo->money) % 100);
	lv_label_set_text(transMoneyVer, buf);
	lv_label_set_align(transMoneyVer, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(transMoneyVer, bgObj, LV_ALIGN_IN_TOP_LEFT, 120, 14 + 24 * 7);

	api_sys_mutextUnlock(mutex_lvgl);
}

void app_lvgl_msgTips(char *msgBuf)
{
	if (!lcdType)
		return;
	api_sys_mutextLock(mutex_lvgl);
	lv_obj_t *parent = app_lvgl_clientBg();
	lv_obj_t *label = lv_label_create(parent, NULL);
	lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
	lv_obj_set_size(label, lv_obj_get_width(parent), lv_obj_get_height(parent) - STATUSBAR_HEIGT);
	lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
	lv_label_set_text(label, msgBuf);
	lv_obj_set_style_local_text_color(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_20);
	lv_obj_align(label, parent, LV_ALIGN_CENTER, 0, 0);
	api_sys_mutextUnlock(mutex_lvgl);
}

void app_lvgl_merchantInfoShow(
	char *dataKey, char *pinKey,
	char *merchantId, char *tid,
	char *merchantName)
{

	if (!lcdType)
		return;
	app_lvgl_clearClientBg();
	api_sys_mutextLock(mutex_lvgl);
	lv_obj_t *parent = app_lvgl_clientBg();
	lv_obj_t *titleLab = app_lvgl_createTitle("Information");
	lv_obj_t *bgObj = lv_obj_create(parent, NULL);
	lv_obj_set_size(bgObj, 240, 260);
	lv_obj_set_style_local_bg_color(bgObj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_DSP_ASH);
	lv_obj_set_style_local_border_width(bgObj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_border_color(bgObj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_obj_set_style_local_radius(bgObj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_align(bgObj, parent, LV_ALIGN_IN_TOP_LEFT, 0, STATUSBAR_HEIGT + TITLE_HEIGHT);
	//////////////////////////////////////
	lv_obj_t *merchantIdLab = lv_label_create(bgObj, NULL);
	lv_label_set_long_mode(merchantIdLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(merchantIdLab, 80);
	lv_label_set_text(merchantIdLab, "merchantId:");
	lv_obj_set_style_local_text_color(merchantIdLab, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_text_font(merchantIdLab, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_12);
	lv_obj_align(merchantIdLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14);

	lv_obj_t *merchantIdVerLab = lv_label_create(bgObj, merchantIdLab);
	lv_label_set_long_mode(merchantIdVerLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(merchantIdVerLab, 155);
	lv_label_set_text(merchantIdVerLab, merchantId);
	lv_label_set_align(merchantIdVerLab, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(merchantIdVerLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 80, 14);
	///////////////////////////////////////

	lv_obj_t *tidLab = lv_label_create(bgObj, merchantIdLab);
	lv_label_set_long_mode(tidLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(tidLab, 80);
	lv_label_set_text(tidLab, "tid:");
	lv_obj_align(tidLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24);

	lv_obj_t *tidVerLab = lv_label_create(bgObj, merchantIdLab);
	lv_label_set_long_mode(tidVerLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(tidVerLab, 155);
	lv_label_set_text(tidVerLab, tid);
	lv_label_set_align(tidVerLab, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(tidVerLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 80, 14 + 24);

	///////////////////////////////////////
	lv_obj_t *merchantNameLab = lv_label_create(bgObj, merchantIdLab);
	lv_label_set_long_mode(merchantNameLab, LV_LABEL_LONG_EXPAND);
	lv_obj_set_width(merchantNameLab, 80);
	lv_label_set_text(merchantNameLab, "merchantName:");
	lv_obj_align(merchantNameLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 5, 14 + 24 * 2);

	lv_obj_t *merchantNameVerLab = lv_label_create(bgObj, merchantIdLab);
	lv_label_set_long_mode(merchantNameVerLab, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(merchantNameVerLab, 155);
	lv_label_set_text(merchantNameVerLab, merchantName);
	lv_label_set_align(merchantNameVerLab, LV_LABEL_ALIGN_RIGHT);
	lv_obj_align(merchantNameVerLab, bgObj, LV_ALIGN_IN_TOP_LEFT, 80, 14 + 24 * 2);

	api_sys_mutextUnlock(mutex_lvgl);
	app_lcd_brush_screen();
}

void app_lvgl_reboot()
{
	app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_reboot.wav", "reboot", 1);
	app_lvgl_mainShow(DSP_RESTART_IMG, 100, 100, "Reboot", LV_COLOR_DSP_RED);
	api_sys_msleep(2000);
	api_sys_reboot();
}

void app_lvgl_poweroff()
{
	app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_poweroff.wav", "power off", 1);
	app_lvgl_mainShow(DSP_POWEROFF_IMG, 100, 100, "Power off", LV_COLOR_DSP_RED);
	api_sys_msleep(2000);
	api_sys_powerOff();
}

void app_lvgl_reset()
{
	app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_restart.wav", "restore factory settings", 1);
	app_lvgl_mainShow(DSP_RESTART_IMG, 100, 100, "Restore factory settings", LV_COLOR_DSP_RED);
	api_sys_msleep(2000);
	api_sys_reboot();
}

void app_lvgl_disableDevice()
{
	device_Status = false;
	app_dsp_play(APP_AUDIO_PLAY, "/ext/audio/english/dsp_deactivate.wav", "deactivate", 1);
	app_lvgl_mainShow(DSP_FAIL_IMG, 100, 100, "Deactivate", LV_COLOR_DSP_RED);
	api_sys_msleep(2000);
}

void app_lvgl_enableDevice()
{
	device_Status = true;
	current_mode = MODE_NORMAL;
	app_static_qr(NULL);
}
