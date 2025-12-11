#include "app_audio.h"
#include "app_pub.h"


void app_tts_play(char* src, int isBlock) {
	int i = 0;
	char ttsTmsBuf[512] = { 0 };
	memset(ttsTmsBuf, 0, sizeof(ttsTmsBuf));
	for (i = 0; i < strlen((char*)src); i++) {
		sprintf(ttsTmsBuf + 2 * i, "%02x", src[i]);
	}

	if (isBlock == 1) {
		do
		{	
			if (!api_audio_ttsGetState())
				break;
			api_sys_msleep(100);
		} while (1);
	}
	api_audio_ttsPlay(ttsTmsBuf, 1);
	if (isBlock == 1) {
		do
		{
			if (!api_audio_ttsGetState())
				break;
			api_sys_msleep(100);
		} while (1);
	}

}



void app_file_play(char* filepath, int isBlock) {

	api_audio_play(filepath);
	if (isBlock == 1) {
		do
		{
			api_sys_msleep(1000);
			if (!api_audio_getState())
				break;
		} while (1);
	}
}


void app_dsp_play(int audiotype,char *filename,char *tts,int isBlock) {
  
	if (audiotype == 0) {
		app_tts_play(tts, isBlock);
	}
	else if (audiotype == 1) {
		app_file_play(filename, isBlock);
	}
}

void app_beep_play(void) {

	while (api_audio_ttsGetState() || api_audio_getState())
	{
		api_sys_msleep(100);
	}
	api_beep_playing(0);
	/*do {
		api_sys_msleep(100);
	} while (api_beep_playing(0));
	*/
	while (api_audio_ttsGetState() || api_audio_getState())
	{
		api_sys_msleep(100);
	}
	/*do {
		api_sys_msleep(100);
	} while (api_audio_ttsGetState() || api_audio_getState());
	*/
}

