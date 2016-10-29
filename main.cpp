#include "inductancecalc.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    inductanceCalc w;

    w.show();

    return a.exec();
}
