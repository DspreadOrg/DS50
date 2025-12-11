#ifndef __API_SEC_ALG_H
#define __API_SEC_ALG_H
#include "api_types.h"


S32 OsSecGetRandNum(u8 *pData, u32 dataLen);


S32 OsSecCalcDes(s32 mode, u8* data, u32 data_len, u8* key, u32 key_len, u8* out, u32* outlen);
S32 OsSecCalcDesCbc(s32 mode, u8* data, u32 data_len, u8* key, u32 key_len, u8* iv, u8* out, u32* outlen);

S32 OsSecCalcAes(s32 mode, u8* data, u32 data_len, u8* key, u32 key_len, u8* out, u32* outlen);
S32 OsSecCalcAesCbc(s32 mode, u8* data, u32 data_len, u8* key, u32 key_len, u8* iv, u8* out, u32* outlen);
s32 OsSecCalcSm4(s32 mode, u8* data, u32 data_len, u8* key, u32 key_len, u8* out, u32* outlen);
s32 OsSecCalcSm4Cbc(s32 mode, u8* data, u32 data_len, u8* key, u32 key_len, u8* iv, u8* out, u32* outlen);

s32 OsSecCalcMd5(u8* pData, u32 dataLen, u8* pHashValue, u32* outlen);
s32 OsSecCalcSm3(u8 *pData, u32 dataLen, u8 *pHashValue,u32 *outlen);
s32 OsSecCalcSha1(u8 *pData, u32 dataLen, u8 *pHashValue, u32* outlen);
s32 OsSecCalcSha256(u8 *pData, u32 dataLen, u8 *pHashValue, u32* outlen);
s32 OsSecCalcSha512(u8* pData, u32 dataLen, u8* pHashValue, u32* outlen);

S32 OsSecCalcMac(s32 mode, u8 alg, u32 idx, u8 *pData, u32 dataLen, u8 *pMac);
S32 OsSecRsaRecovery(s32 modlen, u8 *mod, u32 explen, u8 *exp, u8 *datain, u32 *dataoutlen, u8 *dataout);
s32 OsSecCalcData(s32 mode, u8 alg, u32 idx, u8 *pData, u32 dataLen, u8 *pDataOut, u32 *pDataOutLen);


/*******************************************************************  
Function Purpose:Set pinblock param
Input Paramters: 
              		mode: Bit[31:24],Key type 0:MK/SK,1:DUKPT
              		      Bit[24:16],Pin calc mode Set to 0
              	    alg: 0:DES,1:3DES,2:SM4,3:AES
              	    idx: Key index
              	    minLen: Pin minimun length
              	    maxLen: Pin maximn length
              	    timeOut: Second
              	    lenCard:Card number length
              	    pCardNo:Card number
              	    mark: N/A
              	    lenAmount: N/A
              	    amount:N/A
              	    lenPwdCollection:N/A
              	    pwdCollection:N/A
                  
                       		
Output Paramters:
					
					
Return Value:    0:success
                 other:failed
Remark: 
Modify:		     20230525
********************************************************************/
s32 OsSecSetPinblockParam(s32 mode,u8 alg, u8 idx, u8 minLen, u8 maxLen,u8 timeOut,u8 lenCard, u8 *pCardNo,\
			u8 mark,u8 lenAmount, u8 *amount);

s32 OsSecSetPinpadOfflineModeParam(s32 mode, u8 cardType, u8 minLen, u8 maxLen, u8 timeOut, u32 lenMod, u8* mod, u32 lenE, u8* e, u32 lenRand, u8* rand);


/*******************************************************************  
Function Purpose:Get pinblock 
Input Paramters: 
				  
							
Output Paramters:
				 data: Pinblock 
					   If key type is MK/SK,the data length is 16
					   else if key type is DUKPT,the data length is 26
					   (pinbock(16 byte) + ksn(10 byte)).
				 datalen:The length of data.	   
					
Return Value:	 PCI_PROCESSCMD_SUCCESS  -		success
					   other - failed
Remark: 
Modify: 		 20230525
********************************************************************/
s32 OsSecGetPinblockStatus(u8 *data, u8 *datalen);

s32 OsSecCalcEccEnc(u32 idx, u8 *pData, u32 datalen, u8 *pCipher, u32 *pCipherLen);
s32 OsSecCalcEccDec(u32 idx, u8 *pData, u32 datalen, u8 *pPlaintext, u32 *pPlaintextLen);
s32 OsSecCalcCrc16(u8 *pData, u32 dataLen);
s32 OsSecCalcRsaGen(s32 mode, u32 modlen, s32 exp, u32* pPkeylen, u8 *pPkey, u32* pSkeylen, u8 *pSkey);
S32 OsSecCalcRsaPk(u32 mode, u32 idx, u8 *pData, u32 datalen, u8 *pCipher, u32 *pCipherLen);
S32 OsSecCalcRsaSk(u32 mode, u32 idx, u8 *pData, u32 datalen, u8 *pCipher, u32 *pCipherLen);

/*******************************************************************  
Function Purpose:Calculate mac using dukpt
Input Paramters: 
                       macMode:
                       		0,xor
                       		1,ANSI9.9
                       		3,PBOC
                       		7,ANSI9.19
                       alg:
                       		only support 3DES
                       keyIndex:
                       		key index (0-9)
                       pData:
                       dataLen:
                  
                       		
Output Paramters:
					pMac: 
					pMacLen: Mac length
					pKsn: ksn data
					pKsnLen: ksn len(Fixed 10byte)
					
Return Value:    PCI_PROCESSCMD_SUCCESS  -      success
                       other - failed
Remark: 
Modify:		     20230525
********************************************************************/
s32 OsSecCalcDukptMac(s32 macMode,u8 alg,u32 keyIndex,u8* pData,u32 dataLen,u8*pMac,u32 *pMacLen,u8* pKsn,u32* pKsnLen);

/*******************************************************************  
Function Purpose:Calculate data using dukpt
Input Paramters: 
                       macMode:
                       		0x00, ECB decrypt
                       		0x01, ECB encrypt
                       		0x10, CBC decrypt
                       		0x11, CBC encrypt
                       alg:
                       		only support 3DES
                       keyIndex:
                       		key index (0-9)
                       pData:
                       dataLen:
                  
                       		
Output Paramters:
					pDataOut: 
					pOutLen:length
					pKsn: ksn data
					pKsnLen: ksn len(Fixed 10byte)
					
Return Value:    PCI_PROCESSCMD_SUCCESS  -      success
                       other - failed
Remark: N/A
Modify: N/A 		//20230525
********************************************************************/
s32 OsSecCalcDukptData(s32 mode,u8 alg,u8 key_type,u32 keyIndex,u8* pdataIn,u32 dataInLen,u8* pDataOut,u32* pOutLen,u8* pKsn,u32* pKsnLen);

/*******************************************************************  
Function Purpose: Generate the sm2 key
Input Paramters: 
				mode: reserved                                           		
Output Paramters:	
				pPkey: Public key  64 byte
				pSkey: Private key 32 byte
					
Return Value:  
				0,Verify suc
				Other,Fail
Remark: N/A
Modify: N/A 		//20230628
********************************************************************/
s32 OsSecCalcSm2Gen(s32 mode,u8* pPkey,u8* pSkey);

/*******************************************************************  
Function Purpose:  Using sm2 public key encrype
Input Paramters: 
                   idx: index of sm2 key  scope 0,1     
                   pDataIn: Data to be encrypted
                   inLen:  Length of the data to be encrypted 
Output Paramters:
				  pDataOut: Encrypted data
				  pOutLen:  Length of encrypted data
					
Return Value:  
					0,Verify suc
					Other,Fail
Remark: N/A
Modify: N/A 		//20230628
********************************************************************/
s32 OsSecCalcSm2PkEnc(u32 idx,u8* pDataIn,u32 inLen,u8* pDataOut,u32* pOutLen);

/*******************************************************************  
Function Purpose:  Using sm2 private key decrypted
Input Paramters: 
                   idx: index of sm2 key  scope 0,1     
                   pDataIn: Data to be decrypted
                   inLen:  Length of the data to be decrypted 
Output Paramters:
				  pDataOut: decrypted data
				  pOutLen:  Length of decrypted data
					
Return Value:   
				0,Verify suc
				Other,Fail
Remark: N/A
Modify: N/A 		//20230628
********************************************************************/
s32 OsSecCalcSm2SkDec(u32 idx,u8* pDataIn,u32 inLen,u8* pDataOut,u32* pOutLen);

/*******************************************************************  
Function Purpose: Sm2 sign use private key
Input Paramters: 
               		idx:   Index of sm2 key  scope 0,1 
               		usrId: User's id
               		idLen: Id length
               		pDataIn: Data to be signed
               		inLen: Length of data to be signed
Output Paramters:				
					pDataOut: Signed data
					pOutLen:  Length of the signed data
Return Value:   
					0,Verify suc
					Other,Fail
Remark: N/A
Modify: N/A 		//20230628
********************************************************************/
s32 OsSecCalcSm2Sign(u32 idx,u8* usrId,u32 idLen,u8* pDataIn,u32 inLen,u8* pDataOut,u32* pOutLen);


/*******************************************************************  
Function Purpose:Sm2 verify use private key
Input Paramters: 
                idx:   Index of sm2 key  scope 0,1 
               	usrId: User's id
               	idLen: Id length
               	pDataIn: Data to be verify
               	inLen: Length of data to be verify  
               	pSignData: signed data  64byte
Output Paramters:				
					
Return Value:    
					0,Verify suc
					Other,Fail
Remark: N/A
Modify: N/A 		//20230628
********************************************************************/
s32 OsSecCalcSm2Verify(u32 idx,u8* usrId,u32 idLen,u8* pDataIn,u32 inLen,u8* pSignData);
/*******************************************************************  
Function Purpose:CDC symmetrically encrypted, set iv
Input Paramters: u32 mode:reserve
			  u8 alg:
				KEYMGT_KA_DES	=  0,	   //DES
				KEYMGT_KA_TDES	=  1,	   //3DES
				KEYMGT_KA_SM4	=  2,	   //SM4
				KEYMGT_KA_AES	=  3	   //AES
			  u8 *pData: IV
			  u32 dataLen: IV len
Output Paramters:				
					
Return Value:    
					0,Verify suc
					Other,Fail
Remark: N/A
Modify: N/A 		
********************************************************************/
S32 OsSetIV(u32 mode, u8 alg, u8 *pData, u32 dataLen);
/*******************************************************************  
Function Purpose:Set the PIN entry length limit list
Input Paramters: u8 *table:length limit list
			  u32 tableLen: table len
Output Paramters:				
					
Return Value:    
					0,Verify suc
					Other,Fail
Remark: N/A
Modify: N/A 		
********************************************************************/
S32 OsSetPinLenTable(u8 *table, u32 tableLen);

#endif
