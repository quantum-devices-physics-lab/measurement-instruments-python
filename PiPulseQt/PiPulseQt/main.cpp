#include "PiPulseQt.h"
#include <QtWidgets/QApplication>
#include "ProcessThread.h"
#include <QtCore>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PiPulseQt w;
    w.show();

	ProcessThread t;



	QObject::connect(&t, SIGNAL(signalDataPoint(int,int,double)), &w, SLOT(receivedDataPoint(int,int,double)));
	QObject::connect(&w, SIGNAL(startMeasurement(MeasurementSetting)), &t, SLOT(loadAndStart(MeasurementSetting)));
	QObject::connect(&w, SIGNAL(stopMeasurement()), &t, SLOT(stop()));
	QObject::connect(&t, SIGNAL(finished()), &w, SLOT(finishedMeasurement()));
	

    return a.exec();
}
