#pragma once

#include <QtWidgets/QWidget>
#include <string>
#include "ui_WindbgIFEO.h"

// windbg image file exection options
class WindbgIFEO : public QWidget {
  Q_OBJECT

 public:
  WindbgIFEO(QWidget* parent = Q_NULLPTR);
  ~WindbgIFEO() = default;
  WindbgIFEO(const WindbgIFEO&) = delete;

 protected slots:
  void on_pushButtonAdd_clicked();
  void on_pushButtonDel_clicked();
  void on_pushButtonOpenRegEditor_clicked();

 private:
  QString get_reg_path() const;
  QString get_process_name() const;
  void log_info(const QString &info, bool is_error);
  void search_windbg_path();

 private:
  Ui::WindbgIFEOClass ui;

  const QString _reg_path =
      R"(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options)";
  const QString _bugger_value = "Debugger";
  const QString _windbg_name = "WinDbg.exe";
};