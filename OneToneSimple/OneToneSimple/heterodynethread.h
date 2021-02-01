#pragma once

#include <QtCore>
#include <QMutex>
#include "onetonedatamodel.h"
#include <fstream>


class HeterodyneThread : public QThread
{
	Q_OBJECT


private:
	QMutex m_mutex;
	bool m_stop = false;
	MeasurementSetting heterodyneSettings;
	void simulate();

protected:
	virtual void run();

public slots:
	void stop();

signals:
	void signalDataPoint(double, double);
	void signalLog(char*);
};
