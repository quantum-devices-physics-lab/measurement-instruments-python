#include "PythonThread.h"


void PythonThread::run()
{
	{
		QMutexLocker locker(&m_mutex);
		m_stop = false;
	}
	static double data = 0.0f;
	qDebug("Thread id inside run %d", (int)QThread::currentThreadId());
	for (int i =0; i < 10; i++) 
	{
		{
			QMutexLocker locker(&m_mutex);
			if (m_stop) break;
		}

		
		data += 0.1f;
		qDebug("data point sent %f", data);
		emit signalDataPoint(0,data,data+1);

		msleep(10);
	}
	data = 0.0f;
	for (int i = 0; i < 10; i++)
	{
		{
			QMutexLocker locker(&m_mutex);
			if (m_stop) break;
		}

		data += 0.1f;
		qDebug("data point sent %f", data);
		emit signalDataPoint(1,data, data );

		msleep(10);
	}

}


void PythonThread::stop()
{
	qDebug("Thread::stop called from main thread: %d", currentThreadId());
	QMutexLocker locker(&m_mutex);
	m_stop = true;
}