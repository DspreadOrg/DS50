#ifndef _API_HTTP_H
#define _API_HTTP_H





int OsHttpHandle(char* url, char* method,char *headers,
	const char* params, const char* body,
	int bodylen, char** resp_content,
	int* resp_content_len);


int OsHttpHandleStatus(char* url, char* method, char* headers,
	const char* params, const char* body,
	int bodylen, char** resp_content,
	int* resp_content_len, int* status_code);

int OsHttpDownloadFile(char* url, char* savefilename, void (*pProgressfun)(int cur, int total));


void OsHttpSetTimeout(unsigned char val);


int OsHttpDownloadFileVerifyMd5(char* filename, unsigned char* hashOut);
#endif