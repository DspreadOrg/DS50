#ifndef __API_SEC_KEY_H
#define __API_SEC_KEY_H
#include "api_types.h"
#include "api_security.h"
s32 OsSecUpdatePlainKey(u8 type, u8 alg, u32 idx, u8 *pMkey, u32 mkeyLen);
s32 OsSecUpdateCipherKey(u8 type, u8 mk_alg, u8 wk_alg, u32 mk_idx, u32 wk_idx, u8 *pCipher, u32 cipherLen, u8 chvmod, u8 *chkval, u32 chkvalLen);

/*******************************************************************  
Function Purpose:update dukpt key(BDK/IPEK)
Input Paramters: 
				group: Group id ,Fixed to 0
				type:  Key type,0:IPED,1:BDK
				idx�?  Key index，The value ranges from 0 to 4
				sec_len: The key length,The maximum value is 24
				ipek: Key info
				ksn: Ksn info ,Fixed 10 bytes
				check_crc: Use 3DES to encrypt eight zeros and take the first 4 bytes.
                                                    					
Return Value:   
				0:Suc
				Other: Fail
Remark: N/A
Modify: N/A 		//20230712
********************************************************************/
s32 OsSecUpdateDukpt(u8 group, u8 type, u8 idx, u8 sec_len, u8 *ipek, u8 *ksn, u8* check_crc);
s32 OsSecCheckKeyStatus(u8 type, u8 alg, u32 idx, u8 *crc16);
s32 OsSecDelKey(u8 type, u8 alg, u32 idx);
s32 OsSecUpdateEccKey(u32 mode, u32 idx, u8 *pPkey, u8 *pSkey);
s32 OsSecUpdateTr31(u8 * app_path, u8 alg, u8 type, u8 mk_alg, u8 wk_alg, u8 mk_index,u8 wk_index,u8 datalen,u8 * data,int tr31_len,u8 * tr31_data);
s32 OsSecUpdateRsaKey(u32 mode, u32 idx, u32 modlen, u32 exp, u32 pPkeylen, u8 *pPkey, u32 pSkeylen, u8 *pSkey);

/*******************************************************************  
Function Purpose:update SM2 Key
Input Paramters: 
				mode�?  reserved
				idx�?  key index，range,0,1
                                                    		
Output Paramters:
				pPkey: public key 64 byte
				pSkey: private key 32 byte
					
Return Value:    
Remark: N/A
Modify: N/A 		//20230628
********************************************************************/
s32 OsSecUpdateSm2Key(u32 mode, u32 idx, u8 *pPkey, u8 *pSkey);

#endif
