/**
 * @breif  Public processing functions
 */
#include "lvgl.h"
#include "app_lvgl.h"
#include "app_pub.h"
#include "app_button.h"
#include "api_wifi.h"

unsigned char waitflag=0;


DECIMALPOINT point;
u32 mutex_lvgl;
char serialID[64]; // devise serial number
bool inputpinpassward;
bool lcdType=true;
u32 hibernate_startTick;


//Save up to 10 consumption records
int app_write_record(TRANSINFO* new_record)
{
	int file_id;
	int record_size = sizeof(TRANSINFO);
	long file_size;
	int record_count;
	int write_index;
	int next_write_index;
	int ret;
	file_size = tms_file_getfilesize(TRANSRECORD_FILE);
	if (file_size == 0)
	{
		next_write_index = 0;
		OsFileWrite(file_id, (unsigned char*)&next_write_index, INDEX_SIZE);
	}
	else if (file_size < INDEX_SIZE)
	{
		API_LOG_DEBUG("File size is less than index size. File might be corrupted.");
		OsFileRemove(TRANSRECORD_FILE);
		OsFileClose(file_id);
		return -1;
	}

	file_id = OsFileOpen(TRANSRECORD_FILE, API_RDWR | API_CREAT);
	if (file_id <= 0)
	{
		API_LOG_DEBUG("OsFileOpen fail ");
		return -1;
	}

	OsFileSeek(file_id, 0, API_FS_SEEK_SET);
	OsFileRead(file_id, (unsigned char*)&next_write_index, INDEX_SIZE);


	write_index = (next_write_index % MAX_RECORDS) * record_size + INDEX_SIZE;

	ret = OsFileSeek(file_id, write_index, API_FS_SEEK_SET);
	if (ret < 0)
	{
		API_LOG_DEBUG("OsFileSeek fail ");
		OsFileClose(file_id);
		return -1;
	}

	ret = OsFileWrite(file_id, (char*)new_record, record_size);
	if (ret != record_size)
	{
		API_LOG_DEBUG("OsFileWrite fail ");
		OsFileClose(file_id);
		return -1;
	}


	next_write_index = (next_write_index + 1) % MAX_RECORDS;
	API_LOG_DEBUG("write_record index=[%d]", next_write_index);
	ret = OsFileSeek(file_id, 0, API_FS_SEEK_SET);
	if (ret < 0)
	{
		API_LOG_DEBUG("n_api_file_seek fail ");
		OsFileClose(file_id);
		return -1;
	}

	ret = OsFileWrite(file_id, (char*)&next_write_index, INDEX_SIZE);
	if (ret != INDEX_SIZE)
	{
		API_LOG_DEBUG("n_api_file_write fail ");
		OsFileClose(file_id);
		return -1;
	}

	OsFileClose(file_id);
	return 0;
}

//Read all records
int app_read_all_records(TRANSINFO* records)
{
	int file_id;
	int record_size = sizeof(TRANSINFO);
	long file_size;
	int next_write_index;
	int record_count = 0;

	if (OsFileExist(TRANSRECORD_FILE) == 0)
		return -1;

	file_id = OsFileOpen(TRANSRECORD_FILE, API_RDONLY);
	if (file_id <= 0)
		return -1;

	OsFileSeek(file_id, 0, API_FS_SEEK_SET);
	OsFileRead(file_id, (unsigned char*)&next_write_index, INDEX_SIZE);
	API_LOG_DEBUG("read_all_records index=[%d]", next_write_index);


	OsFileSeek(file_id, INDEX_SIZE, API_FS_SEEK_SET);
	while (record_count < MAX_RECORDS && OsFileRead(file_id, (unsigned char*)&records[record_count], record_size) == record_size)
	{
		API_LOG_DEBUG("record[%d]->amount=[%s]", record_count, records[record_count].amount);
		API_LOG_DEBUG("record[%d]->audioplay=[%s]", record_count, records[record_count].audioplay);
		API_LOG_DEBUG("record[%d]->transdate=[%s]", record_count, records[record_count].transdate);
		API_LOG_DEBUG("record[%d]->transtime=[%s]", record_count, records[record_count].transtime);
		record_count++;
	}


	OsFileClose(file_id);
	return 0;
}
//Amount formatting
void app_amount_formatting(char* src, char* dst)
{
	int sLen = 0;

	sLen = strlen(src);
	if (sLen == 0)
	{
		sprintf(dst, "0.00");
	}
	else if (sLen == 1)
	{
		sprintf(dst, "0.0%c", src[0]);
	}
	else if (sLen == 2)
	{
		sprintf(dst, "0.%c%c", src[0], src[1]);
	}
	else
	{
		memcpy(dst, src, sLen - 2);
		dst[sLen - 2] = '.';
		memcpy(dst + sLen - 1, src + sLen - 2, 2);
		dst[sLen + 1] = 0;
	}
}



void app_pan_formatting(char* src, char* dst)
{
	int sLen = strlen(src);

	if (sLen == 0)
	{
		sprintf(dst, "0");
	}
	else
	{
		sprintf(dst, "%s", src);
	}
}

//Get signal strength
int app_wireless_get_csq(char* signalStatus)
{
	int rssi, ber;
	int ret = -1;

	ret = OsWlGetCsq(&rssi, &ber);
	if (ret != 0)
	{
		strcpy(signalStatus, "Error");
		return -1;
	}


	if (rssi == RSSI_INVALID)
		strcpy(signalStatus, "Invalid");
	else if (rssi >= RSSI_BEST_THRESHOLD)
		strcpy(signalStatus, "Best");
	else if (rssi >= RSSI_GOOD_THRESHOLD)
		strcpy(signalStatus, "Good");
	else if (rssi >= RSSI_WEAK_THRESHOLD)
		strcpy(signalStatus, "Weak");
	else
		strcpy(signalStatus, "Unknown");

	return 0;
}


//Convert string to hexadecimal
void app_dataToHexString(const char* input, int size, char* output)
{
	int i;
	for (i = 0; i < size; i++)
	{
		sprintf(output + i * 2, "%02X", (unsigned char)input[i]);
	}
	output[i * 2] = '\0';
}

size_t app_hex_to_ascii(const char* hex_str, char* ascii_str)
{
	size_t len = strlen(hex_str);
	if (len % 2 != 0)
		return -1;


	size_t ascii_len = len / 2;
	for (size_t i = 0; i < ascii_len; ++i)
	{
		unsigned int byte;
		sscanf(hex_str + 2 * i, "%2x", &byte);
		ascii_str[i] = (char)byte;
	}
	ascii_str[ascii_len] = '\0';

	return ascii_len;
}

void app_generate_dashes(int count, char* buf)
{
	int i;

	for (i = 0; i < count; i++)
	{
		buf[i] = '-';
	}

	buf[count] = '\0';
}


void app_seg_showMoney(char* str)
{
    
	OsBeepPlay(1,100,0);
	if(strlen(str)<10)
	OsSegShowMoney(str);
}

int app_reviseAmount(char* oldAmount, char* newAmount)
{
	char amt[20] = { 0 };
	int index = 0;
	int i;


	if (oldAmount == NULL || strlen(oldAmount) == 0)
	{
		API_LOG_DEBUG("Invalid input: cardpayamount is NULL or empty");
		return -1;
	}


	for (i = 0; i < strlen(oldAmount) && index < sizeof(amt) - 1; i++)
	{
		if (oldAmount[i] == '.')
			continue;

		if (!isdigit(oldAmount[i]))
		{
			API_LOG_DEBUG("Invalid input: cardpayamount contains non-digit characters");
			return -1;
		}
		amt[index++] = oldAmount[i];
	}


	if (index > 12)
	{
		API_LOG_DEBUG("Invalid input: cardpayamount exceeds maximum allowed digits");
		return -1;
	}

	sprintf(newAmount, "%012s", amt);

	API_LOG_DEBUG("app_reviseAmount=[%s]", newAmount);
	return 0;
}

void convertToWords(int num, char* result) {
	const char* units[] = { "", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine" };
	const char* teens[] = { "Ten", "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen" };
	const char* tens[] = { "", "Ten", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety" };
	const char* scales[] = { "", "Thousand", "Million", "Billion" };
	if (num == 0) {
		strcat(result, "Zero");
		return;
	}
	if (num >= 1000000000) {  // 
		int billion = num / 1000000000;
		convertToWords(billion, result);
		strcat(result, "Billion ");
		num %= 1000000000;
	}

	if (num >= 1000000) {  // 
		int million = num / 1000000;
		convertToWords(million, result);
		strcat(result, "Million ");
		num %= 1000000;
	}

	if (num >= 1000) {  // 
		int thousand = num / 1000;
		convertToWords(thousand, result);
		strcat(result, "Thousand ");
		num %= 1000;
	}

	if (num >= 100) {  //
		strcat(result, units[num / 100]);
		strcat(result, "Hundred ");
		num %= 100;
	}

	if (num >= 20) {  // 
		strcat(result, tens[num / 10]);
		strcat(result, " ");
		num %= 10;
	}
	else if (num >= 10) {  //
		strcat(result, teens[num - 10]);
		strcat(result, " ");
		return;
	}

	if (num > 0 && num < 10) {  // 
		strcat(result, units[num]);
		strcat(result, " ");
	}
}

int app_money_play(char* amount) {

	API_LOG_DEBUG("amount:%s\r\n", amount);
	char integerPart[64] = "";
	char decimalPart[125] = "";
	char result[256] = "";
	char* decimalPoint = strchr(amount, '.');
	if (decimalPoint != NULL) {
		*decimalPoint = '\0';
		int integerValue = atoi(amount);
		int decimalValue = atoi(decimalPoint + 1);

		if ((decimalValue % 10) == 0) {
			decimalValue /= 10;
		}

		if (integerValue == 0) {
			strcpy(integerPart, "Zero ");
		}
		else {
			convertToWords(integerValue, integerPart);
		}
		if (decimalValue == 0) {
			strcpy(decimalPart, "Zero ");
		}
		else {
			convertToWords(decimalValue, decimalPart);
		}
		strcat(result, integerPart);
		strcat(result, "Dollars and ");
		strcat(result, decimalPart);
		strcat(result, " Cents");
	}
	else {

		int integerValue = atoi(amount);
		if (integerValue == 0) {
			strcpy(result, "Zero Dollars");
		}
		else {
			convertToWords(integerValue, result);
			strcat(result, "Dollars");
		}
	}
	API_LOG_DEBUG("Amount in words: %s\n", result);
	app_tts_play("received", 1);
	app_tts_play(result, 0);
	return 0;
}

unsigned char app_getkey(void)
{
	while (waitflag==1)
	{
	   OsSysMsleep(100);
	}
    return OsKeypadGetKey();
}


int pub_getRtcTime(char *rtcTimeStr)
{
	int iRet=0;
	static int synclock=0;

    while (synclock==1)
	{
	   OsSysMsleep(100);
	}
	synclock=1;
	iRet=OsSysGetRtcTime(rtcTimeStr);
	synclock=0;
	return iRet;
}

int pub_GetNetWorkStatus()
{
	ST_AP_INFO stAPInfo;
	int ret = -1;
	int count = 10;

	do
	{
		if((OsWlGetNetState()== 1) || OsEspCheckApAsync(&stAPInfo) == 0)
		{
			ret = 0;
			break;
		}

		OsSysMsleep(1000);
		/* code */
	} while (count--);
	
	return ret;
}

int Pub_DateConvert(const char* dateStr, 
                   const char* targetFormat, 
                   char* outStr) {
    if (!dateStr || !targetFormat || !outStr) {
        return -1;
    }

    if (strlen(dateStr) != 14) {
        return -2; // 
    }

    int year = atoi(strndup(dateStr, 4));        // YYYY（4）
    int month = atoi(strndup(dateStr + 4, 2));   // MM（5-6）
    int day = atoi(strndup(dateStr + 6, 2));     // DD（7-8）
    int hour = atoi(strndup(dateStr + 8, 2));    // hh（9-10）
    int min = atoi(strndup(dateStr + 10, 2));    // mm（11-12）
    int sec = atoi(strndup(dateStr + 12, 2));    // ss（13-14）

    if (year < 1970 || year > 9999 ||          // year range
        month < 1 || month > 12 ||              // month range
        day < 1 || day > 31 ||                  // day range
        hour < 0 || hour > 23 ||                // hour range
        min < 0 || min > 59 ||                  // min range
        sec < 0 || sec > 59) {                  // sec range
        return -4;
    }

    if (strcmp(targetFormat, "YYYY/MM/DD hh:mm:ss") == 0) {
        sprintf(outStr, "%04d/%02d/%02d %02d:%02d:%02d",
                 year, month, day, hour, min, sec);
        return 0;
    } 
    else {
        return -3; // format error
    }
}

int Pub_AscToBcd(const char* src, char* dest,int asclen)
{
	int i;

	if (asclen % 2) {
		return 0;
	}
	else {}
	for (i = 0; i<asclen / 2; i++) {
		if (src[i * 2] >= '0' && src[i * 2] <= '9') {
			dest[i] = src[i * 2] - '0';
		}
		else if (src[i * 2] >= 'A' && src[i * 2] <= 'F') {
			dest[i] = src[i * 2] - 'A' + 0x0A;
		}
		else if (src[i * 2] >= 'a' && src[i * 2] <= 'f') {
			dest[i] = src[i * 2] - 'a' + 0x0A;
		}
		else {
			return 0;
		}
		dest[i] *= 16;
		if (src[i * 2 + 1] >= '0' && src[i * 2 + 1] <= '9') {
			dest[i] += src[i * 2 + 1] - '0';
		}
		else if (src[i * 2 + 1] >= 'A' && src[i * 2 + 1] <= 'F') {
			dest[i] += src[i * 2 + 1] - 'A' + 0x0A;
		}
		else if (src[i * 2 + 1] >= 'a' && src[i * 2 + 1] <= 'f') {
			dest[i] += src[i * 2 + 1] - 'a' + 0x0A;
		}
		else {
			return 0;
		}
	}
	return i;
}


int Pub_BcdToAsc(const unsigned char *pBcd, char *pAsc, unsigned int nBcdLen)
{
	unsigned int i;

	for (i = 0; i < (nBcdLen << 1); i++)
	{
		if (i & 0x01)
		{
			pAsc[i] = ((pBcd[i >> 1]) & 0x0F);
		}
		else
		{
			pAsc[i] = ((pBcd[i >> 1]) >> 4);
		}

		if (pAsc[i] > 9)
		{
			pAsc[i] += ('A' - 10);
		}
		else
		{
			pAsc[i] += '0';
		}
	} // end for
	pAsc[i] = '\0';
	return (nBcdLen << 1);
}
int App_IccCardOpen(int nSlotIndex)
{
	return OsIcCardOpen(nSlotIndex);
}
int App_ICCardPresent(int nSlotIndex)
{
	int ret = -1;
	char buf[128] = {0};
	do
	{
		if(OsIcCardGetStatus(nSlotIndex) != 0)
			break;
			OsSysMsleep(20);
		if(OsIcCardPowerOn(nSlotIndex,buf) != 0)
			break;
		ret = 1;
	} while (0);
	
	return ret;
}

int App_ICCardClose(int nSlotIndex)
{
	OsIcCardPowerOff(nSlotIndex);
	OsIcCardlose(nSlotIndex);

	return API_OK;
}

int App_PiccCardOpen()
{
	int ret = -1;
	do
	{
		if(OsRfOpen() != API_OK)
			break;
		if(OsRfPownOn(PICC_TYPE_AB) != API_OK)
			break;
		ret = 1;
	} while (0);

	return ret;	
}
int App_PiccCardPresent()
{
	int ret = -1;
	do
	{
		ret = OsRfGetStatus();
		if(ret != 3 && ret != 4)
		{
			ret = -1;
			break;
		}

		ret = OsRfActivate();
		if(ret != API_OK)
		{
			ret = -1;
			break;
		}

		ret = 1;
	} while (0);
	
	return ret;
}

int App_PiccCardClose()
{
	OsRfPowerOff();
	OsRfClose();

	return API_OK;
}


void rf_test()
{
	int cRet = 0;
	APDU_SEND ApduSend;
	APDU_RESP ApduResp;
	API_LOG_DEBUG("--------------%s %d-----------",__FUNCTION__,__LINE__);
	ApduSend.Command[0] = 0x00;
	ApduSend.Command[1] = 0xa4;
	ApduSend.Command[2] = 0x04;
	ApduSend.Command[3]=0x00;
	ApduSend.Lc=0x0e;
	ApduSend.Le=256;
	memcpy(ApduSend.DataIn,"2PAY.SYS.DDF01",ApduSend.Lc);
	OsRfOpen();
	OsRfPownOn(PICC_TYPE_A);
	cRet=OsRfGetStatus();
	if(cRet==3){
		API_LOG_DEBUG("--------------%s %d-----------",__FUNCTION__,__LINE__);
		if(!OsRfActivate())
		{
			API_LOG_DEBUG("--------------%s %d-----------",__FUNCTION__,__LINE__);
			if(!OsRfExchangeApdu(&ApduSend,&ApduResp))
			{
				API_LOG_DEBUG("--------------%s %d-----------",__FUNCTION__,__LINE__);
				ApduResp.DataOut[ApduResp.LenOut] = ApduResp.SWA;
				ApduResp.DataOut[ApduResp.LenOut + 1] = ApduResp.SWB;

				API_LOG_DEBUG_HEX(ApduResp.DataOut,ApduResp.LenOut + 2);
				while(1)
				{
					cRet=OsRfRemove();
					if(cRet==0)
						break;
					OsSysMsleep(100);
				}
			}
		}
	}
	OsRfPowerOff();
	OsRfClose();
}