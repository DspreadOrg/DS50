#ifndef _API_FILE_H
#define _API_FILE_H
#include <stdbool.h>

#define	API_RDONLY		0		    
#define	API_WRONLY		1		 
#define	API_RDWR		2		 
#define	API_APPEND      0x0008	    
#define	API_CREAT		0x0200	 
#define	API_TRUNC       0x0400	 
#define	API_EXCL		0x0800	 


#define API_FS_SEEK_SET  0
#define API_FS_SEEK_CUR  1
#define API_FS_SEEK_END  2

typedef struct fileinfo_t{
	char dirname[64];
	char filename[128];
	unsigned int filesize;
}fileinfo_t;


 
 
int OsFileOpen(char* pchFileName, unsigned int ucMode);

 
int OsFileRead(int iFileId, unsigned char* pucOutData, unsigned int iReadLen);

 
int OsFileReadFile2Buf(const char *pchFileName, unsigned int offset, char *buf, unsigned int len);

 
int OsFileWrite(int iFileId, char *pucInData, unsigned int iInLen);

 
int OsFileClose(int iFileId);

 
int OsFileSeek(int iFileId, int lOffset, unsigned char iMode);


 
int OsFileRemove(const char* pchFileName);


int OsFileUnlink(const char* pchFileName);
 
long OsFileGetFileSysFreeSize(void);

 
long OsFileGetFileSize(const char* pchFileName);
 
int OsFileExist(char* pchFileName);

 
int OsFileReName(char* pchOldFileName, char* pchNewFileName);

 
int OsFileMkdir(char* pchDirName);

 
int OsFileRmdir(char* pchDirName);


 
bool OsFileExistDir(const char* pchDirName);

 
int OsFileLstdir(char* pchDirName, fileinfo_t* fileinfo);

 
int OsFileTypeCheck(char* filepath);

int OsFileUnzip(const char* file_path, const char* zip_path);
 
int OsFilePathFormat(char* pchDirName);
int OsFileCopy(char *src,char *dst);
#endif