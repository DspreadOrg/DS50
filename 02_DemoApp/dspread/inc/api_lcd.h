#ifndef __API_LCD_H
#define __API_LCD_H


#include "api_types.h"



#define FONT_SONG                   0
#define FONT_SONG_BOLD              1
#define FONT_YOUYUAN                2
#define FONT_YOUYUAN_BOLD           3
#define ASCII_6X12                  0	 
#define ASCII_8X16                  1	 
#define ASCII_12X24                 2	 
#define ASCII_16X32                 3	 
#define ASCII_6X8                   4	 
#define ASCII_16X24                 5 
#define ASCII_16X48                 6 
#define ASCII_20X32                 7 
#define ASCII_28X48                 8	 
#define ASCII_24X48                 9	 
#define ASCII_16X16                 10	 
#define ASCII_5X7                   11	 

#define NATIVE_FONT_12X12           0 
#define NATIVE_FONT_16X16           1	 
#define NATIVE_FONT_20X20           2
#define NATIVE_FONT_24X24           3	 
#define NATIVE_FONT_32X32           4
#define NATIVE_FONT_48X48           5
#define NATIVE_FONT_6X12            6
#define NATIVE_FONT_8X16            7
#define NATIVE_FONT_10X20           8
#define NATIVE_FONT_12X24           9
#define NATIVE_FONT_16X32           10
#define NATIVE_FONT_24X48           11

 
#define api_LCD_CTL_SET_BACKGROUND  0           
#define api_LCD_CTL_SET_COLOR       1          
#define api_LCD_CTL_SET_FONT        2          
#define api_LCD_CTL_SET_LINEWIDTH   3          
#define api_LCD_CTL_RESOLUTION      4          
#define api_LCD_CTL_INVERT_RECT     5        
#define api_LCD_CTL_BRIGHT          6         
#define api_LCD_CTL_ICON            7          
#define api_LCD_CTL_GET_DOTCOLOR    8           
#define api_LCD_CTL_BKLIGHT         9           
#define api_LCD_CTL_BKLIGHTTIME     10         
#define api_LCD_CTL_GET_COLOR       11         
#define api_LCD_CTL_BKLIGHT_CTRL    12          
#define api_LCD_CTL_GET_BRIGHT      13           
#define api_LCD_CTL_GET_FONT        14          
#define api_LCD_CTL_GET_BPP         15           
#define api_LCD_CTL_GET_BGCOLOR     16         
#define api_LCD_CTL_PHY_RESOLUTION  17         
#define api_LCD_CTL_SVER 			18          
#define api_LCD_CTL_CLEAR_LCD_DDRAM	19				 
#define api_LCD_CTL_LEVEL_SCALE		20		     
#define api_LCD_CTL_BRUSH_INTERVAL	21		   
#define api_LCD_CTL_CLEAR_ICON		22			 
#define api_LCD_CTL_DISP_LOGO			23			 

#define BACKGOURND_MODE_COLOR           0x00
#define BACKGOURND_MODE_PICTURE         0x01

#define MAX_BRIGHTNESS                  5
#define DEFAULT_BRIGHTNESS              2

#define RED_COLOR                   0xF800
#define GREEN_COLOR                 0x07E0
#define BLUE_COLOR                  0x001F
#define BLACK_COLOR                 0x0000
#define WHITE_COLOR                 0xFFFF


#define LINE1 0
#define LINE2 1
#define LINE3 2
#define LINE4 3
#define LINE5 4
#define LINE6 5
#define LINE7 6
#define LINE8 7
#define LINE9 8
#define LINE10 9

 
#define api_LCD_TEXT_ROW_LEFT (0 << 0) 
#define api_LCD_TEXT_ROW_RIGHT (1 << 0)  
#define api_LCD_TEXT_ROW_CENTER (2 << 0)  
#define api_LCD_TEXT_ROW_INVERSE (1 << 2)  

#define FDISP 		0x00	 
#define NOFDISP     api_LCD_TEXT_ROW_INVERSE	 
#define LDISP 		api_LCD_TEXT_ROW_LEFT	    
#define CDISP 		api_LCD_TEXT_ROW_CENTER	    
#define RDISP 		api_LCD_TEXT_ROW_RIGHT	    
#define INCOL		0x08	 
#define CTRLA 		0x80	 
#define NOFDISPLINE NOFDISP

#define MAXROW 5


 
typedef enum _eColorFmt
{
    COLOR_aRGB888 = 0,       
    COLOR_RGB565,
    COLOR_COLOR_MONO,
} emColorFmt;

 
typedef struct _strDot
{
    u32 m_x;     
    u32 m_y;    
} strDot;



 
typedef struct _strRect
{
    u32  m_x0;   
    u32  m_y0;  
    u32  m_x1;  
    u32  m_y1;   
} strRect;

 
typedef struct _strPicture
{
    u32  m_width;   
    u32  m_height;   
    u32* m_pic;   
    emColorFmt m_colorfmt;   
} strPicture;

 
typedef struct _strLine
{
    u32  m_x0; 
    u32  m_y0; 
    u32  m_x1;  
    u32  m_y1;  
} strLine;


typedef struct _calioffset
{
    u32 offset_x;
    u32 offset_y;
}calioffset;


typedef struct _strFont
{
	u32 m_font;       
	u32 m_ascsize;    
	u32 m_nativesize;  
} strFont;



 
typedef struct _strBackGround
{
    u32 m_mode;         
    strRect m_rect;     
    u32 m_purecolor;  
    strPicture m_pic;  
} strBackGround;

 
typedef struct _strInvertColor
{
    u32 m_bgcolor;
    u32 m_fgcolor;
} strInvertColor;





int api_lcd_blOn(void);
int api_lcd_blOff(void);
int api_lcd_clrScreen(void);
s32 api_lcd_open(void);
s32 api_lcd_close(void);
s32 api_lcd_fillRect (const strRect *lpstrRect, u32 nRGB);
s32 api_lcd_clearRect (const strRect *lpstrRect);
s32 api_lcd_showText(u32 nX, u32 nY, const u8* lpText);
s32 api_lcd_showPicture(const strRect *lpstrRect,const strPicture * lpstrPic);
s32 api_lcd_showPixelColor(u32 nX, u32 nY, u32 nRGB);
s32 api_lcd_showCircle(u32 nX, u32 nY, u32 radius, u32 nRGB);
s32 api_lcd_showLine(const strLine* lpstrLine );
s32 api_lcd_showRect(const strRect* lpstrRect);
void api_lcd_brushRect(const strRect *lpstrRect);
void api_lcd_brushScreen(void);
s32 api_lcd_sleepmode(S8 mode);
s32 api_lcd_clearScreen(void);
s32 api_lcd_fillRow(u32 nRow, u32 nCol, const S8* lpText, u32 flag);
s32 api_lcd_fillRowFmt(u32 nRow, u32 nCol, u32 flag, const S8* fmt, ...);
s32 api_lcd_clearRow(u32 nRow);
void api_lcd_showRfPic(const unsigned char* picc_tips,
    const unsigned short PICC_TIPS_WIDTH, const unsigned short PICC_TIPS_HEIGHT);

#ifdef TRUETYPE
s32 api_lcd_truetype_show(char* text, int pixsize, int x, int y);
#endif


 
void api_lcd_setFontSize(int ascsize);


 
s32 api_lcd_fillRowBgColor(u32 nRow, u32 nCol, const S8* lpText, u32 flag, int bgcolor);


S8 api_lcd_blacklightControl(u8 value);


#endif

