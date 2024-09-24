#include "application.h"

#include <QTranslator>

Application::Application(int& argc, char** argv) : QApplication(argc, argv) {}

Application::~Application() {}

bool Application::switch_language(Language lang) {
  static QTranslator trans;
  if (Language::ch_ZN == lang) {
    if (!trans.load(":/WindbgConfig/language/zh_CN.qm")) {
      return false;
    }
    qApp->installTranslator(&trans);
    return true;
  } else if (Language::en_US == lang) {
    if (!trans.load(":/WindbgConfig/language/en_US.qm")) {
      return false;
    }
    qApp->installTranslator(&trans);
    return true;
  }
}

QString Application::AppName() const {
  return "WinDbg Config Assistant";
}
