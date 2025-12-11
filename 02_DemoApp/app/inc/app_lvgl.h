#ifndef __APP_LVGL_H__
#define __APP_LVGL_H__
#include "lvgl.h"
#include "app_pub.h"

LV_FONT_DECLARE(lv_font_simsun_14)

#define VISIBLE_ROWS 7
#define ITEM_HEIGHT 32 
#define ITEM_SPACING 6  
#define MENUTITLE_HEIGHT 30  
#define STATUSBAR_HEIGT 30  


#define MENU_ITEM_WIDTH  223
#define MENU_ITEM_HEIGHT 35
#define MENU_ITEM_SPACE 12

#define LIST_ITEM_SHOW_MAX_CNT 6
#define LIST_ITEM_WIDTH  223
#define LIST_ITEM_HEIGHT 20
#define LIST_ITEM_SPACE 5

#define TITLE_HEIGHT 30

lv_obj_t *row_conts[10];  
int scroll_offset ; 


#define MAX_ICON_OBJS 100
typedef struct {
    lv_obj_t* img_obj;
    char img_path[128];
} IconInfo;
IconInfo g_icons[MAX_ICON_OBJS];

typedef struct {
    char text[128];           // Holds the text content to be displayed (max 128 characters)
    int font_size;            // Specifies the size of the font for the text
    int total_lines;          // Total number of lines available on the screen for display
    int target_line;          // Line number where the text should be displayed
    int bg_start_line;        // Start line number for applying the background color
    int bg_end_line;          // End line number for applying the background color
    lv_align_t align;         // Alignment of the text within its line (e.g., left, right, center)
    int x_offset;             // Horizontal offset for the text position within the line
    int y_offset;             // Vertical offset for the text position within the line
    lv_color_t text_color;    // Color of the text
    lv_color_t bg_color;      // Background color for the area between bg_start_line and bg_end_line
    lv_color_t line_bg_color; // Background color specifically for the target_line
    int frameflag;            //Do you want a border  0-No 1-Yes
    int functype;              
} display_message_t; 


#define LV_COLOR_LIGHT_GREEN  LV_COLOR_MAKE(0x90, 0xEE, 0x90)  
#define LV_COLOR_PALE_GREEN   LV_COLOR_MAKE(0x98, 0xFB, 0x98)  
#define LV_COLOR_SPRING_GREEN LV_COLOR_MAKE(0x00, 0xFF, 0x7F)  
#define LV_COLOR_SEA_GREEN    LV_COLOR_MAKE(0x2E, 0x8B, 0x57)  
#define LV_COLOR_FOREST_GREEN LV_COLOR_MAKE(0x22, 0x8B, 0x22) 
#define LV_COLOR_MINT_GREEN   LV_COLOR_MAKE(0x98, 0xFF, 0x98)  
#define LV_COLOR_HONEYDEW     LV_COLOR_MAKE(0xF0, 0xFF, 0xF0)  
#define LV_COLOR_DARK_GREEN   LV_COLOR_MAKE(0x00, 0x64, 0x00)  
#define LV_COLOR_CHARTREUSE   LV_COLOR_MAKE(0x7F, 0xFF, 0x00) 
#define LV_COLOR_LAWN_GREEN   LV_COLOR_MAKE(0x7C, 0xFC, 0x00)  
#define LV_COLOR_MEDIUM_SEA_GREEN LV_COLOR_MAKE(0x3C, 0xB3, 0x71) 
#define LV_COLOR_MEDIUM_SPRING_GREEN LV_COLOR_MAKE(0x00, 0xFA, 0x9A) 

#define LV_COLOR_DSP_LIGHT_BLUE   LV_COLOR_MAKE(0xAD, 0xD8, 0xE6)
#define LV_COLOR_DSP_SKY_BLUE   LV_COLOR_MAKE(0x87, 0xCE, 0xEB)
#define LV_COLOR_DSP_ASH   LV_COLOR_MAKE(0xF3, 0xF3, 0xF3)
#define LV_COLOR_DSP_ORANGE   LV_COLOR_MAKE(0xFF,0x8E,0x34)
#define LV_COLOR_DSP_GREEN   LV_COLOR_MAKE(0x63,0xC8,0x74)
#define LV_COLOR_DSP_BLUE   LV_COLOR_MAKE(0x0F,0x31,0x7F)
#define LV_COLOR_DSP_RED   LV_COLOR_MAKE(0xFF,0x69,0x6A)

#define BLUEBLANK_IMG "/ext/ui/blue_blank.png"
#define DECLINESYMBOL_IMG "/ext/ui/decline_symbol.png"
#define APPROVESYMBOL_IMG "/ext/ui/approve_symbol.png"
#define PENDINGSYMBOL_IMG "/ext/ui/pending_symbol.png"
#define DSP_WELCOME_IMG "/ext/ui/dsp_welcome.png"
#define DSP_CONNECTING_IMG "/ext/ui/dsp_connecting.png"
#define DSP_CONNECTED_IMG "/ext/ui/dsp_connected.png"
#define DSP_DISCONNECTED_IMG "/ext/ui/dsp_disconnected.png"
#define POWEROFF_IMG "/ext/ui/power_off.png"
#define DSP_SYSINIT_IMG "/ext/ui/dsp_sysinit.png"
#define DSP_DOWNLOADING_IMG "/ext/ui/dsp_downloading.png"
#define DSP_UPDATESUCC_IMG "/ext/ui/dsp_updatesucc.png"
#define DSP_RESTART_IMG "/ext/ui/dsp_restart.png"
#define DSP_POWEROFF_IMG "/ext/ui/dsp_poweroff.png"
#define DSP_AUTHORIZATION_IMG "/ext/ui/dsp_authorization.png"
#define DSP_UPDATEFILE_IMG "/ext/ui/dsp_updatefile.png"
#define DSP_NETWORKSUCC_IMG "/ext/ui/dsp_networksucc.png"
#define DSP_NETWORKFAIL_IMG "/ext/ui/dsp_networkfail.png"
#define DSP_DOWNCOMPLETE_IMG "/ext/ui/dsp_downcomplete.png"
#define DSP_FAIL_IMG "/ext/ui/dsp_fail.png"
#define DSP_SUCC_IMG "/ext/ui/dsp_succ.png"
#define DSP_WAITTING_IMG "/ext/ui/dsp_waitting.png"
#define DSP_READY_IMG "/ext/ui/dsp_ready.png"
#define DSP_DECLINESYMBOL6060_IMG "/ext/ui/dsp_fail_60_60.png"
#define DSP_APPROVESYMBOL6060_IMG "/ext/ui/dsp_succ_60_60.png"
#define DSP_PENDINGSYMBOL6060_IMG "/ext/ui/dsp_pending_60_60.png"

void app_lcd_brush_screen();
void app_static_qrcode_show(char* qrurl, char* merchantName);
void app_show_iconStatus(int type, int value);
lv_obj_t* app_lvgl_clientBg();
void app_transaction_resultShow(int tradetype, int result, char* amount, char* transactionId, char* orderID);
void app_lvgl_inputShow(char* title, char* text, int inputMode, int plainMode, char dispChar,bool showtips);
void app_lvgl_menuShow(char* title, char* menuItems[], int itemSize);
void app_lvgl_paramQueryShow();
void app_lvgl_msgTips(char* msgBuf);
void app_lvgl_mainShow(char* imgSrc, int imgwidth, int imgheight, char* content, lv_color_t textColor);
void app_lvgl_reboot();
void app_lvgl_poweroff();
void app_lvgl_reset();
void app_lvgl_disableDevice();
void app_lvgl_enableDevice();

#define SOC_IMG_WIDTH 23
#define SOC_IMG_HEIGT 14

#define WIRE_IMG_WIDTH 16
#define WIRE_IMG_HEIGHT 14

#define WIFI_IMG_WIDTH 18
#define WIFI_IMG_HEIGHT 14
#endif

