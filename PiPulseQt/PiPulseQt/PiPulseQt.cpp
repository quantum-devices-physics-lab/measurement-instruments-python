#include "PiPulseQt.h"

PiPulseQt::PiPulseQt(QWidget *parent)
	: QMainWindow(parent)
	, x(100)
	, y(100)
{
	ui.setupUi(this);
	running = false;


	settings.initialTau = 0;
	settings.finalTau = 10;
	settings.nSteps = 100;
	settings.cFreq = 4;
	settings.qFreq = 5;
	settings.nIter = 1000;
	settings.potency = 7;

	ui.initialTauEdit->setText(QString::number(settings.initialTau));
	ui.finalTauEdit->setText(QString::number(settings.finalTau));
	ui.nStepsEdit->setText(QString::number(settings.nSteps));
	ui.cavityFreqEdit->setText(QString::number(settings.cFreq));
	ui.qubitFreqEdit->setText(QString::number(settings.qFreq));
	ui.nIterEdit->setText(QString::number(settings.nIter));
	ui.potencyEdit->setText(QString::number(settings.potency));

	index = 0;

	for (int i = 0; i < settings.nSteps; i++)
	{
		x[i] = 0;
		y[i] = 0;
	}

	ui.DynamicPlotWidget->xAxis->setRange(settings.initialTau, settings.finalTau);
	ui.DynamicPlotWidget->yAxis->setRange(0, 1);

	ui.DynamicPlotWidget->addGraph();
	ui.DynamicPlotWidget->graph(0)->setPen(QColor(0, 0, 255, 255));
	ui.DynamicPlotWidget->graph(0)->setLineStyle(QCPGraph::lsNone);
	ui.DynamicPlotWidget->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
}



void PiPulseQt::on_initialTauEdit_editingFinished()
{
	settings.initialTau = ui.initialTauEdit->text().toDouble();
	ui.DynamicPlotWidget->xAxis->setRange(settings.initialTau, settings.finalTau);
}

void PiPulseQt::on_finalTauEdit_editingFinished()
{
	settings.finalTau = ui.finalTauEdit->text().toDouble();
	ui.DynamicPlotWidget->xAxis->setRange(settings.initialTau, settings.finalTau);
}

void PiPulseQt::on_nStepsEdit_editingFinished()
{
	settings.nSteps = ui.nStepsEdit->text().toInt();
	x.resize(settings.nSteps);
	y.resize(settings.nSteps);
}

void PiPulseQt::on_cavityFreqEdit_editingFinished()
{
	settings.cFreq = ui.cavityFreqEdit->text().toDouble();
}

void PiPulseQt::on_qubitFreqEdit_editingFinished()
{
	settings.qFreq = ui.qubitFreqEdit->text().toDouble();
}

void PiPulseQt::on_nIterEdit_editingFinished()
{
	settings.nIter = ui.nIterEdit->text().toInt();
}

void PiPulseQt::on_potencyEdit_editingFinished()
{
	settings.potency = ui.potencyEdit->text().toDouble();
}

void PiPulseQt::on_StartMeasurementButton_clicked()
{

	if (!running)
	{

		ui.DynamicPlotWidget->graph(0)->setData(x, y);
		ui.DynamicPlotWidget->replot();

		ui.StartMeasurementButton->setEnabled(false);
		ui.StopMeasurementButton->setEnabled(true);
		running = true;
		emit startMeasurement(settings);
	}

}

void PiPulseQt::on_StopMeasurementButton_clicked()
{
	if (running)
	{
		ui.StartMeasurementButton->setEnabled(true);
		ui.StopMeasurementButton->setEnabled(false);
		running = false;
		emit stopMeasurement();
	}

}



void PiPulseQt::receivedDataPoint(double datax, double datay)
{
	qDebug("Thread id inside receivedDataPoint %d", (int)QThread::currentThreadId());
	qDebug("tau %f amp %f",datax, datay);

	ui.notificationLabel->setText("Acquiring data");
	qDebug("index %d", index);
	x[index] = datax;
	y[index] = datay;
	index++;
	ui.DynamicPlotWidget->graph(0)->setData(x, y);
	ui.DynamicPlotWidget->replot();

}

void PiPulseQt::finishedMeasurement()
{
	qDebug("Finished Measurement %d", (int)QThread::currentThreadId());
	ui.StartMeasurementButton->setEnabled(true);
	ui.StopMeasurementButton->setEnabled(false);
	index = 0;
	for (int i = 0; i < settings.nSteps; i++)
	{
		x[i] = 0;
		y[i] = 0;
	}

	running = false;
	ui.notificationLabel->setText("Waiting Measurement to Start");

}