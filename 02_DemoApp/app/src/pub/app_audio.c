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
			if (!OsAudioTtsGetState())
				break;
			OsSysMsleep(100);
		} while (1);
	}
	OsAudioTtsPlay(ttsTmsBuf, 1);
	if (isBlock == 1) {
		do
		{
			if (!OsAudioTtsGetState())
				break;
			OsSysMsleep(100);
		} while (1);
	}

}



void app_file_play(char* filepath, int isBlock) {

	OsAudioPlay(filepath);
	if (isBlock == 1) {
		do
		{
			OsSysMsleep(1000);
			if (!OsAudioGetState())
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

	while (OsAudioTtsGetState() || OsAudioGetState())
	{
		OsSysMsleep(100);
	}
	OsBeepPlaying(0);
	/*do {
		OsSysMsleep(100);
	} while (OsBeepPlaying(0));
	*/
	while (OsAudioTtsGetState() || OsAudioGetState())
	{
		OsSysMsleep(100);
	}
	/*do {
		OsSysMsleep(100);
	} while (OsAudioTtsGetState() || OsAudioGetState());
	*/
}

