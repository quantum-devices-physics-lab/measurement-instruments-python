#pragma once


struct MeasurementSetting {
	double startFrequency;
	double stopFrequency;
	int nSteps;
	int lowPowerAttenuation;
	int highPowerAttenuation;
	double ifBandwidth;
	double circuitResistance;
};