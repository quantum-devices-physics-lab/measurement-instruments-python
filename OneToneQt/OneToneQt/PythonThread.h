#pragma once

#include <QtCore>
#include <QMutex>


class PythonThread : public QThread
{
	Q_OBJECT


private:
	QMutex m_mutex;
	bool m_stop = false;

protected:
	virtual void run();

public slots:
	void stop();

signals:
	void signalDataPoint(int,double,double);
};