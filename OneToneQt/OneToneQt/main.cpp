#include "OneToneQt.h"
#include <QtWidgets/QApplication>
#include <QtCore>
#include "PythonThread.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OneToneQt w;
    w.show();

	PythonThread t;
	qDebug("Thread id %d", (int)QThread::currentThreadId());
	QObject::connect(w.ui.ControlMeasurementButton, SIGNAL(clicked()), &t, SLOT(start()));
	QObject::connect(&t, SIGNAL(signalDataPoint(double)), &w, SLOT(receivedDataPoint( double)));

    return a.exec();
}
