



#include "lvgl.h"
#include <stdio.h>
#include "lvgtracedef.h"
#include "ddi_log.h"
#include "dev_lcd.h"
//#include "src/lv_lib_png/lv_png.h"
unsigned short* g_p_dispmem;

static lv_group_t* group = 0;
static int gui_init_flag = 0;
static int group_init_flag = 0;

static int ts_left = 0;
static int ts_right = 0;
static int ts_top = 0;
static int ts_bottom = 0;
static int ts_sx = 0;
static int ts_sy = 0;
static int ts_lx = 0;
static int ts_ly = 0;
static int ts_tx = 0;
static int ts_ty = 0;

static int tick_total = 0;
static int tick_count = 0;
static int m_lvg_init = 0;
static int m_ts_read_flag = 0;

static char refresh_task_count = 0;

void lv_start_lock(int mode);

static void mf_lv_flush_cb(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p)
{
    unsigned short* disp_buff = g_p_dispmem;
	/* osl_lock_lcd();
	 auxlcd_flush(g_p_dispmem, LV_HOR_RES_MAX * LV_VER_RES_MAX * 2);
	 osl_unlock_lcd();*/
    DDI_LOG_DEBUG("lvgl brush\r\n");
    lv_start_lock(1);
    dev_lcd_brush_screen();
    lv_start_lock(0);

    lv_disp_flush_ready(disp_drv);
    return;
}








static void mf_lv_display_driver_rounder_callback(lv_disp_drv_t* disp_drv, lv_area_t* area)
{
    area->x1 = 0;
    area->x2 = disp_drv->hor_res - 1;
    area->y1 = 0;
    area->y2 = disp_drv->ver_res - 1;
}


static unsigned char m_lvgl_flag = 0;
unsigned char lv_get_drv_flag() {
    return m_lvgl_flag;
}

int lv_drv_init(void)
{
    int hor_res = LV_HOR_RES_MAX;
    int ver_res = LV_VER_RES_MAX;

   
    lv_init();
    //lv_png_init();//png½âÂë
    //lv_port_fs_init();
    g_p_dispmem = LCD_Getframe_buffer();
   

    static lv_disp_buf_t disp_buf = { 0 };
    lv_disp_buf_init(
        &disp_buf,
        g_p_dispmem,
        NULL,
        hor_res * ver_res);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = hor_res;
    disp_drv.ver_res = ver_res;
    disp_drv.flush_cb = mf_lv_flush_cb;
    disp_drv.buffer = &disp_buf;
    disp_drv.rounder_cb = mf_lv_display_driver_rounder_callback;
    lv_disp_t* g_p_disp = lv_disp_drv_register(&disp_drv);
    if (g_p_disp == NULL)
    {
        LV_LOG_WARN("DBG====Failed to register disp drv!\n");
        return -1;
    }


#if 0
    osl_ts_get(&ts_left, &ts_right, &ts_top, &ts_bottom);
#else
    ts_left = 0;
    ts_right = LV_HOR_RES_MAX;
    ts_top = 0;
    ts_bottom = LV_VER_RES_MAX;
#endif

    ts_sx = ts_right - ts_left;
    ts_sy = ts_bottom - ts_top;

    DDI_LOG_DEBUG("ts:%d,%d-%d,%d=%d,%d\r\n", ts_left, ts_right, ts_top, ts_bottom, ts_sx, ts_sy);

#if 0
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;

    lv_indev_t* dev_touch = lv_indev_drv_register(&indev_drv);
    if (dev_touch == NULL)
    {
        LV_LOG_WARN("Failed to register LV_INDEV_TYPE_POINTER!\n");
        return -2;
    }


    lv_indev_drv_t kb_drv;
    lv_indev_drv_init(&kb_drv);
    kb_drv.type = LV_INDEV_TYPE_KEYPAD;
    kb_drv.read_cb = mf_lv_keyboard_driver_read_callback;
    lv_indev_t* dev_kb = lv_indev_drv_register(&kb_drv);
    if (dev_kb == NULL)
    {
        LV_LOG_WARN("Failed to register LV_INDEV_TYPE_KEYPAD!\n");
        return -3;
    }
#endif
    m_lvgl_flag = 1;
    return 0;
}


void lvgl_group_create()
{
    if (group_init_flag == 1)  return;
    group_init_flag = 1;

    group = lv_group_create();
    lv_indev_t* cur_drv = NULL;
    for (;;) {
        cur_drv = lv_indev_get_next(cur_drv);
        if (!cur_drv) {
            break;
        }
        if (cur_drv->driver.type == LV_INDEV_TYPE_KEYPAD) {
            lv_indev_set_group(cur_drv, group);
        }
    }
    return;
}


void lvgl_group_set_obj(lv_obj_t* obj)
{
    if (group == NULL)
        lvgl_group_create();
    lv_group_add_obj(group, obj);
    lv_group_focus_obj(obj);
}
int lv_start(void)
{
    int ret;
    m_lvg_init = 1;
    while (1)
    {
   
        lv_start_lock(1);
        ret = lv_task_handler();
        lv_start_lock(0);
        ddi_sys_msleep(10);
    }
    return 0;
}

void lv_set_ts_read_flag(int flag)
{
    m_ts_read_flag = flag;
}



int lv_get_ts_x()
{
    return ts_tx;
}


int lv_get_ts_y()
{
    return ts_ty;
}

static unsigned int m_start_lock = 0;

void lv_start_lock(int mode)
{
    if (m_start_lock ==0) {
        m_start_lock = (int)ddi_sem_new(1);
    }
    if (mode == 1) {
        ddi_sem_wait(m_start_lock);
    }
    else {
        ddi_sem_signal(m_start_lock);
    }
}
int lvgl_check_init()
{
    return m_lvg_init;
}




