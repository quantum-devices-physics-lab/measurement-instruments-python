#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_OneToneQt.h"

class OneToneQt : public QMainWindow
{
    Q_OBJECT

public:
    OneToneQt(QWidget *parent = Q_NULLPTR);

private:
    Ui::OneToneQtClass ui;
};
