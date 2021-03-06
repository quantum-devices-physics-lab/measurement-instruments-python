#include "ProcessThread.h"
#include <math.h>
#include <stdlib.h>
#include <fstream>

void ProcessThread::run()
{
	std::ofstream datafile(mSettings.filename);

	//Retirado do naghiloo thesis pg 38

	double dtau = (mSettings.finalTau - mSettings.initialTau) / (mSettings.nSteps - 1);
	double trueQFreq = 4.345;
	double delta, Omega;
	double  A = 1;
	double pop = 0;

	double tau = mSettings.initialTau;

	int ySteps = mSettings.nQFreqSteps;

	if (!mSettings.CheckQFreqIteration)
	{
		ySteps = 1;
		datafile << "Time Duration" << "," << "Population" << "\n";
	}
	else
	{
		datafile << "Time Duration" << "," << "Qubit Drive Frequency" << "," << "Population" << "\n";
	}

	double qFreq = mSettings.initialQFreq;

	double dFreq = (mSettings.finalQFreq - mSettings.initialQFreq)/(mSettings.nQFreqSteps-1);


	for (int j = 0; j < ySteps; j++)
	{
		
		{
			QMutexLocker locker(&m_mutex);
			if (m_stop) break;
		}

		delta = trueQFreq - qFreq;
		Omega = sqrt(A*A + delta * delta);


		tau = mSettings.initialTau;
		for (int i = 0; i < mSettings.nSteps; i++)
		{
			{
				QMutexLocker locker(&m_mutex);
				if (m_stop) break;
			}

			double prob = A * A / (Omega*Omega)*sin(Omega*tau / 2)*sin(Omega*tau / 2) * 100;
			
			
			pop = 0;
			for (int k = 0; k < mSettings.nIter; k++)
			{
				{
					QMutexLocker locker(&m_mutex);
					if (m_stop) break;
				}
				double coin = (rand() % 100);

				if (coin < prob)
				{
					pop += 1.0f;
				}
			}
			pop /= mSettings.nIter;
			

			tau += dtau;

			usleep(10000);

			if (!mSettings.CheckQFreqIteration)
			{
				datafile << tau << "," << pop << "\n";
			}
			else
			{
				datafile << tau << "," << qFreq << "," << pop << "\n";
			}

			emit signalDataPoint(i, j, pop);
		}
		qFreq += dFreq;
	}

	

	datafile.close();
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