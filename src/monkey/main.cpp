#include "monmainwindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    monMainWindow w;
    w.show();
    return a.exec();
}
