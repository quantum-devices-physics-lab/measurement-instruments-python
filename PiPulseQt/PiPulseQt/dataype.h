#pragma once


struct MeasurementSetting {
	double initialTau; // Initial Time duration
	double finalTau;  // Final Time Duration
	int nSteps; // Number of time durations
	int nIter; // Number of iterations to each time duration
	double qFreq; // qubit Frequency
	double cFreq; // cavity Frequnecy
	double potency; // Pulse potency
};