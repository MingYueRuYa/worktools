#include "EnvTools.h"
#include "application.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  Application app(argc, argv);
  app.setWindowIcon(QIcon(":/EnvTools/icon.png"));
  EnvTools w;
  w.show();
  return app.exec();
}
