#ifndef _API_STRING_H
#define _API_STRING_H
#include "api_types.h"

 
unsigned int OsHex2Str(char* inchar, unsigned int len, char* outtxt);

 
unsigned int OsStr2Hex(char* str, char* hex);

 
void OsBufLefMove(char* buffer, int buf_len);

 
void OsBufRightMove(char* buffer, int buf_len);

 
void OsBufTurnOver(int8* buffer, int32 buf_len);
 
uint32 OsLrcCheck(int8* data, uint32 dlength);

 
unsigned int OsInt2Bcd(int num);


 
int OsStr2Bcd(char* str, unsigned char* bcd, int align);

 
void OsBcd2Str(unsigned char* bcd, int bcdlen, char* str);

 
int OsUnicode2Utf8(unsigned long* unic, unsigned char* pOutput, int outsize);

 
int OsUtf82Unicode(const unsigned char* utf8, unsigned long* unicode);


char* OsStr2Upper(char* str);

char* OsStr2Lower(char* str);

#endif