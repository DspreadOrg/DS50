#ifndef __APP_AUDIO_H__
#define __APP_AUDIO_H__


#define MAX_AUDIO_VOLUME 5
#define MIN_AUDIO_VOLUME 1


#define TTS_PLAY 0
#define FILE_PLAY 1
#define APP_AUDIO_PLAY TTS_PLAY


void app_tts_play(char* src, int isBlock);
void app_file_play(char* filepath, int isBlock); 
void app_dsp_play(int audiotype,char *filename,char *tts,int isBlock); 
void app_beep_play(void);

#endif
