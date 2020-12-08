#include "PiPulseQt.h"
#include <QtWidgets/QApplication>
#include "ProcessThread.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PiPulseQt w;
    w.show();

	ProcessThread t;
	QObject::connect(&t, SIGNAL(signalDataPoint(int, double, double)), &w, SLOT(receivedDataPoint(int, double, double)));
	QObject::connect(&w, SIGNAL(startMeasurement(MeasurementSetting)), &t, SLOT(loadAndStart(MeasurementSetting)));
	QObject::connect(&w, SIGNAL(stopMeasurement()), &t, SLOT(stop()));
	QObject::connect(&t, SIGNAL(finished()), &w, SLOT(finishedMeasurement()));

    return a.exec();
}
