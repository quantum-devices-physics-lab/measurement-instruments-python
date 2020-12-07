#include "PythonThread.h"


double lorentzian(double freq, double res_freq, double sigma)
{
	return (sigma / 2) / ((freq - res_freq)*(freq - res_freq) + (sigma / 2)*(sigma / 2));
}


void PythonThread::run()
{

	double dfreq = (PythonSettings.stopFrequency - PythonSettings.startFrequency) / PythonSettings.nSteps;
	qDebug("Thread id inside run %d", (int)QThread::currentThreadId());


	//low power
	for (double freq = PythonSettings.startFrequency; freq <= PythonSettings.stopFrequency; freq+= dfreq)
	{
		{
			QMutexLocker locker(&m_mutex);
			if (m_stop) break;
		}

		double result = lorentzian(freq, freq - 0.05f, 0.01f);

		emit signalDataPoint(0,freq, result);
	}

	//high power
	for (double freq = PythonSettings.startFrequency; freq <= PythonSettings.stopFrequency; freq += dfreq)
	{
		{
			QMutexLocker locker(&m_mutex);
			if (m_stop) break;
		}

		double result = lorentzian(freq, freq, 0.01f);

		emit signalDataPoint(1, freq, result);
	}

}

void PythonThread::loadAndStart(MeasurementSetting settings)
{
	{
		QMutexLocker locker(&m_mutex);
		m_stop = false;
	}
	PythonSettings = settings;
	emit this->start();
}


void PythonThread::stop()
{
	qDebug("Thread::stop called from main thread: %d", currentThreadId());
	QMutexLocker locker(&m_mutex);
	m_stop = true;
}