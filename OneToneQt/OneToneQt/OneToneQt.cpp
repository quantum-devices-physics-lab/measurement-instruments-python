#include "OneToneQt.h"
#include <Qtcore>

OneToneQt::OneToneQt(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	running = false;
}

void OneToneQt::on_StartMeasurementButton_clicked()
{
	qDebug("Thread id inside on_StartMeasurementButton_clicked %d", (int)QThread::currentThreadId());
	if (!running)
	{
		ui.StartMeasurementButton->setEnabled(false);
		ui.StopMeasurementButton->setEnabled(true);
		running = true;
		emit startMeasurement();
	}
	
}

void OneToneQt::on_StopMeasurementButton_clicked()
{
	qDebug("Thread id inside on_StopMeasurementButton_clicked %d", (int)QThread::currentThreadId());
	if (running)
	{
		ui.StartMeasurementButton->setEnabled(true);
		ui.StopMeasurementButton->setEnabled(false);
		running = false;
		emit stopMeasurement();
	}

}



void OneToneQt::receivedDataPoint(double data) 
{
	qDebug("Thread id inside receivedDataPoint %d", (int)QThread::currentThreadId());
	qDebug("data point received %f", data);
	ui.measurementStatusLabel->setText("Acquiring data");
}

void OneToneQt::finishedMeasurement()
{
	qDebug("Finished Measurement %d", (int)QThread::currentThreadId());
	ui.StartMeasurementButton->setEnabled(true);
	ui.StopMeasurementButton->setEnabled(false);
	running = false;
	ui.measurementStatusLabel->setText("Waiting Measurement to Start");
	
}