#pragma once

#include <QtCore>
#include <QMutex>
#include "datatype.h"
#include <fstream>
#include "visa.h"


class PythonThread : public QThread
{
	Q_OBJECT


private:
	QMutex m_mutex;
	bool m_stop = false;
	MeasurementSetting PythonSettings;

protected:
	virtual void run();

public slots:
	void stop();
	void loadAndStart(MeasurementSetting);

signals:
	void signalDataPoint(int,double,double);
};