#pragma once

#include "exec_helper.h"
#include "reg_editor_helper.h"
#include "settings.h"
#include "ui_WindbgIFEO.h"

#include "process_helper.h"
//#include "workspace/workspace.hpp"
#include "ThreadPool/thread_pool.hpp"

#include <map>
#include <string>
#include <thread>
#include <QProcess>

#include <QEvent>
#include <QtWidgets/QWidget>

class NcFramelessHelper;

// windbg image file exection options
class WindbgIFEO : public QWidget {
  Q_OBJECT

    using map_qstring = std::map<QString, QString>;

public:
  enum class LOG_TYPE { VERBORSE, DEBUG, INFO, WARNING, ERR };

public:
  WindbgIFEO(QWidget* parent = Q_NULLPTR);
  ~WindbgIFEO();
  WindbgIFEO(const WindbgIFEO&) = delete;

  void log_info(const QString& info, LOG_TYPE type = LOG_TYPE::INFO);

signals:
  void finished_windbg_exes();
  void finished_process_info();

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

  // attach config
  void on_btn_attach_clicked();
  void on_attach_name_changed(const QString& name);

  // postmortem config
  void on_pushButtonPostmortem_clicked();
  void on_pushButtonCancelPostmortem_clicked();
  void on_pushButtonOpenX64RegEditor_clicked();
  void on_pushButtonOpenX86RegEditor_clicked();
  void on_pushButtonPostmortemQuery_clicked();

  // settings
  // void on_chinese_stateChanged(int state);
  // void on_english_stateChanged(int state);
  void on_auto_start_stateChanged(int state);

  void on_update_windbg_path();
  void on_update_process_info();
  void on_process_finished(int exitCode);
  void on_process_error(QProcess::ProcessError);

  void on_comboBoxChanged(const QString& text);
  void on_comboBoxLanguage(const QString& text);

  void on_btn_refresh_process_clicked();
  void on_btn_refresh_process_2_clicked();
  void on_btn_refresh_path_clicked();

  void on_btn_close_clicked();
  void on_btn_mini_clicked();

protected:
  virtual void changeEvent(QEvent* ev) override;
  virtual bool nativeEvent(const QByteArray& eventType, void* message, long* result);

private:
  void _init_ui();
  void _init_comobo();
  void _init_signal();
  QString _get_reg_path() const;
  QString _get_process_name() const;
  void _query_windbg_path();
  void _enum_process_name();
  void _add_windbg_path(const map_qstring& paths);
  void _add_proc_info(const ProcessHelper::process_map& process);

  QString _format_log_info(const QString& info, LOG_TYPE type);
  bool _get_cur_windbg_path(QString& path, QString& err_msg);
  QString _get_arch_str(const QString& windbg_path);
  QString _get_arch_reg(const QString& windbg_path);
  void _start_reg_edit_proc();
  void _location_reg_path(const QString& reg_path);
  void _remove_combo_item(QComboBox* combo);
    void _updateWindowGeometry();
private:
  Ui::WindbgIFEOClass ui;
  map_qstring _map_windbg_path;
  ProcessHelper::process_map _proces_info;
  RegEditorHelper _reg_editor_helper;

  const QString _ifeo_reg_path =
    R"(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options)";
  const QString _x86_postmortem_reg_path =
    R"(HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Windows NT\CurrentVersion\AeDebug)";
  const QString _x64_postmortem_reg_path =
    R"(HKLM\Software\Microsoft\Windows NT\CurrentVersion\AeDebug)";
  const QString _bugger_value = "Debugger";
  // const QString _windbg_name = "WinDbg.exe";

  const std::map<ExecHelper::Architecture, QString> _arch_map = {
      {ExecHelper::Architecture::ARCH_X86, this->_x86_postmortem_reg_path},
      {ExecHelper::Architecture::ARCH_X64, this->_x64_postmortem_reg_path},
      {ExecHelper::Architecture::ARCH_UNKNOWN, ""} };

  Settings _settings;
  NcFramelessHelper* _frame_less_helper;
  //std::unique_ptr<wsp::workbranch> _workbranch_ptr;
  std::unique_ptr<std::threadpool> _workbranch_ptr;
  int _origin_width = 0;
};
