#include "app_mutex.h"

static unsigned int m_standby_mutex = 0;


int app_standby_wait(void) {
	if (m_standby_mutex == 0) { m_standby_mutex = api_sys_semNew(0); }if (m_standby_mutex != 0)api_sys_semWaitTimeout(m_standby_mutex,15000);
	return 0;
}

int app_standby_signal(void) {
	if (m_standby_mutex != 0)api_sys_semSignal(m_standby_mutex);
	return 0;
}

