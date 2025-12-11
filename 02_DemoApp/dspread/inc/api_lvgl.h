#ifndef _API_LVGL_H
#define _API_LVGL_H

#define USE_LVGL 1
#if(USE_LVGL)
#include "lvgl.h"
//#include "lv_lib_qrcode/lv_qrcode.h"


enum {
	LVGL_FONT_ARIAL_SIZE_10=0,
	LVGL_FONT_ARIAL_SIZE_12,
	LVGL_FONT_ARIAL_SIZE_14,
	LVGL_FONT_ARIAL_SIZE_24,
	LVGL_FONT_ARIAL_SIZE_28,
	LVGL_FONT_MAX,
};


#define LVGL_LCD_TEXT_ROW_LEFT (0 << 0) 
#define LVGL_LCD_TEXT_ROW_RIGHT (1 << 0) 
#define LVGL_LCD_TEXT_ROW_CENTER (2 << 0) 
#define LVGL_LCD_TEXT_ROW_INVERSE (1 << 2)  

#define FDISP 		0x00	 
#define NOFDISP     LVGL_LCD_TEXT_ROW_INVERSE	 
#define LDISP 		LVGL_LCD_TEXT_ROW_LEFT	    
#define CDISP 		LVGL_LCD_TEXT_ROW_CENTER	   
#define RDISP 		LVGL_LCD_TEXT_ROW_RIGHT	    

int OsFontSetFontType(int type);
int OsFontShowRowText(
	unsigned int nRow, unsigned int nCol,
	const unsigned char* lpText, unsigned int flag,
	lv_color_t text_color, lv_color_t bg_color,
	int opa);
int OsFontClearRowText(unsigned int nRow);
int OsLvglShowQrCode(lv_obj_t** qr,char* qrdata, unsigned int size, int offset_x, int offset_y);
int OsLvglQrCodeUpdate(lv_obj_t* qr, char* qrdata);
int OsFontClearScreen(void);





int OsLcdShowPng(
	int startX, int startY,
	int width, int height,
	char* imgpath, lv_obj_t** img);

#endif
#endif