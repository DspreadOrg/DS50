#include "SDK.h"

#include "app_initResource.h"

void loadResource() {
	int ret = -1;
	if (OsFileExist("/ext/sp.bin")==1)
	{
	    OsFileRemove("/ext/sp.bin");
	}
	if (OsFileExist("/ext/tmp.tmp")==1)
	{
	    OsFileRemove("/ext/tmp.tmp");
	}
	if (OsFileExist("/spec/cert.crt")==1)
	{
	   OsFileRemove("/spec/cert.crt");
	}
	if (OsFileExist("/spec/app.img")==1)
	{
	    OsFileRemove("/spec/app.img");
	}

	
	if (!OsFileExistDir("/ext/audio")) {
		OsFileMkdir("/ext/audio");
	}
	if (!OsFileExistDir("/ext/ui")) {
		OsFileMkdir("/ext/ui");
	}
	if (!OsFileExistDir("/ext/fonts")) {
		OsFileMkdir("/ext/fonts");
	}
	if (!OsFileExistDir("/ext/key")) {
		OsFileMkdir("/ext/key");
	}

	if (OsFileExist(MP3_RESOURCE_LOAD_PATH)==1) {
		ret=OsFileUnzip(MP3_RESOURCE_LOAD_PATH, "/ext/audio");
		API_LOG_DEBUG("OsFileUnzip %s ret:%d", MP3_RESOURCE_LOAD_PATH, ret);
		ret=OsFileRemove(MP3_RESOURCE_LOAD_PATH);
		API_LOG_DEBUG("OsFileRemove ret:%d", ret);
	}

	if (OsFileExist(UI_RESOURCE_LOAD_PATH)==1) {
		ret = OsFileUnzip(UI_RESOURCE_LOAD_PATH, "/ext/ui");
		API_LOG_DEBUG("OsFileUnzip %s ret:%d", UI_RESOURCE_LOAD_PATH, ret);
		ret = OsFileRemove(UI_RESOURCE_LOAD_PATH);
		API_LOG_DEBUG("OsFileRemove ret:%d", ret);
	}

	if (OsFileExist(CERT_RESOURCE_LOAD_PATH)==1) {
		ret = OsFileUnzip(CERT_RESOURCE_LOAD_PATH, "/ext/cert");
		API_LOG_DEBUG("OsFileUnzip %s ret:%d", CERT_RESOURCE_LOAD_PATH, ret);
		ret = OsFileRemove(CERT_RESOURCE_LOAD_PATH);
	}
	else
	{
		API_LOG_DEBUG("%s not found",CERT_RESOURCE_LOAD_PATH);
	}

	if (OsFileExist("/flashFile0")==1) 
	{
		OsFileRemove("/flashFile0");
	}
	if (OsFileExist("/flashFile1")==1) 
	{
		OsFileRemove("/flashFile1");
	}
	if (OsFileExist("/ext/flashFile1")==1) 
	{
		OsFileRemove("/ext/flashFile1");
	}
	if (OsFileExist("/emvfile")==1)
	{
		OsFileRemove("/emvfile");
	}
}


/*******************************************
void app_lvgl_png_free() {
	lv_free_png_file(DSP_NETWORKSUCC_IMG);
	lv_free_png_file(DSP_NETWORKFAIL_IMG);
	lv_free_png_file(DSP_AUTHORIZATION_IMG);
	lv_free_png_file(DSP_SYSINIT_IMG);
	lv_free_png_file(DSP_WELCOME_IMG);
}
*****************************************/
