#include "application.h"

#include <QIcon>
#include <QTranslator>
#include <QNetworkProxyFactory>
#include <QLoggingCategory>
#include <QWebEngineSettings>

Application::Application(int& argc, char** argv) : QApplication(argc, argv) {
  _init();
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
      {Language::zh_CN, ":/WindbgConfig/language/zh_CN.qm"},
      {Language::en_US, ":/WindbgConfig/language/en_US.qm"}};

  if (!trans.load(_map[lang])) {
    return false;
  }
  qApp->installTranslator(&trans);
  return true;
}

void Application::_init() {
  this->setWindowIcon(QIcon(":/WindbgConfig/images/icon.png"));

  // 设置系统代理
  QNetworkProxyFactory::setUseSystemConfiguration(true);

  setEffectEnabled(Qt::UI_AnimateCombo, false);
  setEffectEnabled(Qt::UI_AnimateMenu, false);
  setEffectEnabled(Qt::UI_FadeMenu, false);
  setEffectEnabled(Qt::UI_AnimateTooltip, false);
  setEffectEnabled(Qt::UI_FadeTooltip, false);
  setEffectEnabled(Qt::UI_AnimateToolBox, false);

#if defined(_DEBUG)
  QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);
  QLoggingCategory::defaultCategory()->setEnabled(QtWarningMsg, true);
  QLoggingCategory::defaultCategory()->setEnabled(QtCriticalMsg, true);
#else
  QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, false);
  QLoggingCategory::defaultCategory()->setEnabled(QtWarningMsg, false);
  QLoggingCategory::defaultCategory()->setEnabled(QtCriticalMsg, false);
#endif
  QWebEngineSettings* webSetting = QWebEngineSettings::globalSettings();
  webSetting->setAttribute(QWebEngineSettings::AutoLoadImages, true);
  webSetting->setAttribute(QWebEngineSettings::PluginsEnabled, true);
  webSetting->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
  webSetting->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
  webSetting->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
  webSetting->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
  webSetting->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
  webSetting->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
  webSetting->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
  webSetting->setAttribute(QWebEngineSettings::PlaybackRequiresUserGesture, false);
  webSetting->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
  webSetting->setAttribute(QWebEngineSettings::WebGLEnabled, true);
  webSetting->setAttribute(QWebEngineSettings::AutoLoadIconsForPage, false);
  webSetting->setAttribute(QWebEngineSettings::XSSAuditingEnabled, false);
  webSetting->setAttribute(QWebEngineSettings::ErrorPageEnabled, false);
  webSetting->setDefaultTextEncoding("UTF-8");
}
