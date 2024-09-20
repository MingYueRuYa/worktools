#include "application.h"

#include <QTranslator>

Application::Application(int& argc, char** argv) : QApplication(argc, argv) {}

Application::~Application() {}

bool Application::switch_language(Language lang) {
  static QTranslator trans;
  if (trans.load(":/WindbgConfig/language/zh_CN.qm")) {
    qApp->installTranslator(&trans);
    return true;
  } else {
    return true;
  }
}
