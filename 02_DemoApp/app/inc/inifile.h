/**
 * @file
 * @brief initialization file read and write API
 *	-size of the ini file must less than 16K
 *	-after '=' in key value pair, can not support empty char. this would not like WIN32 API
 *	-support comment using ';' prefix
 *	-can not support multi line
 */
 
#ifndef INI_FILE_H_
#define INI_FILE_H_
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{

#endif

int GetIniStr( const char *section, const char *key,const char *default_value, char *value, int size,const char *file);
int GetIniInt( const char *section, const char *key,int default_value, const char *file);
int SetIniStr( const char *section, const char *key,const char *value, const char *file);

#ifdef __cplusplus
}; //end of extern "C" {
#endif

#endif //end of INI_FILE_H_

