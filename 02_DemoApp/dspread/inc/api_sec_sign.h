#ifndef __API_SEC_SIGN_H
#define __API_SEC_SIGN_H
#include "api_types.h"

 
#define SIGN_SIGNINFO_LEN             352  
#define SIGN_HASH_SIZE                32
#define SIGN_SIGNATURE_SIZE           256
#define SIGN_CERT_LEN                 716  

 
typedef enum _cert_type_t{
	OS_CERT   = 0x01,
	APP_CERT  = 0x02,
	ACQ_CERT  = 0x04,
}cert_type_t;

typedef enum _sign_file_id_t
{
 S_FW_ID      =    0x80,  
 S_APP_ID     =    0xA0,    
 S_APPCERT_ID =    0xC0,   
 
}sign_file_id_t;
s32 OsSecSignQueryCert(int type, u8 *datalen, u8 *c_version, u8 *cs_num, u8 *c_ownership,u8 *c_owner, u8 *cv_time);
s32 OsSecSignVerify(u32 fid, u8 * filename);
s32 OsSecDelAppCert();

 
s32 OsSecSignQueryCertExt(int type, u8 *ver, u8 *cs_num, u8 *ower_ship, u8 *cv_time,u8* owner);


#endif
