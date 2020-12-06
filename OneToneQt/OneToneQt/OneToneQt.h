#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_OneToneQt.h"

class OneToneQt : public QMainWindow
{
    Q_OBJECT

public:
    OneToneQt(QWidget *parent = Q_NULLPTR);
	Ui::OneToneQtClass ui;




private:
	bool running;
	QVector<double> xlow, ylow,xhigh,yhigh;
	int indexlow,indexhigh;
	
public slots:
	void receivedDataPoint(int,double,double);
	void finishedMeasurement();

private slots:
	void on_StartMeasurementButton_clicked();
	void on_StopMeasurementButton_clicked();

signals:
	void startMeasurement();
	void stopMeasurement();
};
