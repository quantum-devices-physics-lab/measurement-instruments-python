#include "PythonThread.h"


double lorentzian(double A, double freq, double res_freq, double sigma)
{
	return (sigma / 2) / ((freq - res_freq)*(freq - res_freq) + (sigma / 2)*(sigma / 2))/3.1415;
}


void PythonThread::run()
{

	qDebug("Thread id inside run %d", (int)QThread::currentThreadId());

	int error;

	ViSession session, viPSG1, viPSG2, viAttenuator, viOsc;
	ViChar buffer[5000];

	qDebug("PSG1 address: %s", PythonSettings.Source1Address.c_str());
	qDebug("PSG2 address: %s", PythonSettings.Source2Address.c_str());
	qDebug("Attenuator address: %s", PythonSettings.AttenuatorAddress.c_str());
	qDebug("Oscilloscope address: %s", PythonSettings.OscilloscopeAddress.c_str());

	error = viOpenDefaultRM(&session);
	if (error != VI_SUCCESS)
	{
		qDebug("Error in locating resources");
	}

	error = viOpen(session, PythonSettings.Source1Address.c_str(), VI_NO_LOCK, 10000, &viPSG1);
	if (error != VI_SUCCESS)
	{
		qDebug("Error in opening PSG1");
	}

	error = viOpen(session, PythonSettings.Source2Address.c_str(), VI_NO_LOCK, 10000, &viPSG2);
	if (error != VI_SUCCESS)
	{
		qDebug("Error in opening PSG2");
	}

	error = viOpen(session, PythonSettings.AttenuatorAddress.c_str(), VI_NO_LOCK, 10000, &viAttenuator);
	if (error != VI_SUCCESS)
	{
		qDebug("Error in opening Attenuator");
	}

	error = viOpen(session, PythonSettings.OscilloscopeAddress.c_str(), VI_NO_LOCK, 10000, &viOsc);
	if (error != VI_SUCCESS)
	{
		qDebug("Error in opening Oscilloscope");
	}

	error = viPrintf(viPSG1, "*IDN?\n");
	error = viScanf(viPSG1, "%t", buffer);
	qDebug("*IDN? -> %s", buffer);


	error = viPrintf(viPSG2, "*IDN?\n");
	error = viScanf(viPSG2, "%t", buffer);
	qDebug("*IDN? -> %s", buffer);


	error = viPrintf(viAttenuator, "*IDN?\n");
	error = viScanf(viAttenuator, "%t", buffer);
	qDebug("*IDN? -> %s", buffer);


	error = viPrintf(viOsc, "*IDN?\n");
	error = viScanf(viOsc, "%t", buffer);
	qDebug("*IDN? -> %s", buffer);



	std::ofstream datafile(PythonSettings.filename);

	datafile << "---------------" << "Low Power" << "---------------" << "\n";

	datafile << "Frequency" << "," << "Amplitude" << "\n";

	double dfreq = (PythonSettings.stopFrequency - PythonSettings.startFrequency) / (PythonSettings.nSteps-1);
	

	double half_freq = (PythonSettings.stopFrequency + PythonSettings.startFrequency) / 2;


	//low power
	for (double freq = PythonSettings.startFrequency; freq <= PythonSettings.stopFrequency; freq+= dfreq)
	{
		{
			QMutexLocker locker(&m_mutex);
			if (m_stop) break;
		}

		double result = lorentzian(10,freq, half_freq - 0.05f, 0.01f);

		usleep(10000);

		datafile << freq << "," << result << "\n";

		emit signalDataPoint(0,freq, result);
	}


	datafile << "---------------" << "High Power" << "---------------" << "\n";
	datafile << "Frequency" << "," << "Amplitude" << "\n";

	//high power
	for (double freq = PythonSettings.startFrequency; freq <= PythonSettings.stopFrequency; freq += dfreq)
	{
		{
			QMutexLocker locker(&m_mutex);
			if (m_stop) break;
		}

		double result = lorentzian(10, freq, half_freq, 0.01f);

		usleep(10000);

		datafile << freq << "," << result << "\n";

		emit signalDataPoint(1, freq, result);
	}

	datafile.close();

	viClose(viPSG1);
	viClose(viPSG2);
	viClose(viAttenuator);
	viClose(viOsc);
	viClose(session);
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