#pragma once

#include <QtCore>
#include <QMutex>
#include "dataype.h"


class ProcessThread : public QThread
{
	Q_OBJECT


private:
	QMutex m_mutex;
	bool m_stop = false;
	MeasurementSetting mSettings;

protected:
	virtual void run();

public slots:
	void stop();
	void loadAndStart(MeasurementSetting);

signals:
	void signalDataPoint(int, double, double);
};