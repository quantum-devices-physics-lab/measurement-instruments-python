#include "PiPulseQt.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PiPulseQt w;
    w.show();
    return a.exec();
}
