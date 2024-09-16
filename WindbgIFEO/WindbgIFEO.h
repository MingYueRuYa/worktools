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
  enum class LOG_TYPE {
    VERBORSE,
    DEBUG,
    INFO,
    WARNING,
    ERR
  };

 public:
  WindbgIFEO(QWidget* parent = Q_NULLPTR);
  ~WindbgIFEO();
  WindbgIFEO(const WindbgIFEO&) = delete;

  void log_info(const QString& info, LOG_TYPE type = LOG_TYPE::INFO);

 signals:
  void finished_windbg_exes();

 protected slots:
  // windbg msic
  void on_pushButtonStartDbg_clicked();
  void on_pushButtonDbgDetails_clicked();
  void on_pushButtonStartDbgDir_clicked();

   // ifeo config
  void on_pushButtonAdd_clicked();
  void on_pushButtonDel_clicked();
  void on_pushButtonIFEOOpenReg_clicked();
  void on_pushButtonIFEOQuery_clicked();

   // postmortem config
  void on_pushButtonPostmortem_clicked();
  void on_pushButtonCancelPostmortem_clicked();
  void on_pushButtonOpenRegEditor_clicked();
  void on_pushButtonPostmortemQuery_clicked();

  void on_update_windbg_path();

 private:
  void _init_signal();
  QString _get_reg_path() const;
  QString _get_process_name() const;
  void _query_windbg_path();
  void _add_windbg_path(const map_qstring& paths);
  QString _format_log_info(const QString &info, LOG_TYPE type);
  bool _get_cur_windbg_path(QString &path, QString &err_msg);

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
