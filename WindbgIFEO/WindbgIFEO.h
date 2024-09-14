#pragma once

#include "ui_WindbgIFEO.h"

#include <map>
#include <string>
#include <thread>

#include <QtWidgets/QWidget>

// windbg image file exection options
class WindbgIFEO : public QWidget {
  Q_OBJECT

  using map_qstring = std::map<QString, QString>;

 public:
  WindbgIFEO(QWidget* parent = Q_NULLPTR);
  ~WindbgIFEO();
  WindbgIFEO(const WindbgIFEO&) = delete;

  void log_info(const QString& info, bool is_error = true);

 signals:
  void finished_windbg_exes();

 protected slots:
  void on_pushButtonAdd_clicked();
  void on_pushButtonDel_clicked();
  void on_pushButtonOpenRegEditor_clicked();
  void on_pushButtonPostmortem_clicked();
  void on_update_windbg_path();

 private:
  void init_signal();
  QString get_reg_path() const;
  QString get_process_name() const;
  void search_windbg_path();
  void start_serach_windbg_path();
  void add_windbg_path(const map_qstring& paths);

 private:
  Ui::WindbgIFEOClass ui;
  map_qstring _map_windbg_path;
  std::unique_ptr<std::thread> _search_windbg_ptr;

  const QString _ifeo_reg_path =
      R"(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options)";
  const QString _x86_postmortem_reg_path =
      R"(HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Windows NT\CurrentVersion\AeDebug)";
  const QString _x64_postmortem_reg_path =
      R"(HKLM\Software\Microsoft\Windows NT\CurrentVersion\AeDebug)";
  const QString _bugger_value = "Debugger";
  // const QString _windbg_name = "WinDbg.exe";
};
