#pragma once

#include <QThread>
#include <QMutex>

class PythonThread : public QThread
{
	Q_OBJECT

protected:
	virtual void run();

signals:
	void signalDataPoint(double);
};