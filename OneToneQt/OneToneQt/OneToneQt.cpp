#include "OneToneQt.h"
#include <Qtcore>

OneToneQt::OneToneQt(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	running = false;
}

void OneToneQt::on_ControlMeasurementButton_clicked()
{
	qDebug("Thread id inside on_ControlMeasurementButton_clicked %d", (int)QThread::currentThreadId());
	if (running)
	{
		ui.ControlMeasurementButton->setText("Stop");
		running = false;
	} 
	else 
	{
		ui.ControlMeasurementButton->setText("Start");
		running = true;
	}
	
}


void OneToneQt::receivedDataPoint(double data) 
{
	qDebug("Thread id inside receivedDataPoint %d", (int)QThread::currentThreadId());
	qDebug("data point received %f", data);
}