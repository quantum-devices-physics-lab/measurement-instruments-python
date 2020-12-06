#include "OneToneQt.h"
#include <Qtcore>

OneToneQt::OneToneQt(QWidget *parent)
    : QMainWindow(parent)
	, x(10)
	, y(10)
{
    ui.setupUi(this);
	running = false;

	index = 0;

	for (int i = 0; i < 10; i++)
	{
		x[i] = 0;
		y[i] = 0;
	}

	ui.DynamicPlotWidget->addGraph();
}

void OneToneQt::on_StartMeasurementButton_clicked()
{

	qDebug("Thread id inside on_StartMeasurementButton_clicked %d", (int)QThread::currentThreadId());
	if (!running)
	{
		for (int i = 0; i < 10; i++)
		{
			x[i] = 0;
			y[i] = 0;
		}
		ui.DynamicPlotWidget->graph(0)->setData(x, y);
		ui.DynamicPlotWidget->replot();

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



void OneToneQt::receivedDataPoint(double datax,double datay) 
{
	qDebug("Thread id inside receivedDataPoint %d", (int)QThread::currentThreadId());
	qDebug("data point received %f", datax);
	ui.measurementStatusLabel->setText("Acquiring data");
	x[index] = datax;
	y[index] = datay;
	index++;
	ui.DynamicPlotWidget->graph(0)->setData(x,y);
	ui.DynamicPlotWidget->replot();

}

void OneToneQt::finishedMeasurement()
{
	qDebug("Finished Measurement %d", (int)QThread::currentThreadId());
	ui.StartMeasurementButton->setEnabled(true);
	ui.StopMeasurementButton->setEnabled(false);
	index = 0;
	running = false;
	ui.measurementStatusLabel->setText("Waiting Measurement to Start");
	
}