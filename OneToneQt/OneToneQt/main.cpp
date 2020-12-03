#include "OneToneQt.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OneToneQt w;
    w.show();
    return a.exec();
}
