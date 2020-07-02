#include "cattle.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Cattle w;
    w.show();
    return a.exec();
}
