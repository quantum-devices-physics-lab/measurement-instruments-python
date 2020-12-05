#include "PythonThread.h"


void PythonThread::run()
{
	qDebug("Thread id inside run %d", (int)QThread::currentThreadId());
	static double data = 0.0f;
	data += 0.1f;
	qDebug("data point sent %f", data);
	emit signalDataPoint(data);
}