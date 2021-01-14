#pragma once


struct MeasurementSetting {
	double startFrequency;
	double stopFrequency;
	int nSteps;
	int lowPowerAttenuation;
	int highPowerAttenuation;
	double ifBandwidth;
	double circuitResistance;
	std::string filename;
	std::string Source1Address;
	std::string Source2Address;
	std::string AttenuatorAddress;
	std::string OscilloscopeAddress;
};