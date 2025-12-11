#ifndef __API_SECURITY_H__
#define __API_SECURITY_H__
#include "api_types.h"


#define  API_SMGT_FW_VER                    0x00  
#define  API_SMGT_RSA_RECOVER               0x01  
#define  API_SMGT_TUSN_INFO                 0x02  
#define  API_SMGT_GETPIN_PARAM       		0x03   
#define  API_SMGT_GETPIN_STATUS      		0x04  
#define  API_SMGT_CALC_SEC_CODE      		0x05  
#define  API_SMGT_CHECK_KEY_STAT     		0x06  
#define  API_SMGT_UPDATE_CIPHER_MKEY 		0x07  
#define  API_SMGT_CHECK_LIB 		 		0x08   
#define  API_SMGT_UPDATE_CIPHER_MKEY_PRO 	0x09   
#define  API_SMGT_RSA_PK_DEC 	     		0x0A  
#define  API_SMGT_LIB_VER             		0x0B  
#define  API_SMGT_READ_LICENSE          	0x0C  
#define  API_SMGT_RSA_SK_ENC           		0x0D  
#define  API_SMGT_RSA_KEY_SETUP           	0x0E  
#define  API_SMGT_GEN_SM2KEY             	0x0F   
#define  API_SMGT_CALC_DATA_PRO             0x10  
#define  SMGT_CALC_SM4_PRO                  0x12  
#define  API_SMGT_CALC_AES                  0x14  
#define  API_SMGT_UPDATE_CIPHER_MKEY_SPEC   0x16 
#define  API_SMGT_UPDATE_TTK                0x17   

 
typedef struct _sec_rsa_param_t
{
	u8* pModule;     
	u32 modulelen;   
	u8* pExp;        
	u32 explen;      
	u8* pDatain;   
	u8* pDataout;    

}sec_rsa_param_t;

 
typedef struct _smgt_sec_calc_aes_t
{
	u32 mode;        
	u8* pData;      
	u32 uiDataLen;  
	u8* pKey;        
	u32 uiKeyLen;    
}sec_calc_aes_t;

 
typedef struct _sec_smgt_update_cipher_mk_pro_t
{
	u32 mkAlg;			 
	u32 mkIdx;			 
	u32 mkeyLen;		 
	u8 pCipherMkey[32];	 
	u32 chvlen;			 
	u8 pChkval[8];		 
	u32 ttkAlg;			 
	u32 ttkIdx;			 
	u8 rsv[8];			 
}sec_smgt_update_cipher_mk_pro_t;


 
typedef struct _keymgt_update_cipher_mk_spec_t
{
	u32 mkAlg;			 
	u32 mkIdx;			 
	u32 valueLen;       
	u8  pValueBuf[32];   
	u32 mkeyLen;		 
	u8 pCipherMkey[32]; 
	u32 chvlen;			 
	u8 pChkval[8];		 
	u32 ttkAlg;			 
	u32 ttkIdx;			 
	u8 rsv[8];			 
}keymgt_update_cipher_mk_spec_t;

 
typedef struct _smgt_sec_key_
{
	u8  alg;        
	u8  index;      
	u8* pkey;       
	u32 keyLen;    
}smgt_sec_key_data_t;

 
typedef enum _sdal_tamper_type_t
{
	SDAL_TT_NORMAL = 0,    
	SDAL_TT_TAMPER = 1,     
	SDAL_TT_BPK_TAMPER = 2,    
	SDAL_TT_OTHER_ERR = 3      

}sdal_tamper_type_t;

 
typedef struct _sdal_tamper_info_t
{
	sdal_tamper_type_t  type;   
	u32  errcode;              
}sdal_tamper_info_t;
#if 1
typedef enum _smgt_work_mode_t {
	SMGT_KM_FAC = 0x55,   
	SMGT_KM_DBG = 0x5A,  
	SMGT_KM_PROD = 0xA5,   
	SMGT_KM_FRN = 0xAA,   
}smgt_work_mode_t;

typedef enum _smgt_prod_stat_t {
	SMGT_PS_ACTIVE = 0x5A, 
	SMGT_PS_LOCK = 0xA5, 
}smgt_prod_stat_t;
#else
#endif

#endif


