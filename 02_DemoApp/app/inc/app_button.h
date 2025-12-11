#ifndef __APP_BUTTON_H__
#define __APP_BUTTON_H__

#define MAX_LENGTH 20

enum InputMode { MODE_LOWERCASE, MODE_UPPERCASE, MODE_DIGIT, MODE_SYMBOL };

typedef enum {
	MODE_ANY = 0,
    MODE_NORMAL,
    MODE_PLUS,
    MODE_FUNC,
	MODE_MENU,
	MODE_LANGUAGE_SET,
	MODE_TRANS_INFO,
	MODE_VERSION_INFO,
	MODE_NETWORK_SET,
	MODE_WIFI_SETTING,
	MODE_NETWORK_WIFI_CONFIG,
	MODE_NETWORK_WIFI_INPUTPWD,
	MODE_NETWORK_WIRELESS_TIP,
	MODE_PARAM_QUERY,
	MODE_SYSTEM_SETTING,
	MODE_TRANS_TIPS,
	MODE_UPDATE_VERSION,
	MODE_PASSWORD_SETTING,
	MODE_SYS_INFORMATION,
	MODE_TMS_SHOW,
	MODE_TMS_UPDATE_VERSION,
	MODE_SYS_PARAMS_SIGNIN_TIP,
	MODE_SYS_PARAMS_SET_TIME,
	MODE_ISO8583,
	MODE_SYS_PARAMS_SYNC_TMS_TASKS,
	MODE_ENVIRONMENT,
	MODE_LOGSWITCH_SET,
	MODE_CARDPAY_SHOW,
}AppMode;
extern AppMode current_mode;

static void handle_power(void);
static void handle_volume_inc(void);
static void handle_volume_dec(void);
static void handle_digit(u32 button);
void handle_menu(u32 button);
static void handle_clear(void);
static void handle_cancel(void);
static void handle_enter(void);
static void handle_plus(void);
static void handle_func(void);
static void handle_pgup(void);
static void handle_pgdown(void);
static void handle_language(void);
static void handle_detail_language(void);
static void handle_trans_query(void);
static void handle_version_query(void);
static void handle_network_setting(void);
static void handle_system_param(void);
static void handle_ios_8583(void);
static void handle_ios_8583_trans(u32 button);
static void handle_log_switch(void);
static void handle_log_switch_set(u32 button);
static void handle_sim_setting(void);
static void handle_wifi_device_setting(void);
static void handle_wifi_phone_setting(void);
static void handle_wifi_firmware_update(void);
static void handle_wifi_camera_setting(void);
static void handle_infomation(void);
static void handle_param_settings(void);
static void handle_sign_in(void);
static void handle_sync_time(void);
static void handle_environment(void);
void app_mon_button(void *param); 
int app_button_semWait();
int app_button_semSig();
void app_handle_cardpay(u32 button);
void app_cardpay_show(u32 button);

typedef struct {
    u32 key;
    AppMode mode;
    void (*handler)(u32);
} KeyHandler;



void handle_menu(u32 button);

#endif

