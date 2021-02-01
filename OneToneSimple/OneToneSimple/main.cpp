#include "onetonesimple.h"
#include <QtWidgets/QApplication>
#include "heterodynethread.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	OneToneSimple w;
	w.show();

	bool ok;
	QString dirText = QInputDialog::getText(0, "Experiment Directory",
		"Directory:", QLineEdit::Normal,
		QDir::home().dirName(), &ok);
	
	if(ok) {

		qDebug("Thread id %d", (int)QThread::currentThreadId());
		
		HeterodyneThread t(dirText.toUtf8().toStdString());
		QObject::connect(&t, SIGNAL(signalDataPoint(double, double)), &w, SLOT(receivedDataPoint(double, double)));
		QObject::connect(&t, SIGNAL(signalLog(char*)), &w, SLOT(receivedLog(char*)));
		QObject::connect(&w, SIGNAL(stopMeasurement()), &t, SLOT(stop()));

		t.start();
		return a.exec();
	}

	return 0;
}
