#include "app_mutex.h"

static unsigned int m_standby_mutex = 0;


int app_standby_wait(void) {
	if (m_standby_mutex == 0) { m_standby_mutex = OsSysSemNew(0); }if (m_standby_mutex != 0)OsSysSemWaitTimeout(m_standby_mutex,15000);
	return 0;
}

int app_standby_signal(void) {
	if (m_standby_mutex != 0)OsSysSemSignal(m_standby_mutex);
	return 0;
}

