#include "WindbgIFEO.h"

#include <QCompleter>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QTimer>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <algorithm>
#include <tuple>

#include "NcFrameLessHelper.h"
#include "application.h"
#include "dolproductinfo.h"
#include "everything/Everything.h"

WindbgIFEO::WindbgIFEO(QWidget* parent)
    : QWidget(parent), _map_windbg_path(), _stop_enum_process(false) {
  ui.setupUi(this);
  this->_init_ui();
  this->_init_signal();
  this->_query_windbg_path();
  this->_enum_process_name();
}

WindbgIFEO::~WindbgIFEO() {
  _stop_enum_process = true;
  std::vector<std::unique_ptr<std::thread>*> thr_ptr = {&_query_windbg_ptr,
                                                        &_enum_process_ptr};
  for (auto& item : thr_ptr) {
    if ((*item)->joinable()) {
      this->log_info(tr("wait thread exit..."));
      (*item)->join();
    }
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
  // TODO: start failed reason
  result = QProcess::startDetached(cur_path, QStringList(), work_dir);
  const std::map<bool, std::pair<QString, LOG_TYPE>> map_result = {
      {{true, {tr("Start WinDbg successful."), LOG_TYPE::INFO}},
       {false, {tr("Start WinDbg error."), LOG_TYPE::ERR}}}};
  auto itr = map_result.find(result);
  this->log_info(itr->second.first, itr->second.second);
}
void WindbgIFEO::on_pushButtonDbgDetails_clicked() {
  QString cur_path = "";
  QString err_msg = "";
  bool result = this->_get_cur_windbg_path(cur_path, err_msg);
  if (!result) {
    this->log_info(err_msg, LOG_TYPE::ERR);
    return;
  }
  dolProductInfo info(cur_path.toStdString(), true);
  QString file_details = QString(tr("File description:")) +
                         QString::fromStdString(info.GetFileDescription()) +
                         "\n";
  file_details +=
      QString(tr("Type:")) + QString::fromStdString(info.GetType()) + "\n";
  file_details += QString(tr("File version:")) +
                  QString::fromStdString(info.GetFileVersion()) + "\n";
  file_details += QString(tr("Production name:")) +
                  QString::fromStdString(info.GetProductName()) + "\n";
  file_details += QString(tr("Production version:")) +
                  QString::fromStdString(info.GetProductVersion()) + "\n";
  file_details += QString(tr("Copyright:")) +
                  QString::fromStdString(info.GetCopyRight()) + "\n";
  file_details +=
      QString(tr("Size:")) + QString::fromStdString(info.GetSize()) + "\n";
  file_details += QString(tr("Date modified:")) +
                  QString::fromStdString(info.GetModifiedTime()) + "\n";
  file_details += QString(tr("Language:")) +
                  QString::fromStdString(info.GetLanguage()) + "\n";
  file_details += QString(tr("Original filename:")) +
                  QString::fromStdString(info.GetOriginName()) + "\n";
  file_details += QString(tr("Digital Signatures:")) +
                  QString::fromStdString(info.GetDigSignature()) + "\n";

  this->log_info(file_details, LOG_TYPE::INFO);
}

void WindbgIFEO::on_pushButtonStartDbgDir_clicked() {
  QString cur_path = "";
  QString err_msg = "";
  bool result = this->_get_cur_windbg_path(cur_path, err_msg);
  if (!result) {
    this->log_info(err_msg, LOG_TYPE::ERR);
    return;
  }

  QFileInfo file_info(cur_path);
  const QString work_dir = "file:///" + file_info.absoluteDir().absolutePath();
  result = QDesktopServices::openUrl(work_dir);
}

void WindbgIFEO::on_pushButtonAdd_clicked() {
  QString reg_path = this->_get_reg_path();
  if (reg_path.isEmpty()) {
    this->log_info(tr("Please input process name"), LOG_TYPE::ERR);
    return;
  }

  if (0 == ui.comboBox_windbg_path->count()) {
    this->log_info(tr("Not find any WinDbg path"), LOG_TYPE::ERR);
    return;
  }

  const QString value = "\"" + ui.comboBox_windbg_path->currentText() + "\"" +
                        " " + ui.lineEdit_process_param->text();
  QSettings settings(reg_path, QSettings::NativeFormat);
  settings.setValue(this->_bugger_value, value);
  this->log_info(QString(tr("set value successful, process name:%1"))
                     .arg(this->_get_process_name()));
}

void WindbgIFEO::on_pushButtonDel_clicked() {
  QString process_name = this->_get_process_name();
  if (process_name.isEmpty()) {
    this->log_info(tr("Please input process name"), LOG_TYPE::ERR);
    return;
  }

  QSettings settings(this->_ifeo_reg_path, QSettings::NativeFormat);
  settings.remove(process_name);
  this->log_info(QString(tr("Remove value successful, process name:%1"))
                     .arg(this->_get_process_name()));
}

void WindbgIFEO::on_pushButtonIFEOOpenReg_clicked() {
  this->_start_reg_edit_proc();
  this->_location_reg_path(this->_ifeo_reg_path);
}

void WindbgIFEO::on_pushButtonIFEOQuery_clicked() {
  const QString sel_proc_name = this->_get_process_name();
  QString reg_path = "";
  QStringList process_names = {};
  if (sel_proc_name.isEmpty()) {
    reg_path = this->_ifeo_reg_path;
    QSettings settings(this->_ifeo_reg_path, QSettings::NativeFormat);
    QStringList groups = settings.childGroups();
    for (const auto& group : groups) {
      if (group.contains("\\")) {
        continue;
      }
      process_names.append(group);
    }
  } else {
    reg_path = this->_ifeo_reg_path + "\\" + sel_proc_name;
    process_names.append(sel_proc_name);
  }

  // Image File Exection Options value
  this->log_info(tr("Start query image file exection options:"));

  if (process_names.isEmpty()) {
    this->log_info(tr("Not find any value."));
  } else {
    for (const QString& name : process_names) {
      QSettings settings(this->_ifeo_reg_path + "\\" + name,
                         QSettings::NativeFormat);
      QString value = settings.value(this->_bugger_value).toString();
      if (value.isEmpty()) {
        value = tr("Not find value.");
      }
      this->log_info(QString(tr("%1:%2")).arg(name).arg(value));
    }
  }
}

void WindbgIFEO::on_btn_attach_clicked() {
  // 1 获取需要调试的进程
  // 2 获取pid
  // 3 根据进程类型选择合适的windbg挂载调试
  QString windbg_path = "";
  QString error_msg = "";
  bool result = this->_get_cur_windbg_path(windbg_path, error_msg);
  QString pid = this->ui.comboBox_attach_pid->currentText();
  QStringList params = {"-p", pid};
  QFileInfo file_info(windbg_path);
  const QString work_dir = file_info.absoluteDir().absolutePath();
  QProcess::startDetached(windbg_path, params, work_dir);
}

void WindbgIFEO::on_attach_name_changed(const QString& name) {
  std::vector<int> vec_pid = {};
  for (auto& item : _proces_info) {
    if (name.toLower() != QString::fromStdWString(item.second.name).toLower()) {
      continue;
    }
    vec_pid.push_back(item.first);
  }

  ui.comboBox_attach_pid->clear();
  for (auto& pid : vec_pid) {
    ui.comboBox_attach_pid->addItem(QString::number(pid));
  }
}

void WindbgIFEO::on_pushButtonPostmortem_clicked() {
  QString windbg_path = "";
  QString err_msg = "";
  bool result = this->_get_cur_windbg_path(windbg_path, err_msg);
  if (!result) {
    this->log_info(err_msg);
    return;
  }

  QString reg_path = this->_get_arch_reg(windbg_path);
  QString str_arch = this->_get_arch_str(windbg_path);

  const QString command_line = " -p %ld -e %ld -g";
  windbg_path = QString("\"") + windbg_path + QString("\"") + command_line;

  QSettings bug_settings(reg_path, QSettings::NativeFormat);
  bug_settings.setValue(this->_bugger_value, windbg_path);
  bug_settings.setValue("Auto", 1);
  this->log_info(
      QString(tr("Register %1 postmortem successful")).arg(str_arch));
}

void WindbgIFEO::on_pushButtonCancelPostmortem_clicked() {
  QString windbg_path = "";
  QString err_msg = "";
  bool result = this->_get_cur_windbg_path(windbg_path, err_msg);
  if (!result) {
    this->log_info(err_msg);
    return;
  }

  QString reg_path = this->_get_arch_reg(windbg_path);
  QString str_arch = this->_get_arch_str(windbg_path);

  QSettings bug_settings(reg_path, QSettings::NativeFormat);
  bug_settings.setValue(this->_bugger_value, "");
  bug_settings.setValue("Auto", 1);
  QString log_info =
      QString(tr("Unregister %1 postmortem successful")).arg(str_arch);
  this->log_info(log_info);
}

void WindbgIFEO::on_pushButtonOpenX64RegEditor_clicked() {
  this->_start_reg_edit_proc();
  this->_location_reg_path(this->_x64_postmortem_reg_path);
}

void WindbgIFEO::on_pushButtonOpenX86RegEditor_clicked() {
  this->_start_reg_edit_proc();
  this->_location_reg_path(this->_x86_postmortem_reg_path);
}

void WindbgIFEO::on_pushButtonPostmortemQuery_clicked() {
  // TODO: declear type by decltype key word
  // namespace type = decltype(this->_arch_map.begin());

  auto func = [this](const auto& item) {
    QString path = item.second;
    QSettings bug_settings(path, QSettings::NativeFormat);
    QVariant var = bug_settings.value(this->_bugger_value, "");
    if (item.first == ExecHelper::Architecture::ARCH_X64) {
      this->log_info(tr("x64 config:"));
    } else if (item.first == ExecHelper::Architecture::ARCH_X86) {
      this->log_info(tr("x86 config:"));
    }
    QString value = var.toString();
    if (value.isEmpty()) {
      value = tr("Not find any value");
    }
    this->log_info(var.toString());
  };
  std::for_each(this->_arch_map.begin(), this->_arch_map.end(), func);
}

void WindbgIFEO::on_auto_start_stateChanged(int state) {
  const QString reg_path =
      R"(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run)";

  QSettings setting(reg_path, QSettings::NativeFormat);
  QString name = ((Application*)qApp)->AppName();

  this->_settings.set_auto_start(Qt::Checked == state);
  if (Qt::Checked == state) {
    QString cur_path = "\"" + qApp->applicationFilePath() + "\"";
    setting.setValue(name.replace(" ", ""), cur_path);
    this->log_info(
        QString(tr("set auto start successful, path:%1")).arg(cur_path),
        LOG_TYPE::INFO);
  } else {
    setting.remove(name);
    this->log_info(tr("unset auto start successful"), LOG_TYPE::INFO);
  }
}

QString WindbgIFEO::_get_reg_path() const {
  QString process_name = this->_get_process_name();
  if (process_name.isEmpty()) {
    return QString("");
  } else {
    return this->_ifeo_reg_path + QString("\\") + process_name;
  }
}

QString WindbgIFEO::_get_process_name() const {
  return ui.comboBox_proc_name->currentText();
}

void WindbgIFEO::log_info(const QString& info, LOG_TYPE type) {
  ui.textBrowserLog->append(info);
}

void WindbgIFEO::_query_windbg_path() {
  // std::thread thr();
  auto func = [this]() {
    Everything_SetMatchCase(true);
    Everything_SetMatchWholeWord(true);
    Everything_SetSearch(L"windbg.exe|DbgX.Shell.exe ext:exe");

    // Execute the query.
    Everything_Query(TRUE);

    // Display results.
    map_qstring map_path = {};
    QString path = "";
    DWORD i;
    for (i = 0; i < Everything_GetNumResults(); i++) {
      path = QString::fromStdWString(Everything_GetResultPath(i)) +
             QString("\\") +
             QString::fromStdWString(Everything_GetResultFileName(i));
      map_path[path] = path;
      qDebug() << path;
    }
    this->_add_windbg_path(map_path);
  };

  _query_windbg_ptr = std::make_unique<std::thread>(func);
}

void WindbgIFEO::_enum_process_name() {
  auto func = [this]() {
    // while (!this->_stop_enum_process) {
    {
      ProcessHelper::process_map proc_map = ProcessHelper::EnumAllProcess();
      this->_add_proc_info(proc_map);
      // std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  };
  _enum_process_ptr = std::make_unique<std::thread>(func);
}

void WindbgIFEO::_add_proc_info(const ProcessHelper::process_map& process) {
  this->_proces_info = process;
  emit this->finished_process_info();
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

QString WindbgIFEO::_get_arch_str(const QString& windbg_path) {
  ExecHelper::Architecture arch = ExecHelper::detect_arch(windbg_path);
  return arch == ExecHelper::Architecture::ARCH_X86 ? "x86" : "x64";
}

QString WindbgIFEO::_get_arch_reg(const QString& windbg_path) {
  // TODO: cache
  ExecHelper::Architecture arch = ExecHelper::detect_arch(windbg_path);
  auto find_itr = this->_arch_map.find(arch);
  if (find_itr == this->_arch_map.end()) {
    return "";
  } else {
    return find_itr->second;
  }
}

void WindbgIFEO::_start_reg_edit_proc() {
  QProcess* process = new QProcess();
  connect(process, SIGNAL(finished(int)), this, SLOT(on_process_finished(int)));
  process->setProgram("regedit");
  process->start();
}

void WindbgIFEO::_location_reg_path(const QString& reg_path) {
  bool result = this->_reg_editor_helper.send_value(reg_path.toStdWString());
  QString msg = result ? tr("Open registry editor successful")
                       : tr("Open registry editor failed");
  this->log_info(msg, LOG_TYPE::INFO);
  this->log_info(reg_path, LOG_TYPE::INFO);
}

void WindbgIFEO::_remove_combo_item(QComboBox* combo) {
  int item_count = combo->count();
  for (int i = 0; i < item_count; i++) {
    combo->removeItem(i);
  }
}

void WindbgIFEO::_init_ui() {
  this->setAttribute(Qt::WA_TranslucentBackground, true);
  this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
  _frame_less_helper = new NcFramelessHelper();
  _frame_less_helper->activeOnWithChildWidget(this, ui.widget_title);
  ui.chb_auto_start->setChecked(_settings.get_auto_start());

  this->_init_comobo();

  QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
  effect->setOffset(0, 0);
  effect->setColor(Qt::lightGray);
  effect->setBlurRadius(10);
  this->setGraphicsEffect(effect);
}

void WindbgIFEO::_init_comobo() {
  std::vector<QComboBox*> vec_com = {ui.comboBox_proc_name,
                                     ui.comboBox_attach_name};
  for (auto& item : vec_com) {
    QCompleter* com = new QCompleter(item->model(), this);
    com->setCaseSensitivity(Qt::CaseInsensitive);
    item->setCompleter(com);
  }

  using data_pair = std::pair<QString, Application::Language>;
  for (auto& item :
       {data_pair(tr("chinese"), Application::Language::zh_CN),
        data_pair(tr("english(US)"), Application::Language::en_US)}) {
    this->ui.comboBox_language->addItem(item.first, (int)item.second);
  }
  // this->ui.comboBox_language->addItems({tr("chinese"), tr("english(US)")});
  std::string lang = this->_settings.get_lang();
  if (lang == "zh_CN") {
    this->ui.comboBox_language->setCurrentText("zh_CN");
    ((Application*)qApp)->switch_language(Application::Language::zh_CN);
  } else {
    this->ui.comboBox_language->setCurrentText("en_US");
    ((Application*)qApp)->switch_language(Application::Language::en_US);
  }
}

void WindbgIFEO::_init_signal() {
  connect(this, SIGNAL(finished_process_info()), this,
          SLOT(on_update_process_info()), Qt::QueuedConnection);

  connect(this, SIGNAL(finished_windbg_exes()), this,
          SLOT(on_update_windbg_path()), Qt::QueuedConnection);

  // connect(ui.chb_chinese, SIGNAL(stateChanged(int)), this,
  //        SLOT(on_chinese_stateChanged(int)));
  // connect(ui.chb_english, SIGNAL(stateChanged(int)), this,
  //        SLOT(on_english_stateChanged(int)));
  connect(ui.comboBox_attach_name, SIGNAL(currentTextChanged(QString)), this,
          SLOT(on_attach_name_changed(QString)));
  connect(ui.chb_auto_start, SIGNAL(stateChanged(int)), this,
          SLOT(on_auto_start_stateChanged(int)));
  connect(ui.comboBox_language, SIGNAL(currentTextChanged(QString)), this,
          SLOT(on_comboBoxLanguage(QString)));
}

void WindbgIFEO::on_update_windbg_path() {
  std::for_each(
      this->_map_windbg_path.begin(), this->_map_windbg_path.end(),
      [this](const std::pair<QString, QString>& value) {
        QString windbg_path = value.second;
        if (windbg_path.contains("arm")) {
          this->log_info(QString(tr("filter arm version windbg path:%1"))
                             .arg(windbg_path));
        } else {
          this->ui.comboBox_windbg_path->addItem(value.second);
          this->log_info(value.second);
        }
      });
  connect(ui.comboBox_windbg_path, SIGNAL(currentTextChanged(QString)), this,
          SLOT(on_comboBoxChanged(QString)));
}

void WindbgIFEO::on_update_process_info() {
  std::vector<QComboBox*> vec_com = {ui.comboBox_proc_name,
                                     ui.comboBox_attach_name};

  QStringList process_list = {};
  for (auto& info : this->_proces_info) {
    QString name = QString::fromStdWString(info.second.name);
    if (-1 != process_list.indexOf(name)) {
      continue;
    }
    process_list.append(name);
  }
  for (auto& item : vec_com) {
    item->blockSignals(true);
    // item->clear();
    this->_remove_combo_item(item);
    item->addItems(process_list);
    item->blockSignals(false);
  }
}

void WindbgIFEO::on_process_finished(int exitCode) {
  QProcess* process = (QProcess*)sender();
  process->deleteLater();
}

void WindbgIFEO::on_comboBoxChanged(const QString& text) {
  this->log_info(tr("selecte current Windbg:") + "\n" + text, LOG_TYPE::INFO);
}

void WindbgIFEO::on_comboBoxLanguage(const QString& text) {
  int data = this->ui.comboBox_language->currentData(Qt::UserRole).toInt();
  bool is_zh_CN = (Application::Language)data == Application::Language::zh_CN;
  ((Application*)qApp)
      ->switch_language(is_zh_CN ? Application::Language::zh_CN
                                 : Application::Language::en_US);
  this->_settings.set_lang(is_zh_CN ? "zh_CN" : "en_US");
  this->log_info(is_zh_CN ? tr("set language chinese successful")
                          : tr("set language english successful"),
                 LOG_TYPE::INFO);

  this->update();
}

void WindbgIFEO::on_btn_close_clicked() {
  this->close();
}

void WindbgIFEO::on_btn_mini_clicked() {
  QPoint pos = QCursor::pos();
  QCursor::setPos(pos + QPoint(0, 30));  // 设置鼠标位置
  QTimer::singleShot(100, [this]() { this->showMinimized(); });
}

void WindbgIFEO::changeEvent(QEvent* ev) {
  switch (ev->type()) {
    case QEvent::LanguageChange:
      ui.retranslateUi(this);
  }
  QWidget::changeEvent(ev);
}