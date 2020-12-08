#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PiPulseQt.h"
#include "dataype.h"

class PiPulseQt : public QMainWindow
{
    Q_OBJECT

public:
    PiPulseQt(QWidget *parent = Q_NULLPTR);
	Ui::PiPulseQtClass ui;
 
private:
	bool running;
	QVector<double> x, y;
	int index;
	MeasurementSetting settings;


public slots:
	void receivedDataPoint(double, double);
	void finishedMeasurement();

private slots:
	void on_StartMeasurementButton_clicked();
	void on_StopMeasurementButton_clicked();
	void on_initialTauEdit_editingFinished();
	void on_finalTauEdit_editingFinished();
	void on_nStepsEdit_editingFinished();
	void on_cavityFreqEdit_editingFinished();
	void on_qubitFreqEdit_editingFinished();
	void on_nIterEdit_editingFinished();
	void on_potencyEdit_editingFinished();


signals:
	void startMeasurement(MeasurementSetting);
	void stopMeasurement();

};
