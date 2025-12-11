
#include "SDK.h"
#include "app_param.h"
int App_PosParamInit(void)
{	
#if CFG_APP_DEBUG_MODE	

	int nRet;
	char buf[30+1]={0};
	unsigned char mainKey[32]="\x55\x55\x55\x55\x55\x55\x55\x55\x66\x66\x66\x66\x66\x66\x66\x66";
	unsigned char PINKey[32]="\x4A\x2D\xC7\x50\x9E\x20\xFD\x47\x60\xBF\x3F\xEE\xB8\xC8\xE6\x5F";
	unsigned char kcv[4]="\xBB\x02\x56\x9C";


	nRet= OsSecUpdatePlainKey(0, 0x01, 0, mainKey, 16);
	API_LOG_DEBUG("tms OsSecUpdatePlainKey=[%d]",nRet);

	nRet= OsSecUpdateCipherKey(0x10, 1, 1, 0, 10, PINKey, 16, 1, kcv, 4);

	API_LOG_DEBUG("tms OsSecUpdateCipherKey=[%d]",nRet);

	if(OsFileExist(FILE_EMV_APP_PARAM)!=1 || OsFileExist(FILE_EMV_CAPK_PARAM)!=1)
	{
		EmvL2_SetDefaultAidAndCapk();
	}
#endif	
	return 0;
}

