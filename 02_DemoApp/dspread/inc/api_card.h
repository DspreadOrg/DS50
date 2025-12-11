#ifndef __API_CARD_H
#define __API_CARD_H


/* IOCTL commands */
#define API_RF_CTL_VER                      0
#define API_RF_CTL_SAK                      1
#define API_RF_CTL_UID                      2
#define API_RF_CTL_MF_AUTH              3
#define API_RF_CTL_MF_READ_RAW          4
#define API_RF_CTL_MF_WRITE_RAW         5
#define API_RF_CTL_MF_READ_VALUE            6
#define API_RF_CTL_MF_WRITE_VALUE       7
#define API_RF_CTL_MF_INC_VALUE         8
#define API_RF_CTL_MF_DEC_VALUE         9
#define API_RF_CTL_MF_BACKUP_VALUE      (10)
#define API_RF_CTL_SET_PARAM                (11)
#define API_RF_CTL_GET_PARAM                (12)
#define API_RF_CTL_NFCBUILTIN               (13)
#define API_RF_CTL_RF_POWER_TEST			(14)
#define API_RF_CTL_GET_PICC_INFO 			(20)
#define API_RF_CTL_MF_GET_TYPE 			(21)
#define API_RF_CTL_GET_PICC_ATS         (22)

typedef struct _strMfAuth
{
	u8 m_authmode;          //0x60 for KEYA¡ê?0x61 for KEYB
	u8 m_key[6];            //Mifare key,6 bytes
	u8 m_uid[10];           //UID
	u8 m_block;                 //Block number
}strMfAuth;
typedef  struct _CL_PARAM
{
	u32 record_flag;
	u8 A_ModGsPReg_level;//H:0x29
	u8 A_RFCfg_level;//H:0x26
	u8 A_GsNReg_level;//H:0x27
	u8 A_CWGsPReg_level;//H:0x28
	u8 B_ModGsPReg_level;//
	u8 B_RFCfg_level;//
	u8 B_GsNReg_level;//
	u8 B_CWGsPReg_level;//
	u32 crc;
}CL_PARAM;
typedef struct _strRfChipParam
{
	u8 m_modindex;          //Modulation index
	u8 m_rfolevel;     		//RFO power level
	u8 m_typeagain;         //Gain of Type A
	u8 m_typebgain;         //Gain of Type B
	//u8 m_felicagain;      //Gain of FeliCa
	u8 m_rfu[28];           //Reserved for future use
} strRfChipParam;


 
#define DEV_IC_NO       0
#define DEV_SIM_NO1     1
#define DEV_SIM_NO2     2
#define SIM_NO_MAX      2

#define API_ICCPSAM_CTL_VER                 0   
#define API_ICCPSAM_CTL_ETUD                1    
#define API_MEMORY_CTL_POWEROFF             2   
#define API_SLE4428_CTL_RESET               3    
#define API_SLE4428_CTL_READ                4  
#define API_SLE4428_CTL_WRITE               5   
#define API_SLE4428_CTL_READEC      6   
#define API_SLE4428_CTL_VERIFY              7   
#define API_SLE4428_CTL_UPDATESC        8  
#define API_SLE4442_CTL_RESET               9    
#define API_SLE4442_CTL_READ                10   
#define API_SLE4442_CTL_WRITE               11   
#define API_SLE4442_CTL_READEC      12  
#define API_SLE4442_CTL_VERIFY              13 
#define API_SLE4442_CTL_UPDATESC            14 
#define API_AT24CXX_CTL_READ                15 
#define API_AT24CXX_CTL_WRITE               16  
#define API_AT88SC153_1608_CTL_RESET        17  
#define API_AT88SC153_1608_CTL_READ         18  
#define API_AT88SC153_1608_CTL_WRITE        19  
#define API_AT88SC153_1608_CTL_READEC   20  
#define API_AT88SC153_1608_CTL_VERIFY       21   
#define API_AT88SC153_1608_CTL_UPDATEEC 22  
#define API_IS23SC102_1604_CTL_RESET        23  
#define API_IS23SC102_1604_CTL_READ         24  
#define API_IS23SC102_1604_CTL_WRITE        25  
#define API_IS23SC102_1604_CTL_ERASE        26   
#define API_IS23SC102_1604_CTL_READEC       27   
#define API_IS23SC102_1604_CTL_VERIFY       28  
#define API_IS23SC102_1604_CTL_UPDATEEC 29  
#define API_ICCPSAM_CTL_HWVER								30
#define API_ICCPSAM_CTL_STD									31
#define API_SLE4442_CTL_ICCDETECT            32
typedef struct _strSle4428
{
	u32  m_protect;   
	u32  m_addr;     
	u32  m_le;       
} strSle4428;

typedef struct _strSle4442
{
	u32  m_area;   
	u32  m_addr;    
	u32  m_le;     
} strSle4442;

typedef struct _strAT24Cxx
{
	u32  m_type;    
	u32  m_addr;   
	u32  m_le;    
} strAT24Cxx;

typedef struct _strAT88SCxx
{
	u32  m_type;    
	//u8   m_zone;    
	u32  m_addr;   
	u32  m_le;      
} strAT88SCxx;


typedef struct _strAT88SCxxReadEC
{
	u32  m_type;   
	u32  m_mode;  
	u32  m_index;  
} strAT88SCxxReadEC;


typedef struct _strAT88SCxxVerify
{
	u32  m_type;    
	//u8   m_zone;  
	u32  m_mode;    
	u8   m_key[3];  
	u8  m_index;
} strAT88SCxxVerify;
 
typedef struct _strIs23SC1604
{
	u32  m_addr;   
	u32  m_le;    
} strIs23SC1604;

typedef struct _strIs23SC1604ReadEC
{
	u32  m_zone;    
	u32  m_mode;   
} strIs23SC1604ReadEC;


typedef struct _strIs23SC1604Verify
{
	u32  m_zone;   
	u32  m_mode;  
	u8   m_key[2]; 
} strIs23SC1604Verify;

//magnetic
#define API_MAG_CTL_VER                0x00
#define API_MAG_CTL_SECHEADVER   		0x01
#define API_MAG_CTL_GRADE            0x02
#define API_MAG_CTL_RESET             0x03
#define API_MAG_CTL_XGDENCRYPT   0x04
#define API_MAG_CTL_DATA		0x12 
#define API_MAG_CTL_CFG		0x13 
#define API_MAG_CTL_LRC         0x00000014
#define  READ_MAG_VERSION_CNT 15

//apdu struct
typedef struct
{
	unsigned char   Command[4]; // CLA INS  P1 P2
	int  Lc;         // P3
	unsigned char   DataIn[512];//512
	int  Le;
} APDU_SEND;

typedef struct
{
	int  LenOut;     // length of dataout 
	unsigned char   DataOut[512];
	unsigned char   SWA;
	unsigned char   SWB;
} APDU_RESP;


#endif