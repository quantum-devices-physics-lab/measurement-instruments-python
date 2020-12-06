#include "PythonThread.h"


void PythonThread::run()
{
	{
		QMutexLocker locker(&m_mutex);
		m_stop = false;
	}

	qDebug("Thread id inside run %d", (int)QThread::currentThreadId());
	for (int i =0; i < 10; i++) 
	{
		{
			QMutexLocker locker(&m_mutex);
			if (m_stop) break;
		}

		static double data = 0.0f;
		data += 0.1f;
		qDebug("data point sent %f", data);
		emit signalDataPoint(data,data+1);

		msleep(1000);
	}

}


void PythonThread::stop()
{
	qDebug("Thread::stop called from main thread: %d", currentThreadId());
	QMutexLocker locker(&m_mutex);
	m_stop = true;
}