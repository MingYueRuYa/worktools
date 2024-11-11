#pragma once

#include <QtWidgets/QWidget>
#include "ui_EnvTools.h"
#include "EnvHelper.h"

class EnvTools : public QWidget
{
  Q_OBJECT

public:
  EnvTools(QWidget* parent = nullptr);
  ~EnvTools();

private:
  void _init_ui();

private:
  Ui::EnvToolsClass ui;
  EnvHelper _env_helper;
};
