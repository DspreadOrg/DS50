#ifndef _API_LOG_H
#define _API_LOG_H

#include "api_types.h"
#include <stdarg.h>
enum {
	DEBUG_LEVEL=0,
	INFO_LEVEL=1,
	WARNING_LEVEL=2,
	ERROR_LEVEL=3,
	MAX_LEVEL=4,
};

#define API_LOG_DEBUG(format,...)   api_log_output(DEBUG_LEVEL,format,##__VA_ARGS__)
#define API_LOG_INFO(format,...)   api_log_output(INFO_LEVEL,format,##__VA_ARGS__)
#define API_LOG_WARNING(format,...)   api_log_output(WARNING_LEVEL,format,##__VA_ARGS__)
#define API_LOG_ERROR(format,...)   api_log_output(ERROR_LEVEL,format,##__VA_ARGS__)
#define API_LOG_DEBUG_HEX(data,datalen)  OsLogHexOutput(DEBUG_LEVEL,data,datalen)
#define API_LOG_INFO_HEX(data,datalen)    OsLogHexOutput(INFO_LEVEL,data,datalen)
#define API_LOG_WARNING_HEX(data,datalen)    OsLogHexOutput(WARNING_LEVEL,data,datalen)
#define API_LOG_ERROR_HEX(data,datalen)   OsLogHexOutput(ERROR_LEVEL,data,datalen)


 
void OsLogSwitch(int val);


 
void OsLogSetLevel(unsigned char value);
 
void api_log_output(unsigned char level, const char* format, ...);
 
void OsLogHexOutput(unsigned char level, unsigned char* data, int len);

 
void OsLogSetPause();

 
void OsLogSetResume();


#endif
