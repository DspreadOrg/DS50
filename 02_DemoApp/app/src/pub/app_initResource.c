#include "SDK.h"

#include "app_initResource.h"

void loadResource() {
	int ret = -1;
	if (api_file_exist("/ext/sp.bin")==1)
	{
	    api_file_remove("/ext/sp.bin");
	}
	if (api_file_exist("/ext/tmp.tmp")==1)
	{
	    api_file_remove("/ext/tmp.tmp");
	}
	if (api_file_exist("/spec/cert.crt")==1)
	{
	   api_file_remove("/spec/cert.crt");
	}
	if (api_file_exist("/spec/app.img")==1)
	{
	    api_file_remove("/spec/app.img");
	}

	
	if (!api_file_existDir("/ext/audio")) {
		api_file_mkdir("/ext/audio");
	}
	if (!api_file_existDir("/ext/ui")) {
		api_file_mkdir("/ext/ui");
	}
	if (!api_file_existDir("/ext/fonts")) {
		api_file_mkdir("/ext/fonts");
	}
	if (!api_file_existDir("/ext/key")) {
		api_file_mkdir("/ext/key");
	}

	if (api_file_exist(MP3_RESOURCE_LOAD_PATH)==1) {
		ret=api_file_unzip(MP3_RESOURCE_LOAD_PATH, "/ext/audio");
		API_LOG_DEBUG("api_file_unzip %s ret:%d", MP3_RESOURCE_LOAD_PATH, ret);
		ret=api_file_remove(MP3_RESOURCE_LOAD_PATH);
		API_LOG_DEBUG("api_file_remove ret:%d", ret);
	}

	if (api_file_exist(UI_RESOURCE_LOAD_PATH)==1) {
		ret = api_file_unzip(UI_RESOURCE_LOAD_PATH, "/ext/ui");
		API_LOG_DEBUG("api_file_unzip %s ret:%d", UI_RESOURCE_LOAD_PATH, ret);
		ret = api_file_remove(UI_RESOURCE_LOAD_PATH);
		API_LOG_DEBUG("api_file_remove ret:%d", ret);
	}

	if (api_file_exist(CERT_RESOURCE_LOAD_PATH)==1) {
		ret = api_file_unzip(CERT_RESOURCE_LOAD_PATH, "/ext/cert");
		API_LOG_DEBUG("api_file_unzip %s ret:%d", CERT_RESOURCE_LOAD_PATH, ret);
		ret = api_file_remove(CERT_RESOURCE_LOAD_PATH);
	}
	else
	{
		API_LOG_DEBUG("%s not found",CERT_RESOURCE_LOAD_PATH);
	}

	if (api_file_exist("/flashFile0")==1) 
	{
		api_file_remove("/flashFile0");
	}
	if (api_file_exist("/flashFile1")==1) 
	{
		api_file_remove("/flashFile1");
	}
	if (api_file_exist("/ext/flashFile1")==1) 
	{
		api_file_remove("/ext/flashFile1");
	}
	if (api_file_exist("/emvfile")==1)
	{
		api_file_remove("/emvfile");
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
