#include "WindbgIFEO.h"
#include "application.h"

#include <QDebug>
#include <QtWidgets/QApplication>

int main(int argc, char* argv[]) {
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  Application app(argc, argv);
  if (!app.switch_language(Application::Language::ch_ZN)) {
    qDebug() << "switch language failed";
  }
  WindbgIFEO w;
  w.show();
  return app.exec();
}
