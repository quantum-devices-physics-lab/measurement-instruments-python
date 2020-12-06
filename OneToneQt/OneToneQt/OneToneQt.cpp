#include "OneToneQt.h"
#include <Qtcore>

OneToneQt::OneToneQt(QWidget *parent)
    : QMainWindow(parent)
	, xlow(10)
	, ylow(10)
	, xhigh(10)
	, yhigh(10)
{
    ui.setupUi(this);
	running = false;

	indexlow = 0;
	indexhigh = 0;

	for (int i = 0; i < 10; i++)
	{
		xlow[i] = 0;
		ylow[i] = 0;
		xhigh[i] = 0;
		yhigh[i] = 0;
	}

	ui.DynamicPlotWidget->addGraph();
	ui.DynamicPlotWidget->graph(0)->setPen(QColor(0,0, 255, 255));
	ui.DynamicPlotWidget->graph(0)->setLineStyle(QCPGraph::lsNone);
	ui.DynamicPlotWidget->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
	

	ui.DynamicPlotWidget->addGraph();
	ui.DynamicPlotWidget->graph(1)->setPen(QColor(255, 0, 0, 255));
	ui.DynamicPlotWidget->graph(1)->setLineStyle(QCPGraph::lsNone);
	ui.DynamicPlotWidget->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 4));
}

void OneToneQt::on_StartMeasurementButton_clicked()
{

	qDebug("Thread id inside on_StartMeasurementButton_clicked %d", (int)QThread::currentThreadId());
	if (!running)
	{

		ui.DynamicPlotWidget->graph(0)->setData(xlow, ylow);
		ui.DynamicPlotWidget->graph(1)->setData(xhigh, yhigh);
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



void OneToneQt::receivedDataPoint(int graph,double datax,double datay) 
{
	qDebug("Thread id inside receivedDataPoint %d", (int)QThread::currentThreadId());
	qDebug("data point received %f", datax);
	ui.measurementStatusLabel->setText("Acquiring data");
	if (graph == 0)
	{
		xlow[indexlow] = datax;
		ylow[indexlow] = datay;
		indexlow++;
		ui.DynamicPlotWidget->graph(0)->setData(xlow, ylow);
	}
	else if(graph == 1) {
		xhigh[indexhigh] = datax;
		yhigh[indexhigh] = datay;
		indexhigh++;
		ui.DynamicPlotWidget->graph(1)->setData(xhigh, yhigh);
	}
	
	ui.DynamicPlotWidget->replot();

}

void OneToneQt::finishedMeasurement()
{
	qDebug("Finished Measurement %d", (int)QThread::currentThreadId());
	ui.StartMeasurementButton->setEnabled(true);
	ui.StopMeasurementButton->setEnabled(false);
	indexlow = 0;
	indexhigh = 0;
	for (int i = 0; i < 10; i++)
	{
		xlow[i] = 0;
		ylow[i] = 0;
		xhigh[i] = 0;
		yhigh[i] = 0;
	}

	running = false;
	ui.measurementStatusLabel->setText("Waiting Measurement to Start");
	
}