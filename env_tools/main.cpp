#include "EnvTools.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication a(argc, argv);
  a.setWindowIcon(QIcon(":/EnvTools/icon.png"));
  EnvTools w;
  w.show();
  return a.exec();
}
