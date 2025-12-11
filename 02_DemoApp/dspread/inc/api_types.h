#ifndef __API_TYPES_H__
#define __API_TYPES_H__










#define API_OK          0    
#define API_ERR         -1   
#define API_ETIMEOUT    -2  
#define API_EBUSY       -3   
#define API_ENODEV      -4 
#define API_EACCES      -5  
#define API_EINVAL      -6  
#define API_EIO         -7 
#define API_EDATA       -8  
#define API_EPROTOCOL   -9  
#define API_ETRANSPORT  -10 
#define API_ENOMEM  -11  

#define EM_SUCCESS	                    0 
#define EM_ERROR	                   -1	 
#define EM_SECERR_ETIMEOUT	       -2	 
#define EM_SECERR_EBUSY	           -3  
#define EM_SECERR_ENODEV	           -4   
#define EM_SECERR_EACCESS	           -5	 
#define EM_SECERR_PARAINVALID       -6   
#define EM_SECERR_EIO	               -7   
#define EM_SECERR_EDATA	           -8    
#define EM_SECERR_EPROTOCOL	       -9   
#define EM_SECERR_ETRANSPORT		   -10   
#define EM_SECERR_NOTSUPPORT		   -14  


 
#define NEXGO_FILE_OK 					0					 
#define NEXGO_FILE_EOF					-1					 
#define NEXGO_FILE_SEEK_ERROR  	-2								 
#define NEXGO_FILE_ERROR 				-3					 
#define NEXGO_FILE_NOEXIST			-4						 

 
#define DRV_OK          0   
#define DRV_ERR         -1  
#define DRV_ETIMEOUT    -2  
#define DRV_EBUSY       -3   
#define DRV_ENODEV      -4  
#define DRV_EACCES      -5  
#define DRV_EINVAL      -6 
#define DRV_EIO         -7  
#define DRV_EDATA       -8   
#define DRV_EPROTOCOL   -9   
#define DRV_ETRANSPORT  -10  
#define DRV_ENOMEM  -11 	 





// #ifndef s8
// #define s8 char
// #endif

#ifndef S8
#define S8 char
#endif

#ifndef int8
#define int8 char
#endif

#ifndef INT8
#define INT8 char
#endif
#ifndef u8
#define  u8  unsigned char
#endif
#ifndef U8
#define U8 unsigned char
#endif

#ifndef UINT8
#define UINT8 unsigned char
#endif

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef u16
#define u16 unsigned short
#endif


#ifndef U16
#define U16 unsigned short
#endif

#undef s16
#define s16 short


#ifndef S16
#define S16 short
#endif

#ifndef int32
#define int32 int
#endif

#ifndef INT32
#define INT32 int
#endif
#ifndef UINT32
#define UINT32 unsigned int
#endif

#ifndef uint32
#define uint32 unsigned int
#endif



#ifndef u32
#define u32 unsigned int
#endif



#ifndef U32
#define U32 unsigned int
#endif



#ifndef s32
#define s32 int
#endif

#ifndef S32
#define S32 int
#endif


#define NULL  (void *)0  

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifndef bool
#define bool unsigned char
#endif

#ifndef BOOL
typedef bool BOOL;
#endif

#ifndef WORD
typedef unsigned short WORD;
#endif

#ifndef DWORD
typedef unsigned int DWORD;
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif



#endif

