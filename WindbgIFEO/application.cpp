#include "application.h"

#include <QIcon>
#include <QTranslator>

Application::Application(int& argc, char** argv) : QApplication(argc, argv) {
  this->setWindowIcon(QIcon(":/WindbgConfig/images/icon.png"));
}

Application::~Application() {}

bool Application::switch_language(Language lang) {
  static QTranslator trans;
  return this->_install_lang(trans, lang);
}

QString Application::AppName() const {
  return "WinDbg Config Assistant";
}

bool Application::_install_lang(QTranslator& trans, Language lang) {
  std::map<Language, QString> _map = {
      {Language::ch_ZN, ":/WindbgConfig/language/zh_CN.qm"},
      {Language::en_US, ":/WindbgConfig/language/en_US.qm"}};

  if (!trans.load(_map[lang])) {
    return false;
  }
  qApp->installTranslator(&trans);
  return true;
}
