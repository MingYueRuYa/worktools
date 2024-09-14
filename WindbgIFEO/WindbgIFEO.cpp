﻿#include "WindbgIFEO.h"
#include "everything/Everything.h"
#include "exec_helper.h"

#include <QtCore/QProcess>
#include <QtCore/QSettings>

WindbgIFEO::WindbgIFEO(QWidget* parent) : QWidget(parent) {
  ui.setupUi(this);
  //ui.label_reg_path->setText(this->_ifeo_reg_path);
  this->search_windbg_path();
}

void WindbgIFEO::on_pushButtonAdd_clicked() {
  QString reg_path = this->get_reg_path();
  if (reg_path.isEmpty()) {
    this->log_info(tr("Error: Please input process name"), false);
    return;
  }

  if (0 == ui.comboBox_windbg_path->count()) {
    this->log_info(tr("Error: Not find any windbg path"), false);
    return;
  }

  QSettings settings(reg_path, QSettings::NativeFormat);
  settings.setValue(this->_bugger_value,
                    ui.comboBox_windbg_path->currentText());
  this->log_info("set value successful", true);
}

void WindbgIFEO::on_pushButtonDel_clicked() {
  QString process_name = this->get_process_name();
  if (process_name.isEmpty()) {
    this->log_info(tr("Please input process name"), false);
    return;
  }

  QSettings settings(this->_ifeo_reg_path, QSettings::NativeFormat);
  settings.remove(process_name);
  this->log_info(tr("remove value successful"), true);
}

void WindbgIFEO::on_pushButtonOpenRegEditor_clicked() {}

void WindbgIFEO::on_pushButtonPostmortem_clicked() {
  QString windbg_path = this->ui.comboBox_windbg_path->currentText();
  if (windbg_path.isEmpty()) {
    this->log_info("Please select windbg", false);
    return;
  }

  QString reg_path = "";
  ExecHelper exec_helper;
  ExecHelper::Architecture arch = exec_helper.detect_arch(windbg_path);
  std::map<ExecHelper::Architecture, QString> arch_map = {
      {ExecHelper::Architecture::ARCH_X86, this->_x86_postmortem_reg_path},
      {ExecHelper::Architecture::ARCH_X64, this->_x64_postmortem_reg_path}};
  auto find_itr = arch_map.find(arch);

  QString command_line = " -p %ld -e %ld -g";
  windbg_path = QString("\"") + windbg_path + QString("\"") + command_line;

  QSettings bug_settings(find_itr->second, QSettings::NativeFormat);
  bug_settings.setValue(this->_bugger_value, windbg_path);
  bug_settings.setValue("Auto", 1);
  this->log_info("register postmortem successful", true);
}

QString WindbgIFEO::get_reg_path() const {
  QString process_name = this->get_process_name();
  if (process_name.isEmpty()) {
    return QString("");
  }

  return this->_ifeo_reg_path + QString("\\") + process_name;
}

QString WindbgIFEO::get_process_name() const {
  return ui.lineEdit_process_name->text();
}

void WindbgIFEO::log_info(const QString& info, bool is_suc) {
  ui.textBrowserLog->append(info);
}

void WindbgIFEO::search_windbg_path() {
  DWORD i;

  // Set the search string to abc
  Everything_SetMatchCase(true);
  Everything_SetMatchWholeWord(true);
  Everything_SetSearch(L"windbg.exe|DbgX.Shell.exe ext:exe");

  // Execute the query.
  Everything_Query(TRUE);

  // Display results.
  QString path = "";
  for (i = 0; i < Everything_GetNumResults(); i++) {
    path = QString::fromStdWString(Everything_GetResultPath(i)) +
           QString("\\") +
           QString::fromStdWString(Everything_GetResultFileName(i));
    ui.comboBox_windbg_path->addItem(path);
  }
}
