#include "EnvTools.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication a(argc, argv);
  EnvTools w;
  w.show();
  return a.exec();
}
