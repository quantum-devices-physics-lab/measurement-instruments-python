#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_OneToneQt.h"
#include "datatype.h"

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
	MeasurementSetting settings;
	double max_y_value;
	QDoubleValidator *doubleValidator;
	QIntValidator *intValidator;

	
public slots:
	void receivedDataPoint(int,double,double);
	void finishedMeasurement();

private slots:
	void on_StartMeasurementButton_clicked();
	void on_StopMeasurementButton_clicked();
	void on_FreqStartEdit_editingFinished();
	void on_FreqStopEdit_editingFinished();
	void on_NStepsEdit_editingFinished();
	void on_LowPowerEdit_editingFinished();
	void on_HighPowerEdit_editingFinished();
	void on_CircuitResistanceEdit_editingFinished();
	void on_IFBandwidthEdit_editingFinished();
	void on_MeasurementNameEdit_editingFinished();


signals:
	void startMeasurement(MeasurementSetting);
	void stopMeasurement();
};
