#ifndef _API_BASE64_H
#define _API_BASE64_H



 
int OsBase64Encode(unsigned char* dst, unsigned int dlen, unsigned int* olen, const unsigned char* src, unsigned int slen);

 
int Osbase64Decode(unsigned char* dst, unsigned int dlen, unsigned int* olen, const unsigned char* src, unsigned int slen);

#endif