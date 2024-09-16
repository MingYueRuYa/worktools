#include "WindbgIFEO.h"

#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <algorithm>
#include <tuple>

#include "everything/Everything.h"
#include "exec_helper.h"

WindbgIFEO::WindbgIFEO(QWidget* parent) : QWidget(parent), _map_windbg_path() {
  ui.setupUi(this);

  this->_init_signal();
  this->_query_windbg_path();
}

WindbgIFEO::~WindbgIFEO() {
  if (this->_search_windbg_ptr->joinable()) {
    this->log_info(tr("wait thread exit..."));
    this->_search_windbg_ptr->join();
  }
}

void WindbgIFEO::on_pushButtonStartDbg_clicked() {
  QString cur_path = "";
  QString err_msg = "";
  bool result = this->_get_cur_windbg_path(cur_path, err_msg);
  if (!result) {
    this->log_info(err_msg, LOG_TYPE::ERR);
    return; 
  }

  QFileInfo file_info(cur_path);
  const QString work_dir = file_info.absoluteDir().absolutePath();
  result = QProcess::startDetached(cur_path, QStringList(), work_dir);
  const std::map<bool, std::pair<QString, LOG_TYPE>> map_result = {
      {{true, {tr("Start windbg successful."), LOG_TYPE::INFO}},
       {false, {tr("Start windbg error."), LOG_TYPE::ERR}}}};
  auto itr = map_result.find(result);
  this->log_info(itr->second.first, itr->second.second);
}
void WindbgIFEO::on_pushButtonDbgDetails_clicked() {}

void WindbgIFEO::on_pushButtonStartDbgDir_clicked() {
  //const QString process_path = this->ui.comboBox_windbg_path->currentText();
  //if (process_path.isEmpty()) {
  //  this->log_info(tr("application can't be empty."), LOG_TYPE::ERR);
  //  return;
  //}

  //QFileInfo file_info(process_path);
  //if (!file_info.exists(process_path)) {
  //  this->log_info(tr("Not find application"), LOG_TYPE::ERR);
  //  return;
  //}

  QString cur_path = "";
  QString err_msg = "";
  bool result = this->_get_cur_windbg_path(cur_path, err_msg);
  if (! result) {
    this->log_info(err_msg, LOG_TYPE::ERR);
    return;
  }

  QFileInfo file_info(cur_path);
  const QString work_dir = "file:///"+file_info.absoluteDir().absolutePath();
  result = QDesktopServices::openUrl(work_dir);
}

void WindbgIFEO::on_pushButtonAdd_clicked() {
  QString reg_path = this->_get_reg_path();
  if (reg_path.isEmpty()) {
    this->log_info(tr("Error: Please input process name"), LOG_TYPE::ERR);
    return;
  }

  if (0 == ui.comboBox_windbg_path->count()) {
    this->log_info(tr("Error: Not find any windbg path"), LOG_TYPE::ERR);
    return;
  }

  QSettings settings(reg_path, QSettings::NativeFormat);
  settings.setValue(this->_bugger_value,
                    ui.comboBox_windbg_path->currentText());
  this->log_info("set value successful");
}

void WindbgIFEO::on_pushButtonDel_clicked() {
  QString process_name = this->_get_process_name();
  if (process_name.isEmpty()) {
    this->log_info(tr("Please input process name"), LOG_TYPE::ERR);
    return;
  }

  QSettings settings(this->_ifeo_reg_path, QSettings::NativeFormat);
  settings.remove(process_name);
  this->log_info(tr("remove value successful"));
}

void WindbgIFEO::on_pushButtonIFEOOpenReg_clicked() {}

void WindbgIFEO::on_pushButtonIFEOQuery_clicked() {}

void WindbgIFEO::on_pushButtonPostmortem_clicked() {
  QString windbg_path = this->ui.comboBox_windbg_path->currentText();
  if (windbg_path.isEmpty()) {
    this->log_info("Please select windbg", LOG_TYPE::ERR);
    return;
  }

  QString reg_path = "";
  ExecHelper exec_helper;
  ExecHelper::Architecture arch = exec_helper.detect_arch(windbg_path);
  std::map<ExecHelper::Architecture, QString> arch_map = {
      {ExecHelper::Architecture::ARCH_X86, this->_x86_postmortem_reg_path},
      {ExecHelper::Architecture::ARCH_X64, this->_x64_postmortem_reg_path}};
  auto find_itr = arch_map.find(arch);

  const QString command_line = " -p %ld -e %ld -g";
  windbg_path = QString("\"") + windbg_path + QString("\"") + command_line;

  QSettings bug_settings(find_itr->second, QSettings::NativeFormat);
  bug_settings.setValue(this->_bugger_value, windbg_path);
  bug_settings.setValue("Auto", 1);
  this->log_info("register postmortem successful");
}

void WindbgIFEO::on_pushButtonCancelPostmortem_clicked() {}

void WindbgIFEO::on_pushButtonOpenRegEditor_clicked() {}

void WindbgIFEO::on_pushButtonPostmortemQuery_clicked() {}

QString WindbgIFEO::_get_reg_path() const {
  QString process_name = this->_get_process_name();
  if (process_name.isEmpty()) {
    return QString("");
  }

  return this->_ifeo_reg_path + QString("\\") + process_name;
}

QString WindbgIFEO::_get_process_name() const {
  return ui.lineEdit_process_name->text();
}

void WindbgIFEO::log_info(const QString& info, LOG_TYPE type) {
  ui.textBrowserLog->append(info);
}

void WindbgIFEO::_query_windbg_path() {
  // std::thread thr();
  _search_windbg_ptr = std::make_unique<std::thread>([this]() {
    DWORD i;
    // Set the search string to abc
    Everything_SetMatchCase(true);
    Everything_SetMatchWholeWord(true);
    Everything_SetSearch(L"windbg.exe|DbgX.Shell.exe ext:exe");

    // Execute the query.
    Everything_Query(TRUE);

    // Display results.
    map_qstring map_path = {};
    QString path = "";
    for (i = 0; i < Everything_GetNumResults(); i++) {
      path = QString::fromStdWString(Everything_GetResultPath(i)) +
             QString("\\") +
             QString::fromStdWString(Everything_GetResultFileName(i));
      map_path[path] = path;
      qDebug() << path;
    }
    this->_add_windbg_path(map_path);
  });
}

void WindbgIFEO::_add_windbg_path(const map_qstring& paths) {
  this->_map_windbg_path = paths;
  emit this->finished_windbg_exes();
}

QString WindbgIFEO::_format_log_info(const QString& info, LOG_TYPE type) {
  const std::map<LOG_TYPE, QString> map_log = {
      {LOG_TYPE::VERBORSE, "[VERBORSE]"},
      {LOG_TYPE::DEBUG, "[DEBUG]:"},
      {LOG_TYPE::INFO, "[INFO]:"},
      {LOG_TYPE::WARNING, "[WARNING]:"},
      {LOG_TYPE::ERR, "[ERR]:"}};

  auto itr = map_log.find(type);
  return itr->second + info;
}

bool WindbgIFEO::_get_cur_windbg_path(QString& path, QString& err_msg) {
  const QString process_path = this->ui.comboBox_windbg_path->currentText();
  if (process_path.isEmpty()) {
    err_msg = tr("application can't be empty.");
    return false;
  }
  QFileInfo file_info(process_path);
  if (!file_info.exists(process_path)) {
    err_msg = tr("Not find application");
    return false;
  }
  path = process_path;
  return true;
}

void WindbgIFEO::_init_signal() {
  connect(this, SIGNAL(finished_windbg_exes()), this,
          SLOT(on_update_windbg_path()), Qt::QueuedConnection);
}

void WindbgIFEO::on_update_windbg_path() {
  std::for_each(this->_map_windbg_path.begin(), this->_map_windbg_path.end(),
                [this](const std::pair<QString, QString>& value) {
                  this->ui.comboBox_windbg_path->addItem(value.second);
                  this->log_info(value.second);
                });
}
