#include "ProcessThread.h"
#include <math.h>


void ProcessThread::run()
{

	double dtau = (mSettings.finalTau - mSettings.initialTau) / (mSettings.nSteps - 1);

	for (double tau = mSettings.initialTau; tau <= mSettings.finalTau; tau += dtau)
	{
		{
			QMutexLocker locker(&m_mutex);
			if (m_stop) break;
		}

		double result = cos(tau)+1;

		emit signalDataPoint( tau, result);
	}


}

void ProcessThread::loadAndStart(MeasurementSetting settings)
{
	{
		QMutexLocker locker(&m_mutex);
		m_stop = false;
	}
	mSettings = settings;
	emit this->start();
}


void ProcessThread::stop()
{
	QMutexLocker locker(&m_mutex);
	m_stop = true;
}