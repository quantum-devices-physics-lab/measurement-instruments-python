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
	settings.initialQFreq = 5;
	settings.finalQFreq = 7;
	settings.nIter = 1000;
	settings.potency = 7;
	settings.nQFreqSteps = 100;
	settings.CheckQFreqIteration = false;

	ui.initialTauEdit->setText(QString::number(settings.initialTau));
	ui.finalTauEdit->setText(QString::number(settings.finalTau));
	ui.nStepsEdit->setText(QString::number(settings.nSteps));
	ui.cavityFreqEdit->setText(QString::number(settings.cFreq));
	ui.qubitFreqEdit->setText(QString::number(settings.initialQFreq));
	ui.nIterEdit->setText(QString::number(settings.nIter));
	ui.potencyEdit->setText(QString::number(settings.potency));
	ui.nStepsQfreqEdit->setText(QString::number(settings.nQFreqSteps));

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

	colorMap = new QCPColorMap(ui.DynamicPlotWidget->xAxis, ui.DynamicPlotWidget->yAxis);

	colorMap->data()->setSize(settings.nSteps, settings.nQFreqSteps);
	colorMap->setGradient(QCPColorGradient::gpHot);
	colorMap->setDataRange(QCPRange(0, 1));
	colorMap->setInterpolate(false);
	colorMap->rescaleDataRange(true);
}

void PiPulseQt::on_saveFileLocationEdit_editingFinished()
{
	qDebug("here %s", ui.saveFileLocationEdit->text().toLocal8Bit().constData());
	settings.filename = ui.saveFileLocationEdit->text().toLocal8Bit().constData();
}

void PiPulseQt::on_initialQFreqEdit_editingFinished()
{
	settings.initialQFreq = ui.initialQFreqEdit->text().toDouble();
}

void PiPulseQt::on_finalQFreqEdit_editingFinished()
{
	settings.finalQFreq = ui.finalQFreqEdit->text().toDouble();
}

void PiPulseQt::on_nStepsQfreqEdit_editingFinished()
{
	settings.nQFreqSteps = ui.nStepsQfreqEdit->text().toInt();
	colorMap->data()->setSize(settings.nSteps, settings.nQFreqSteps);
}

void PiPulseQt::on_qFreqInterationCheckBox_stateChanged(int state)
{
	if (state == Qt::Unchecked) 
	{
		ui.initialQFreqLabel->setEnabled(false);
		ui.initialQFreqEdit->setEnabled(false);
		ui.finalQFreqLabel->setEnabled(false);
		ui.finalQFreqEdit->setEnabled(false);
		ui.nStepsQfreqLabel->setEnabled(false);
		ui.nStepsQfreqEdit->setEnabled(false);

		ui.qubitFreqLabel->setEnabled(true);
		ui.qubitFreqEdit->setEnabled(true);

		settings.CheckQFreqIteration = false;

	}
	else {
		ui.initialQFreqLabel->setEnabled(true);
		ui.initialQFreqEdit->setEnabled(true);
		ui.finalQFreqLabel->setEnabled(true);
		ui.finalQFreqEdit->setEnabled(true);
		ui.nStepsQfreqLabel->setEnabled(true);
		ui.nStepsQfreqEdit->setEnabled(true);

		ui.qubitFreqLabel->setEnabled(false);
		ui.qubitFreqEdit->setEnabled(false);

		settings.CheckQFreqIteration = true;


	}
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
	colorMap->data()->setSize(settings.nSteps, settings.nQFreqSteps);
}

void PiPulseQt::on_cavityFreqEdit_editingFinished()
{
	settings.cFreq = ui.cavityFreqEdit->text().toDouble();
}

void PiPulseQt::on_qubitFreqEdit_editingFinished()
{
	settings.initialQFreq = ui.qubitFreqEdit->text().toDouble();
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

		ui.StartMeasurementButton->setEnabled(false);
		ui.StopMeasurementButton->setEnabled(true);
		running = true;

		if (!settings.CheckQFreqIteration)
		{
			ui.DynamicPlotWidget->graph(0)->setData(x, y);
			ui.DynamicPlotWidget->replot();

			emit startMeasurement(settings);
		}
		else
		{
			colorMap->data()->setRange(QCPRange(settings.initialTau , settings.finalTau), QCPRange(settings.initialQFreq, settings.finalQFreq));
			ui.DynamicPlotWidget->rescaleAxes();
			colorMap->data()->fill(0.0f);
			emit startMeasurement(settings);
		}
		
		ui.notificationLabel->setText("Acquiring data");
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



void PiPulseQt::receivedDataPoint(int i, int j, double data)
{
	if (!settings.CheckQFreqIteration)
	{
		double dtau = (settings.finalTau - settings.initialTau) / (settings.nSteps - 1);
		x[i] = i * dtau + settings.initialTau;
		y[i] = data;
		ui.DynamicPlotWidget->graph(0)->setData(x, y);
		ui.DynamicPlotWidget->replot();
	}
	else
	{
		colorMap->data()->setCell(i, j, data);
		ui.DynamicPlotWidget->replot();
	}
	

}

void PiPulseQt::finishedMeasurement()
{
	ui.StartMeasurementButton->setEnabled(true);
	ui.StopMeasurementButton->setEnabled(false);
	for (int i = 0; i < settings.nSteps; i++)
	{
		x[i] = 0;
		y[i] = 0;
	}

	running = false;
	ui.notificationLabel->setText("Waiting Measurement to Start");

}