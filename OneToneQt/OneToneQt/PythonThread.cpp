#include "PythonThread.h"
#include "CustomPythonLib.h"

void PythonThread::run()
{
	qDebug("got the %f", PythonSettings.startFrequency);
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

		msleep(1000);
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

		msleep(1000);
	}

}

void PythonThread::loadAndStart(MeasurementSetting settings)
{
	qDebug("python test %f", process());
	PythonSettings = settings;
	emit this->start();
}


void PythonThread::stop()
{
	qDebug("Thread::stop called from main thread: %d", currentThreadId());
	QMutexLocker locker(&m_mutex);
	m_stop = true;
}