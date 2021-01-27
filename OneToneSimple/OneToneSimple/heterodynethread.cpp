#include "heterodynethread.h"


void HeterodyneThread::run()
{
	heterodyneSettings.attenuation = 10;

	emit signalLog("test");

	for (int i = 0; i < 10; i++)
	{
		{
			QMutexLocker locker(&m_mutex);
			if (m_stop) break;
		}
		
		emit signalDataPoint(i, i);
	}
	
}

void HeterodyneThread::stop()
{
	qDebug("Thread::stop called from main thread: %d", currentThreadId());
	QMutexLocker locker(&m_mutex);
	m_stop = true;
}