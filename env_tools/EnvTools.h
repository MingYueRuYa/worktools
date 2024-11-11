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
  void _init_signal();
  void _clear_list();


protected slots:
  void DoCellClicked(int row, int column);
  void on_btn_add_all_clicked();
  void on_btn_del_all_clicked();
  void on_btn_add_sub_clicked();
  void on_btn_del_sub_clicked();

private:
  Ui::EnvToolsClass ui;
  EnvHelper _env_helper;
};
