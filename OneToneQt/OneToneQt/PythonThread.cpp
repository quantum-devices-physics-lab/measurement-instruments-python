#include "PythonThread.h"
#include <math.h>
#include <sstream>
#include <string>

double lorentzian(double A, double freq, double res_freq, double sigma)
{
	return (sigma / 2) / ((freq - res_freq)*(freq - res_freq) + (sigma / 2)*(sigma / 2))/3.1415;
}

int capture_waveform(ViSession viOsc, char *buffer, double *points, int channel, int nPoints)
{
	int error;


	error = viPrintf(viOsc, ":WAV:FORMAT WORD\n");
	error = viPrintf(viOsc, ":WAV:SOUR CHAN%d, \n", channel);

	error = viPrintf(viOsc, "WAVEFORM:YINCREMENT?\n");
	error = viScanf(viOsc, "%t", buffer);
	qDebug("yincrement -> %s", buffer);

	std::string s = buffer;
	std::istringstream os(s);
	double yincrement;
	os >> yincrement;

	error = viPrintf(viOsc, "WAVEFORM:YORIGIN?\n");
	error = viScanf(viOsc, "%t", buffer);
	qDebug("yorigin -> %s", buffer);

	std::string s2 = buffer;
	std::istringstream os2(s2);
	double yorigin;
	os2 >> yorigin;


	error = viPrintf(viOsc, ":WAV:DATA? 1, \n");	
	error = viScanf(viOsc, "%t", buffer);

	int numberHead = (int)(buffer[1] - 0x30);
	int numberOfBytes = 0;

	for (int i = 2; i < numberHead + 2; i++)
	{
		numberOfBytes += (int)(buffer[i] - 0x30) * pow(10, numberHead -1 - i + 2);
	}

	qDebug("*buffer -> %s", buffer);
	qDebug("*buffer -> %c", buffer[0]);
	qDebug("*buffer -> %c", buffer[1]);



	char numberOfBytesPerPoint = 2;
	int n_points = numberOfBytes / numberOfBytesPerPoint;



	for (int i = 0; i < n_points; i++)
	{
		unsigned int byte1 = buffer[2 + numberHead + numberOfBytesPerPoint * i];
		unsigned int byte2 = buffer[2 + numberHead + 1 + numberOfBytesPerPoint * i];

		if (((0xff << 24) & byte2) >> 24 == 255)
			byte2 += 0x00000100;

		points[i] = ((signed int)((byte1 << 8) | byte2))*yincrement+ yorigin;
	}

	return 0;
}

double VtodBm(double V)
{
	auto c = 10 * log10(20);
	return 20 * log10(V) + c;
}


void PythonThread::run()
{

	qDebug("Thread id inside run %d", (int)QThread::currentThreadId());

	int error;

	ViSession session, viPSG1, viPSG2, viAttenuator, viOsc;
	char *buffer = new char[5000];

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

	error = viPrintf(viPSG1, ":OUTP 0\n");
	error = viPrintf(viPSG2, ":OUTP 0\n");

	error = viPrintf(viPSG1, ":OUTP:MOD 0\n");
	error = viPrintf(viPSG2, ":OUTP:MOD 0\n");

	error = viPrintf(viPSG1, ":UNIT:POW DBM\n");
	error = viPrintf(viPSG2, ":UNIT:POW DBM\n");

	error = viPrintf(viPSG1, ":SOUR:POW:LEV:IMM:AMPL 16\n");
	error = viPrintf(viPSG2, ":SOUR:POW:LEV:IMM:AMPL 1\n");

	error = viPrintf(viOsc, ":STOP\n");
	error = viPrintf(viOsc, ":TIM:RANG %fE-6\n",PythonSettings.timeRange);
	error = viPrintf(viOsc, ":ACQ:SRAT:ANAL %fE+6\n",PythonSettings.sampleRate);
	error = viPrintf(viOsc, ":SINGLE\n");
	sleep(5);
	error = viPrintf(viOsc, ":WAV:POIN?\n");
	error = viScanf(viOsc, "%t", buffer);

	
	std::string str = buffer;
	int nPoints = std::stoi(str);

	delete[] buffer;
	buffer = new char[2 * nPoints + 1000];


	double *Y1 = new double[nPoints];
	double *Y2 = new double[nPoints];
	double *Y3 = new double[nPoints];

	
	
	

	std::ofstream datafile(PythonSettings.filename);

	datafile << "---------------" << "Low Power" << "---------------" << "\n";

	datafile << "Frequency" << "," << "I" << "," << "Q" << "\n";

	const double dfreq = (PythonSettings.stopFrequency - PythonSettings.startFrequency) / (PythonSettings.nSteps-1);

	const double ifFreq = PythonSettings.ifFrequency*1e-3;


	double I = 0;
	double Q = 0;
	
	error = viPrintf(viPSG1, ":OUTP 1\n");
	error = viPrintf(viPSG2, ":OUTP 1\n");

	//low power
	for (double freq = PythonSettings.startFrequency; freq <= PythonSettings.stopFrequency; freq+= dfreq)
	{
		{
			QMutexLocker locker(&m_mutex);
			if (m_stop) break;
		}

		I = 0;
		Q = 0;


		for (int j = 0; j < PythonSettings.averages; j++)
		{

			error = viPrintf(viPSG1, ":FREQ %fE+9\n", freq+ ifFreq);
			error = viPrintf(viPSG2, ":FREQ %fE+9\n", freq);

			usleep(100000);

			error = viPrintf(viOsc, ":SINGLE\n");

			usleep(100000);



			capture_waveform(viOsc, buffer, Y1, 1, nPoints);
			capture_waveform(viOsc, buffer, Y2, 2, nPoints);
			capture_waveform(viOsc, buffer, Y3, 3, nPoints);



			double aI = 0;
			double aQ = 0;
			for (int i = 0; i < nPoints; i++)
			{
				aI += Y1[i] * Y2[i] / nPoints;
				aQ += Y3[i] * Y2[i] / nPoints;
			}
			I += aI / PythonSettings.averages;
			Q += aQ / PythonSettings.averages;
		}


		double result = VtodBm(4 * sqrt(I*I + Q * Q));


		datafile << freq << "," << I <<',' << Q << "\n";

		emit signalDataPoint(0,freq, result);
	}




	datafile << "---------------" << "High Power" << "---------------" << "\n";

	datafile << "Frequency" << "," << "I" << "," << "Q" << "\n";
	/*
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
	*/

	delete[] Y1;
	delete[] Y2;
	delete[] Y3;
	delete[] buffer;

	error = viPrintf(viPSG1, ":OUTPUT 0\n");
	error = viPrintf(viPSG2, ":OUTPUT 0\n");

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