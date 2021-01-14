#include "OneToneQt.h"
#include "visa.h"
#include <Qtcore>
#include <QGuiApplication>

OneToneQt::OneToneQt(QWidget *parent)
    : QMainWindow(parent)
	, xlow(100)
	, ylow(100)
	, xhigh(100)
	, yhigh(100)
	, max_y_value(5)
{
	int error;
	ViSession session, vi;

    ui.setupUi(this);
	running = false;

	doubleValidator = new QDoubleValidator();
	intValidator = new QIntValidator();

	ui.FreqStartEdit->setValidator(doubleValidator);
	ui.FreqStopEdit->setValidator(doubleValidator);
	ui.NStepsEdit->setValidator(intValidator);
	ui.LowPowerEdit->setValidator(intValidator);
	ui.HighPowerEdit->setValidator(intValidator);
	ui.CircuitResistanceEdit->setValidator(doubleValidator);
	ui.IFBandwidthEdit->setValidator(doubleValidator);

	settings.startFrequency = 4.7;
	settings.stopFrequency = 5.2;
	settings.nSteps = 100;
	settings.lowPowerAttenuation = 60;
	settings.highPowerAttenuation = 10;
	settings.circuitResistance = 50;
	settings.ifBandwidth = 300;

	ui.FreqStartEdit->setText(QString::number(settings.startFrequency));
	ui.FreqStopEdit->setText(QString::number(settings.stopFrequency));
	ui.NStepsEdit->setText(QString::number(settings.nSteps));
	ui.LowPowerEdit->setText(QString::number(settings.lowPowerAttenuation));
	ui.HighPowerEdit->setText(QString::number(settings.highPowerAttenuation));
	ui.CircuitResistanceEdit->setText(QString::number(settings.circuitResistance));
	ui.IFBandwidthEdit->setText(QString::number(settings.ifBandwidth));

	ViChar buffer[5000];
	error = viOpenDefaultRM(&session);
	if (error != VI_SUCCESS)
	{
		qDebug("Error in locating resources");
	}

	ViFindList findlist;
	ViUInt32 matches;

	error = viFindRsrc(session, "?*INSTR", &findlist, &matches, buffer);
	if (error != VI_SUCCESS)
	{
		qDebug("Error in locating resources");
	}

	qDebug("matches: %d", matches);

	qDebug("resources:\n%s", buffer);
	ui.ResourceAddressesList->addItem(buffer);
	for (int i = 1; i < matches; i++) {
		viFindNext(findlist, buffer);
		ui.ResourceAddressesList->addItem(buffer);
	}

	error = viClose(session);


	indexlow = 0;
	indexhigh = 0;

	for (int i = 0; i < settings.nSteps; i++)
	{
		xlow[i] = 0;
		ylow[i] = 0;
		xhigh[i] = 0;
		yhigh[i] = 0;
	}

	ui.DynamicPlotWidget->xAxis->setRange(settings.startFrequency, settings.stopFrequency);
	ui.DynamicPlotWidget->yAxis->setRange(0, max_y_value);
	ui.DynamicPlotWidget->legend->setVisible(true);

	ui.DynamicPlotWidget->addGraph();
	ui.DynamicPlotWidget->graph(0)->setPen(QColor(0,0, 255, 255));
	ui.DynamicPlotWidget->graph(0)->setLineStyle(QCPGraph::lsNone);
	ui.DynamicPlotWidget->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
	ui.DynamicPlotWidget->graph(0)->setName("Low Power");

	
	ui.DynamicPlotWidget->addGraph();
	ui.DynamicPlotWidget->graph(1)->setPen(QColor(255, 0, 0, 255));
	ui.DynamicPlotWidget->graph(1)->setLineStyle(QCPGraph::lsNone);
	ui.DynamicPlotWidget->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 4));
	ui.DynamicPlotWidget->graph(1)->setName("High Power");
}

void OneToneQt::on_VisaConnectionButton_clicked()
{
	qDebug("Testing visa Connection");

	qDebug("PSG1 address: %s", ui.Psg1Edit->text().toLocal8Bit().constData());
	qDebug("PSG2 address: %s", ui.Psg2Edit->text().toLocal8Bit().constData());
	qDebug("Attenuator address: %s", ui.attenuatorEdit->text().toLocal8Bit().constData());
	qDebug("Oscilloscope address: %s", ui.OscEdit->text().toLocal8Bit().constData());

	int error;
	ViSession session, viPSG1, viPSG2, viAttenuator, viOsc;
	ViChar buffer[5000];

	error = viOpenDefaultRM(&session);
	if (error != VI_SUCCESS)
	{
		qDebug("Error in locating resources");
	}

	error = viOpen(session, ui.Psg1Edit->text().toLocal8Bit().constData(), VI_NO_LOCK, 10000, &viPSG1);
	if (error != VI_SUCCESS)
	{
		qDebug("Error in opening PSG1");
	}

	error = viOpen(session, ui.Psg2Edit->text().toLocal8Bit().constData(), VI_NO_LOCK, 10000, &viPSG2);
	if (error != VI_SUCCESS)
	{
		qDebug("Error in opening PSG2");
	}

	error = viOpen(session, ui.attenuatorEdit->text().toLocal8Bit().constData(), VI_NO_LOCK, 10000, &viAttenuator);
	if (error != VI_SUCCESS)
	{
		qDebug("Error in opening Attenuator");
	}

	error = viOpen(session, ui.OscEdit->text().toLocal8Bit().constData(), VI_NO_LOCK, 10000, &viOsc);
	if (error != VI_SUCCESS)
	{
		qDebug("Error in opening Oscilloscope");
	}

	error = viPrintf(viPSG1, "*IDN?\n");
	error = viScanf(viPSG1, "%t", buffer);
	qDebug("*IDN? -> %s", buffer);

	ui.PSG1ConnectionStatusLabel->setText(buffer);

	error = viPrintf(viPSG2, "*IDN?\n");
	error = viScanf(viPSG2, "%t", buffer);
	qDebug("*IDN? -> %s", buffer);

	ui.PSG2ConnectionStatusLabel->setText(buffer);

	error = viPrintf(viAttenuator, "*IDN?\n");
	error = viScanf(viAttenuator, "%t", buffer);
	qDebug("*IDN? -> %s", buffer);

	ui.AttenuatorConnectionStatusLabel->setText(buffer);

	error = viPrintf(viOsc, "*IDN?\n");
	error = viScanf(viOsc, "%t", buffer);
	qDebug("*IDN? -> %s", buffer);

	ui.OscilloscopeConnectionStatusLabel->setText(buffer);

	viClose(viPSG1);
	viClose(viPSG2);
	viClose(viAttenuator);
	viClose(viOsc);
	viClose(session);

}

void OneToneQt::on_ResourceAddressesList_itemActivated(QListWidgetItem *item)
{
	QClipboard* clipboard = QGuiApplication::clipboard();
	clipboard->setText(item->text());

	qDebug("selected "  + item->text().toLocal8Bit());
}

void OneToneQt::on_MeasurementNameEdit_editingFinished()
{
	settings.filename = ui.MeasurementNameEdit->text().toLocal8Bit().constData();
}

void OneToneQt::on_FreqStartEdit_editingFinished()
{
	settings.startFrequency = ui.FreqStartEdit->text().toDouble();
	ui.DynamicPlotWidget->xAxis->setRange(settings.startFrequency, settings.stopFrequency);
}


void OneToneQt::on_FreqStopEdit_editingFinished()
{
	settings.stopFrequency = ui.FreqStopEdit->text().toDouble();
	ui.DynamicPlotWidget->xAxis->setRange(settings.startFrequency, settings.stopFrequency);
}

void OneToneQt::on_NStepsEdit_editingFinished()
{
	settings.nSteps = ui.NStepsEdit->text().toInt();
	xlow.resize(settings.nSteps);
	xhigh.resize(settings.nSteps);
	ylow.resize(settings.nSteps);
	yhigh.resize(settings.nSteps);
}

void OneToneQt::on_LowPowerEdit_editingFinished()
{
	settings.lowPowerAttenuation = ui.LowPowerEdit->text().toInt();
}

void OneToneQt::on_HighPowerEdit_editingFinished()
{
	settings.highPowerAttenuation = ui.HighPowerEdit->text().toInt();
}

void OneToneQt::on_CircuitResistanceEdit_editingFinished()
{
	settings.circuitResistance = ui.CircuitResistanceEdit->text().toDouble();
}

void OneToneQt::on_IFBandwidthEdit_editingFinished()
{
	settings.ifBandwidth = ui.IFBandwidthEdit->text().toDouble();
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
		emit startMeasurement(settings);
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
	qDebug("type: %d, freq %f amp %f", graph,datax,datay);

	if (datay > max_y_value)
	{
		max_y_value = datay;
		ui.DynamicPlotWidget->yAxis->setRange(0, max_y_value);
	}

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
	for (int i = 0; i < settings.nSteps; i++)
	{
		xlow[i] = 0;
		ylow[i] = 0;
		xhigh[i] = 0;
		yhigh[i] = 0;
	}

	

	running = false;
	ui.measurementStatusLabel->setText("Waiting Measurement to Start");
	
}