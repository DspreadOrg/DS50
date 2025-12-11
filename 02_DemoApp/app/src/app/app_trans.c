/**
 * @breif  QR code business related
 */
#include "lvgl.h"
#include "app_pub.h"
#include "app_trans.h"
#include "app_dsp.h"
#include "app_card_payment.h"
#include "app_dsp.h"
#include "app_lvgl.h"
#include "dsp_emv.h"


extern char g_dynamic_amountFormat[64];

extern bool gTransThreadFlag;
extern bool offlinepinflag;
extern int card_read_method;

static int tradeType = -1;

static unsigned int g_qrMutex = 0;
static unsigned int g_qr_queue = 0;
char g_qrOutTradeNo[64] = { 0 };

static unsigned int m_qrpay_mutext_lock = 0;
static unsigned int m_transThread_mutext_lock = 0;

#define QRPAY_MUTEX_LOCK		 if(m_qrpay_mutext_lock == 0){m_qrpay_mutext_lock= OsSysSemNew(1);}if(m_qrpay_mutext_lock!=0)OsSysSemWait(m_qrpay_mutext_lock);
#define	QRPAY_MUTEX_UNLOCK	  if(m_qrpay_mutext_lock!=0)OsSysSemSignal(m_qrpay_mutext_lock);

typedef struct
{
	int tranState;
}qr_queue_t;



int trans_thread_mutexLock(void) {
	if (m_transThread_mutext_lock == 0)
	{
		m_transThread_mutext_lock = OsSysSemNew(1);
	}
	if (m_transThread_mutext_lock != 0)
		OsSysSemWait(m_transThread_mutext_lock);
	return 0;
}


int trans_thread_mutexUnlock(void) {
	if (m_transThread_mutext_lock != 0)OsSysSemSignal(m_transThread_mutext_lock);
	return 0;
}


void app_trans_init(void) {
	if (g_qrMutex == 0) {
		g_qrMutex = OsSysMutexCreate();
	}
	if (g_qr_queue == 0)
		g_qr_queue = OsSysQueueCreate(1, sizeof(qr_queue_t));
}


void app_static_qr(void* param)
{
	API_LOG_DEBUG("app_static_qr start ......");
	int  ret = -1;
	char msgOut[64] = { 0 };
	trans_thread_mutexLock();
	OsSysAppLock();
	ret = app_dsp_getStaticQrcode(
		MCHNTCD, DEVICEID,
		DSP_URL, gQrUrl, gMchntcnName,
		msgOut);
	API_LOG_DEBUG("app_dsp_getStaticQrcode ret:%d,gQrUrl:%s,mchntcnNameStr:%s\r\n",
		ret, gQrUrl, gMchntcnName);
	if (ret != 0) {
		strcpy(gQrUrl, "https://www.dspread.com");
	}
	API_LOG_DEBUG("res gQrUrl:%s,mchntcnNameStr:%s\r\n", gQrUrl, gMchntcnName);
	app_static_qrcode_show(gQrUrl, "Dspread");
	OsSysAppUnLock();
	trans_thread_mutexUnlock();

}
int UnLockFlag=0;
 void WaitAppUnlock(void)
 {
    int startTick=0;
	static int syncflag=0;
	
	API_LOG_DEBUG("WaitAppUnlock start syncflag=%d",syncflag);
	while(syncflag==1)
	{
	   fibo_watchdog_feed();
	   OsSysMsleep(10);
	}
	syncflag=1;
	startTick=OsSysTick();
	API_LOG_DEBUG("WaitAppUnlock start UnLockFlag=%d",UnLockFlag);
	while(UnLockFlag==1)
	{
		if (OsSysTick() - startTick > 15000) {
			break;
		}
		fibo_watchdog_feed();
	    OsSysMsleep(10);
	}
	syncflag=0;
	API_LOG_DEBUG("WaitAppUnlock end AppLockStatus=%d",UnLockFlag);
 }

 void app_trans_process(void* param)
 {
  	int ret = -1;
 	char static_qr[256];
	char dynamic_qr[512];
	char timeStr[15];
	char tradeNoStr[32] = { 0 };
	char outtradeNoStr[64] = { 0 };
	char uniqueOrderIdentifier[64] = { 0 };
 
    char  moneyBuf[13];
    char msgTips[64] = { 0 };
	qr_queue_t qr_data = { 0 };
	int tranState = 0;
	u32 sleepstate;
	u32 startTick = 0;
	u8* sweep_code_buf = NULL;
	u32 code_len = 0;
	int code_type = 0;
	u8 transTime[16] = { 0 };
	u8 transAmountBuf[16] = { 0 };
	static int pending_flag = 0;
	APPEMVINFO emvInfo;
	unsigned char errorMsg[64]={0};
    int i=0;
	 int initFlag=0;
	 u32 memsize = 0, avail_size = 0, avail_block_size = 0;
	 pending_flag = 0;
	 while (1)
	 {
		 trans_thread_mutexLock();
 
		 if (gTransThreadFlag) 
		 {
		
			 OsSysAppLock();
			 API_LOG_DEBUG("app_trans_process start ......");
			
			 startTick = OsSysTick() + 120000;
			 memset(static_qr, 0, sizeof(static_qr));
			 memset(dynamic_qr, 0, sizeof(dynamic_qr));
	 
			 memset(moneyBuf, 0, sizeof(moneyBuf));
			 memset(&emvInfo, 0, sizeof(APPEMVINFO));
			 tradeType = -1;
			 if (g_qr_queue != 0)
		 	 OsSysQueueClear(g_qr_queue);
 
			 {
				 OsSysGetHeapInfo(&memsize, &avail_size, &avail_block_size);
				 API_LOG_DEBUG("app_trans_process start memory left:%u,%u,%u\r\n", memsize, avail_size, avail_block_size);
			 }
 
 
			 sprintf(moneyBuf, "%s", g_dynamic_amountFormat);
			 sprintf(transAmountBuf, "%d", (int)(atof(g_dynamic_amountFormat) * 100));
			 API_LOG_DEBUG("app_trans_process transAmountBuf:%s\r\n", transAmountBuf);

			 memset(g_qrOutTradeNo, 0, sizeof(g_qrOutTradeNo));
			 app_lvgl_cardpayShow(moneyBuf);
			 {
				 inputpinpassward = false;
				 offlinepinflag = false;
				 Pub_EMVOpenCard(CHECK_INSERTIC|CHECK_RFCARD);
			 }
             initFlag=1;
			 do
			 {
				 for(i=0;i<1;i++)
				 {
					 card_read_method = Pub_EMVCheckCard(CHECK_INSERTIC | CHECK_RFCARD);
					 if (card_read_method == 2 || card_read_method == 1)
					 {
						 if (card_read_method == 2)
						 {
							 tradeType = 2;
						 }
						 if (card_read_method == 1)
						 {
							 tradeType = 3;
						 }
						 gTraceNo = ++gTraceNo % 10000000;
						 sprintf(tradeNoStr, "%d", gTraceNo);
						 if (card_read_method == 1)
							 app_beep_play();
						 ret = Pub_EmvProcess(card_read_method);
						 if (ret == -8) {
							 app_dsp_play(0, "/ext/audio/english/dsp_transactioncancel.wav", "Transaction Cancelled", 0);
						 }
						 goto exit;
					 }
				 }
				 if (gTransThreadFlag == false) {
					 app_dsp_play(0,"/ext/audio/english/dsp_transactioncancel.wav","Transaction Cancelled", 0);
					 ret = -1;
					 goto exit;
				 }
				 if (OsSysTick() > startTick)
				 {
					 ret = -1;
					 tradeType = -1;
					 goto exit;
				 }
				 OsSysMsleep(100);
			 } while (1);
		 exit:
			 API_LOG_DEBUG("app_trans_process ret:%d,tradeType:%d\r\n", ret, tradeType);
			 sprintf(tradeNoStr, "%d", gTraceNo);
			 if (ret == APP_RC_COMPLETED) {
 
				 app_transaction_resultShow(tradeType, 1, moneyBuf, tradeNoStr, outtradeNoStr);
				 app_dsp_play(0,"/ext/audio/english/dsp_transactionsucc.wav","transaction success", 1);
			 }
			 else {
				 memset(msgTips, 0, sizeof(msgTips));
				 if ((tradeType == 0) || (tradeType == -1)) {
					 API_LOG_DEBUG("app_trans_process g_qrOutTradeNo:%s\r\n", g_qrOutTradeNo);
					 WaitAppUnlock();
				 }
				 app_transaction_resultShow(tradeType, 2, moneyBuf, tradeNoStr, outtradeNoStr);
				 app_dsp_play(0,"/ext/audio/english/dsp_transactionfail.wav","transaction fail", 1);
			 }
			 if(initFlag==1)
			 {
				 App_ICCardClose(IC_CARD_NO);
			 	 App_PiccCardClose();
				 initFlag=0;
			 }
			 OsSysMsleep(2000);
			 app_seg_showMoney("0.00");
			 inputpinpassward = false;
			 gTransThreadFlag = false;
			 {
				 OsSysGetHeapInfo(&memsize, &avail_size, &avail_block_size);
				 API_LOG_DEBUG("app_trans_process end memory left:%u,%u,%u\r\n", memsize, avail_size, avail_block_size);
			 }
			 handle_menu(0);
			 OsSysAppUnLock();
		 }
		 trans_thread_mutexUnlock();
		 OsSysMsleep(50);
 
	 }
 }

void app_transthread_paycheck(void* params) {

	int ret = -1;
	char msgTips[64] = { 0 };
	int qrpay_flag = 0;
    int tranState = -1;
	qr_queue_t qr_data = { 0 };
	while (1) {
		ret = -1;
		memset(msgTips, 0, sizeof(msgTips));
		memset(&qr_data, 0, sizeof(qr_queue_t));
		qrpay_flag = 0;
		tranState = -1;
	
		while (gTransThreadFlag && ((tradeType == -1) || (tradeType == 0))) {
			if (qrpay_flag == 2) {
				break;
			}
			memset(msgTips, 0, sizeof(msgTips));
			
			if (strlen(g_qrOutTradeNo)) {
			    API_LOG_DEBUG("app_transthread_paycheck STRAT ret:%d\r\n", ret);
				WaitAppUnlock();
				API_LOG_DEBUG("WaitAppUnlock\r\n");
				//������Ѿ�ȷ��֧����ʽ��ֱ���˳�
				if (tradeType == 2 || tradeType == 3 || tradeType ==1 ) {
					break;
				}
				UnLockFlag=1;
				API_LOG_DEBUG("app_dsp_checkPay start\r\n");
				ret = app_dsp_checkPay(
					MCHNTCD, DEVICEID, DSP_URL, g_qrOutTradeNo, &tranState,
					msgTips);
				API_LOG_DEBUG("app_dsp_checkPay end\r\n");
				UnLockFlag=0;
				API_LOG_DEBUG("app_transthread_paycheck ret:%d\r\n", ret);
			}
			if (ret != 0) {
				OsSysMsleep(3000);
				continue;
			}
			API_LOG_DEBUG("app_transthread_paycheck qrcode pay tranState:%d\r\n", tranState);
			memset(&qr_data, 0, sizeof(qr_queue_t));
			if (ret == 0 && tranState == 2)
			{
				qr_data.tranState = 2;
				OsSysQueuePut(g_qr_queue, (void*)&qr_data, 1000);
				qrpay_flag = 2;
			}
			else if (ret == 0 && tranState == 1)
			{
				qr_data.tranState = 1;
				if (qrpay_flag != 1) {
					qrpay_flag = 1;
					OsSysQueuePut(g_qr_queue, (void*)&qr_data, 1000);
				}
			}
			API_LOG_DEBUG("app_transthread_paycheck qrcode pay tranState:%d\r\n", tranState);

			OsSysMsleep(2000);

		}
		
		OsSysMsleep(1000);
	}
}

static int m_sleep_time = AUTO_SLEEP_TIME;
static int m_wake_type = 0;
static void hibernate_Func(int wake_type)
{
	API_LOG_DEBUG("sleep wake up callback,wake_up type:%d\r\n", wake_type);
	m_wake_type = wake_type;
	if (wake_type == 1) {
		m_sleep_time = 5;
	}
	else {
		m_sleep_time = AUTO_SLEEP_TIME;
	}

	app_hibernate_reset(m_sleep_time);
}
void app_hibernate_reset(int value) {
	hibernate_startTick = OsSysTick();
	m_sleep_time = value;
}

void app_hibernate(void* param)
{
	u32 hibernate_endTick;
	u32 sleepstate;
	hibernate_startTick = OsSysTick();
	while (1)
	{
	    TMS_IsBusyWait();
	    hibernate_endTick = OsSysTick();
		if ((hibernate_endTick - hibernate_startTick) > 1000 * m_sleep_time)
		{
			sleepstate = OsGetSleepState();
			//API_LOG_DEBUG("app_hibernate sleepstate=[%d]", sleepstate);
			if ((sleepstate == 0) && (gTransThreadFlag == false) && (OsAudioGetState() == 0) && (OsAudioTtsGetState() == 0) && (TMS_IsBusy() == 0))
			{
				API_LOG_DEBUG("app is start sleep......");
				if (m_wake_type == 0) {
					OsSleepCmd(SYS_CMD_SET_AUTO_SLEEP_AVAIBLE, 1, (u32)&hibernate_Func);
				}
				else if (m_wake_type == 1) {
					OsSleepCmd(SYS_CMD_SET_NET_AUTO_SLEEP_AVAIBLE, 1, (u32)&hibernate_Func);
				}
			}
			else if (sleepstate == 1) {
				API_LOG_DEBUG("app is sleeping");
			}
			else {
				API_LOG_DEBUG("app is running cannot sleep");
			}
		}
		OsSysMsleep(5000);
	}
}




