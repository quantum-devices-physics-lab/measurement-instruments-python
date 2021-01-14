
#define _USE_MATH_DEFINES
#include "OneToneQt.h"
#include <QtWidgets/QApplication>
#include "PythonThread.h"
#include "datatype.h"
#include "visa.h"


int main(int argc, char *argv[])
{
	int error;
	ViSession session, vi;

	ViChar* resrcDefault = "TCPIP0::169.254.101.103::inst0::INSTR";
	ViChar* resrc = resrcDefault;
	error = viOpenDefaultRM(&session);
	error = viOpen(session, resrc, VI_NO_LOCK, 10000, &vi);
	if (error != VI_SUCCESS)
	{
		viClose(session);
		qDebug("Visa session failed");
		return 1;
	}

	// Query the instrument identity
	error = viPrintf(vi, "*IDN?\n");
	ViChar buffer[5000];
	error = viScanf(vi, "%t", buffer);
	qDebug("*IDN? -> %s", buffer);

	// Ask for frequency
	error = viPrintf(vi, ":FREQ?\n");
	error = viScanf(vi, "%t", buffer);
	qDebug(":FREQ? -> %s", buffer);

	// Set frequency
	error = viPrintf(vi, ":FREQ +1.5E+10\n");
	

	error = viClose(vi);
	error = viClose(session);



    QApplication a(argc, argv);	
    OneToneQt w;
    w.show();

	
	qDebug("Thread id %d", (int)QThread::currentThreadId());
	
	PythonThread t;
	QObject::connect(&t, SIGNAL(signalDataPoint(int, double,double)), &w, SLOT(receivedDataPoint(int, double,double)));
	QObject::connect(&w, SIGNAL(startMeasurement(MeasurementSetting)), &t, SLOT(loadAndStart(MeasurementSetting)));
	QObject::connect(&w, SIGNAL(stopMeasurement()), &t, SLOT(stop()));
	QObject::connect(&t, SIGNAL(finished()), &w, SLOT(finishedMeasurement()));

    return a.exec();
}
