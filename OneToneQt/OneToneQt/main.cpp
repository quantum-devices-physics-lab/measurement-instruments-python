
#define _USE_MATH_DEFINES
#include "OneToneQt.h"
#include <QtWidgets/QApplication>
#include "PythonThread.h"
#include "datatype.h"



int main(int argc, char *argv[])
{



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
