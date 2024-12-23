#pragma once

#include <QApplication>

class Application : public QApplication {
  Q_OBJECT

 public:
  enum class Language { zh_CN = 0, en_US = 1 };

 public:
  Application(int& argc, char** arg);
  ~Application();

  bool switch_language(Language lang);
  QString AppName() const;

 private:
  bool _install_lang(QTranslator& trans, Language lang);
};
