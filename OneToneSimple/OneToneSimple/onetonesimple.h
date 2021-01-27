#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_onetonesimple.h"

class OneToneSimple : public QMainWindow
{
    Q_OBJECT

public:
    OneToneSimple(QWidget *parent = Q_NULLPTR);

private:
    Ui::OneToneSimpleClass ui;
};
