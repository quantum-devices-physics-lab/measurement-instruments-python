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
	
public slots:
	void receivedDataPoint(double);

private slots:
	void on_ControlMeasurementButton_clicked();
};
