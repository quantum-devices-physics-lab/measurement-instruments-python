#include "ProcessThread.h"
#include <math.h>


void ProcessThread::run()
{

	//Retirado do naghiloo thesis pg 38

	double dtau = (mSettings.finalTau - mSettings.initialTau) / (mSettings.nSteps - 1);
	double delta = 0.2;
	double  A = 1;
	double Omega = sqrt(A*A + delta * delta);

	for (double tau = mSettings.initialTau; tau <= mSettings.finalTau; tau += dtau)
	{
		{
			QMutexLocker locker(&m_mutex);
			if (m_stop) break;
		}

		

		double pop = A*A/(Omega*Omega)*sin(Omega*tau/2)*sin(Omega*tau/2);

		emit signalDataPoint( tau, pop);
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