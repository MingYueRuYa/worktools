#pragma once

#include <QApplication>

class Application : public QApplication {
  Q_OBJECT

 public:
  enum class Language { ch_ZN = 0, en_US = 1 };

 public:
  Application(int& argc, char** arg);
  ~Application();

  bool switch_language(Language lang);
  QString AppName() const;
};
