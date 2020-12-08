#include "ProcessThread.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

void ProcessThread::run()
{
	srand(time(NULL));
	//Retirado do naghiloo thesis pg 38

	double dtau = (mSettings.finalTau - mSettings.initialTau) / (mSettings.nSteps - 1);
	double trueQFreq = 4.345;
	double delta = trueQFreq - mSettings.qFreq;
	double  A = 1;
	double Omega = sqrt(A*A + delta * delta);
	double pop = 0;

	for (double tau = mSettings.initialTau; tau <= mSettings.finalTau; tau += dtau)
	{
		{
			QMutexLocker locker(&m_mutex);
			if (m_stop) break;
		}

		

		double prob = A*A/(Omega*Omega)*sin(Omega*tau/2)*sin(Omega*tau/2)*100;
		
		pop = 0;
		for (int i = 0; i < mSettings.nIter; i++)
		{
			double coin = (rand() % 100);

			if (coin < prob)
			{
				pop += 1.0f;
			}
		}

		pop /= mSettings.nIter;

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