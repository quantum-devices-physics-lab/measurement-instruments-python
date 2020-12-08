#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PiPulseQt.h"

class PiPulseQt : public QMainWindow
{
    Q_OBJECT

public:
    PiPulseQt(QWidget *parent = Q_NULLPTR);
	Ui::PiPulseQtClass ui;
    
};
