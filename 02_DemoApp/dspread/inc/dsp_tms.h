#ifndef _DSP_TMS_H
#define _DSP_TMS_H

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
|   Typedefs
+----------------------------------------------------------------------------*/

enum TMS_ERR_RCODE {
    TMS_ERR_OK        =  0,
    TMS_ERR_PARAM     = -1,
    TMS_ERR_CONNECT   = -2,
    TMS_ERR_NO_AVAIALABLE = -3,
    TMS_ERR_SN_NOT_REGISTER = -4,
};


typedef enum
{
    TMS_DISP_START_CHECK = 0,
    TMS_DISP_HAVE_UPDATE_TASE,
    TMS_DISP_START_DOWNLOAD,
    TMS_DISP_DOWNLOADING_PROGRESS,
    TMS_DISP_DOWNLOAD_SUCCESS,
    TMS_DISP_DOWNLOAD_FAIL,
    TMS_DISP_VERIFY_FAIL,
    TMS_DISP_UPGRADING,
    TMS_DISP_UPGRADE_SUCCESS,
    TMS_DISP_UPGRADE_FAIL,
}TMS_DISP_STEP;

extern void larktms_service_start(void (*Tms_disp)(unsigned int, void*),unsigned char* url,unsigned char* app_ver);
extern int TMS_IsBusy();
extern void TMS_IsBusyWait();
#endif
