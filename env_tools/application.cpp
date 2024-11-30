#include "application.h"

#include <QIcon>
#include <QTranslator>

Application::Application(int& argc, char** argv) : QApplication(argc, argv) {
  this->setWindowIcon(QIcon(":/WindbgConfig/images/icon.png"));
  this->switch_language(Language::zh_CN);
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
      {Language::zh_CN, ":/EnvTools/translator/env_tools_zh.qm"},
      {Language::en_US, ":/EnvTools/translator/env_tools_en.qm"} };

  if (!trans.load(_map[lang])) {
    return false;
  }
  return qApp->installTranslator(&trans);
}
