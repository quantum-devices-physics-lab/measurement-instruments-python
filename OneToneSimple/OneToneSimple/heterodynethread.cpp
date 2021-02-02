#include "heterodynethread.h"
#include <qmath.h>
#include <random>
#include <toml.hpp>

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

void HeterodyneThread::simulate()
{

	double lower_bound = -0.0001;
	double upper_bound = 0.0001;
	std::uniform_real_distribution<double> unif(lower_bound, upper_bound);
	std::uniform_real_distribution<double> unif_dut(lower_bound * 10, upper_bound * 100);
	std::default_random_engine re;

	

	int N = int(heterodyneSettings.sampleRate*heterodyneSettings.timeRange);
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


	for (double freq = heterodyneSettings.startFrequency; freq < heterodyneSettings.stopFrequency; freq += (heterodyneSettings.stopFrequency- heterodyneSettings.startFrequency)/heterodyneSettings.nSteps )
	{
		I = 0;
		Q = 0;

		for (int i = 0; i < N; i++)
		{
			{
				QMutexLocker locker(&m_mutex);
				if (m_stop) break;
			}


			wave(0.001 + unif(re), freq, 0, t, sourceI, N);
			wave(0.001 + unif(re), freq, 3.1415 / 2, t, sourceQ, N);
			wave(lorentzian(1, freq, 3, 0.1) + unif_dut(re), freq, 0, t, dut, N);

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

void HeterodyneThread::execute()
{

}


void HeterodyneThread::run()
{
	if (heterodyneSettings.isSimulation)
	{
		emit signalLog("Simulation");
		simulate();
	}
	else
	{
		emit signalLog("Hardware");
		execute();
	}
	
}

void HeterodyneThread::stop()
{
	qDebug("Thread::stop called from main thread: %d", currentThreadId());
	QMutexLocker locker(&m_mutex);
	m_stop = true;
}

HeterodyneThread::HeterodyneThread(std::string dir)
{

	qDebug("Thread::stop called from main thread: %d", currentThreadId());
	qDebug(dir.c_str());


	const auto data = toml::parse(dir.c_str());
	
	const auto& instruments = toml::find(data, "settings","instruments");

	const auto toSimulate = toml::find<bool>(data, "settings", "simulation");
	heterodyneSettings.isSimulation = toSimulate;

	const auto filename = toml::find<std::string>(data, "settings", "savefile_name");
	heterodyneSettings.filename = filename;

	const auto att_address = toml::find<std::string>(instruments, "Attenuator");
	const auto source1_address = toml::find<std::string>(instruments, "Source1");
	const auto source2_address = toml::find<std::string>(instruments, "Source2");
	const auto osc_address = toml::find<std::string>(instruments, "Oscilloscope");
	heterodyneSettings.Source1Address = source1_address;
	heterodyneSettings.Source2Address = source2_address;
	heterodyneSettings.AttenuatorAddress = att_address;
	heterodyneSettings.OscilloscopeAddress = osc_address;

	const auto Channel_I_ref = toml::find<int>(instruments, "Channel_I_ref");
	heterodyneSettings.ChannelI = Channel_I_ref;

	const auto Channel_Q_ref = toml::find<int>(instruments, "Channel_Q_ref");
	heterodyneSettings.ChannelQ = Channel_Q_ref;

	const auto Channel_DUT_signal = toml::find<int>(instruments,"Channel_DUT_signal");
	heterodyneSettings.ChannelSignal = Channel_DUT_signal;


	const auto& experiment = toml::find(data, "experiment");

	const auto frequencies = toml::find<std::vector<double>>(experiment, "frequencies");
	heterodyneSettings.startFrequency = frequencies[0];
	heterodyneSettings.stopFrequency = frequencies[1];

	const auto frequency_nsteps = toml::find<int>(experiment, "frequency_nsteps");
	heterodyneSettings.nSteps = frequency_nsteps;

	const auto attenuation = toml::find<int>(experiment, "attenuation");
	heterodyneSettings.attenuation = attenuation;

	const auto averages = toml::find<int>(experiment, "averages");
	heterodyneSettings.averages = averages;

	const auto timeRange = toml::find<double>(experiment, "time_range");
	heterodyneSettings.timeRange = timeRange;

	const auto sampleRate = toml::find<double>(experiment, "sample_rate");
	heterodyneSettings.sampleRate = sampleRate;

	const auto ifFreq = toml::find<double>(experiment, "if_frequency");
	heterodyneSettings.ifFrequency = ifFreq;

	const auto source1Amp = toml::find<int>(experiment, "source1_amp");
	heterodyneSettings.source1Amp = source1Amp;

	const auto source2Amp = toml::find<int>(experiment, "source2_amp");
	heterodyneSettings.source2Amp = source2Amp;


}