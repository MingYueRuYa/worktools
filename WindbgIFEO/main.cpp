#include "WindbgIFEO.h"
#include "application.h"

#include <QDebug>
#include <QtWidgets/QApplication>

int main(int argc, char* argv[]) {
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

  Application app(argc, argv);
  WindbgIFEO w;
  w.show();
  return app.exec();
}
