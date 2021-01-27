#include "heterodynethread.h"
#include <qmath.h>

double lorentzian(double A, double freq, double res_freq, double sigma)
{
	return (sigma / 2) / ((freq - res_freq)*(freq - res_freq) + (sigma / 2)*(sigma / 2)) / 3.1415;
}

void wave(double A, double freq, double fase, double* t, double* wavepoints, int N)
{

	for (int i = 0; i < N; i++)
	{
		wavepoints[i] = A * cos(freq*t[i] + fase);
	}

}

double VtodBm(double V)
{
	auto c = 10 * log10(20);
	return 20 * log10(V) + c;
}


void HeterodyneThread::run()
{
	heterodyneSettings.attenuation = 10;

	emit signalLog("test");

	int N = 1000;
	double* t = new double[N];
	double* sourceI = new double[N];
	double* sourceQ = new double[N];
	double* dut = new double[N];

	for (int i = 0; i < N; i++)
	{
		{
			QMutexLocker locker(&m_mutex);
			if (m_stop) break;
		}

		t[i] = 0.1*i;
	}
	double I, Q;

	for (double freq = 1.0; freq < 5.0; freq += 0.01)
	{
		I = 0;
		Q = 0;

		for (int i = 0; i < N; i++)
		{
			{
				QMutexLocker locker(&m_mutex);
				if (m_stop) break;
			}

			wave(0.001, freq, 0, t, sourceI, N);
			wave(0.001, freq, 3.1415/2, t, sourceQ, N);
			wave(lorentzian(1,freq,3,0.1), freq, 0, t, dut, N);
		}

		
		for (int i = 0; i < N; i++)
		{
			{
				QMutexLocker locker(&m_mutex);
				if (m_stop) break;
			}

			I += sourceI[i] * dut[i] / N;
			Q += sourceQ[i] * dut[i] / N;
		}

		double result = VtodBm(4 * sqrt(I*I + Q * Q));
		emit signalDataPoint(freq, result);

	}

	delete t;
	delete sourceI;
	delete sourceQ;
	delete dut;

}

void HeterodyneThread::stop()
{
	qDebug("Thread::stop called from main thread: %d", currentThreadId());
	QMutexLocker locker(&m_mutex);
	m_stop = true;
}