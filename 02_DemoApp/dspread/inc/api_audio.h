#ifndef _API_AUDIO_H
#define _API_AUDIO_H

#define API_AUDIOPLAY_ERROR -2001
#define API_AUDIOCLEAR_ERROR -2002
#define API_AUDIOSTOP_ERROR -2003
#define API_AUDIOSETVOLUME_ERROR -2004
#define API_AUDIOGETVOLUME_ERROR -2005
#define API_AUDIOSETTTSSPEED_ERROR -2006
#define API_AUDIOPLAYLIST_ERROR -2007
 
int OsAudioPlay(char* filename);
int OsAudioClear();

 
int OsAudioSetVolume(int mode, int volume);


 
int OsAudioGetVolume(int mode);


 
int OsAudioStop();

 
int OsAudioTtsGetState();


 
int OsAudioTtsSetSpeed(int speed);


 
int OsAudioTtsSetPitch(int pitch);
int OsAudioTtsPlay(char* ttsbuf, int tts_type);

int OsAudioGetState();

int OsAudioListPlay(const char fname[][128], int num);

int OsAudioPlayList(char* filenamelist[][128], int listSize);

#endif