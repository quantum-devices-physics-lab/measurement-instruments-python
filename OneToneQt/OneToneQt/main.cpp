#include "OneToneQt.h"
#include <QtWidgets/QApplication>
#include "PythonThread.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);	
    OneToneQt w;
    w.show();

	
	qDebug("Thread id %d", (int)QThread::currentThreadId());
	
	PythonThread t;
	QObject::connect(&t, SIGNAL(signalDataPoint(double)), &w, SLOT(receivedDataPoint( double)));
	QObject::connect(&w, SIGNAL(startMeasurement()), &t, SLOT(start()));
	QObject::connect(&w, SIGNAL(stopMeasurement()), &t, SLOT(stop()));
	QObject::connect(&t, SIGNAL(finished()), &w, SLOT(finishedMeasurement()));

    return a.exec();
}
