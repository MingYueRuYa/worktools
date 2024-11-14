#include "EnvTools.h"

#include <QTableWidgetItem>
#include <QProcess>

const int kCOLUMN_KEY = 0;
const int kCOLUMN_VALUE = 1;

EnvTools::EnvTools(QWidget* parent)
  : QWidget(parent)
{
  ui.setupUi(this);
  this->_init_ui();
  this->_init_signal();
}

EnvTools::~EnvTools()
{}

void EnvTools::_init_ui()
{
  std::map<wstring, wstring> values = _env_helper.get_env();

  this->ui.label_key->setText("");
  this->_clear_table_item();
  this->ui.listWidget->clear();
  int idx = 0;
  QString key = "";
  QString value = "";
  for (auto& item : values) {
    this->ui.tableWidget->insertRow(idx);
    key = QString::fromStdWString(item.first);
    QTableWidgetItem* new_item = new QTableWidgetItem(key);
    new_item->setData(Qt::UserRole, key);
    this->ui.tableWidget->setItem(idx, kCOLUMN_KEY, new_item);

    value = QString::fromStdWString(item.second);
    new_item = new QTableWidgetItem(value);
    new_item->setData(Qt::UserRole, value);
    this->ui.tableWidget->setItem(idx, kCOLUMN_VALUE, new_item);
    idx++;
  }
}

void EnvTools::_init_signal()
{
  this->_connect_sig(true);
}

void EnvTools::_connect_sig(bool is_con)
{
  if (is_con)
  {
    connect(ui.tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(DoCellClicked(int, int)));
    connect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(OnTableItemChanged(QTableWidgetItem*)));
    connect(ui.listWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(OnListItemChanged(QListWidgetItem*)));
    connect(ui.listWidget, SIGNAL(row_moved()), this, SLOT(OnListItemMoved()));
  }
  else
  {
    disconnect(ui.tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(DoCellClicked(int, int)));
    disconnect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(OnTableItemChanged(QTableWidgetItem*)));
    disconnect(ui.listWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(OnListItemChanged(QListWidgetItem*)));
    disconnect(ui.listWidget, SIGNAL(row_moved()), this, SLOT(OnListItemMoved()));
  }
}

void EnvTools::_update_cached(EditType type,
  const QString& old_key,
  const QString& new_key,
  const QString& old_value,
  const QString& new_value)
{
  Key key = old_key.toStdWString();
  auto data = this->_gen_cached(type, old_key, new_key, old_value, new_value);

  auto ifd = _cached_data.find(key);
  if (ifd != _cached_data.end()) {
    _cached_data.erase(key);
  }
  _cached_data.insert({ key, data });
}

CachedData* EnvTools::_gen_cached(EditType type,
  const QString& old_key,
  const QString& new_key,
  const QString& old_value,
  const QString& new_value)
{
  CachedData* data = new CachedData();
  data->type = type;
  data->old_key = old_key.toStdWString();
  data->new_key = new_key.toStdWString();
  data->old_value = old_value.toStdWString();
  data->new_value = new_value.toStdWString();
  return data;
}

void EnvTools::_clear_cached()
{
  std::for_each(_cached_data.begin(), _cached_data.end(),
    [](std::pair<Key, Value> v) {delete v.second; v.second = nullptr; });
  _cached_data.clear();
}

QString EnvTools::_find_table_value(const QString& key)
{
  if (key.isEmpty())
  {
    return "";
  }

  QString value = "";
  for (int i = 0; i < ui.tableWidget->rowCount(); ++i) {
    QString old_key = this->_get_table_item_data(i, kCOLUMN_KEY);
    if (old_key != key)
    {
      continue;
    }
    value = this->_get_table_item_text(i, kCOLUMN_VALUE);
  }
  return value;
}

QString EnvTools::_list_get_value()
{
  QStringList values = {};
  QString temp_value = "";
  for (int i = 0; i < ui.listWidget->count(); ++i) {
    temp_value = ui.listWidget->item(i)->text();
    if (temp_value.isEmpty())
    {
      continue;
    }
    values << temp_value;
  }
  return values.join(";");
}

void EnvTools::_kill_explorer()
{
  QProcess process;
  QStringList arguments;
  arguments << "/f" << "/im" << "explorer.exe";
  process.start("taskkill", arguments);
  process.waitForFinished();
}

void EnvTools::_start_explorer()
{
  QProcess::startDetached("explorer.exe", QStringList());
}

QString EnvTools::_get_table_item_data(int row, int column)
{
  auto item = ui.tableWidget->item(row, column);
  if (item == nullptr)
  {
    return "";
  }
  else
  {
    return item->data(Qt::UserRole).toString();
  }
}

QString EnvTools::_get_table_item_data(QTableWidgetItem* item)
{
  int row = this->ui.tableWidget->row(item);
  int column = this->ui.tableWidget->column(item);
  return this->_get_table_item_data(row, column);
}

QString EnvTools::_get_table_item_text(int row, int column)
{
  auto item = ui.tableWidget->item(row, column);
  if (item == nullptr)
  {
    return "";
  }
  else
  {
    return item->text();
  }
}
QString EnvTools::_get_table_item_text(QTableWidgetItem* item)
{
  int row = this->ui.tableWidget->row(item);
  int column = this->ui.tableWidget->column(item);
  return this->_get_table_item_text(row, column);
}

void EnvTools::_reset_data()
{
  this->_connect_sig(false);
  this->_env_helper.reset();
  this->_init_ui();
  this->_connect_sig(true);
  this->_clear_cached();
}

void EnvTools::_clear_table_item()
{
  while (ui.tableWidget->rowCount() > 0) {
    ui.tableWidget->removeRow(0);
  }
}

void EnvTools::DoCellClicked(int row, int column)
{
  ui.listWidget->clear();
  QString name = ui.tableWidget->item(row, kCOLUMN_KEY)->text();
  QString target_value = this->_find_table_value(name);
  ui.label_key->setText(name);
  wstring value = target_value.toStdWString();
  std::vector<wstring> vec_v = EnvHelper::split(value, L';');
  for (auto& item : vec_v) {
    QListWidgetItem* list_item = new QListWidgetItem(QString::fromStdWString(item));
    list_item->setFlags(list_item->flags() | Qt::ItemIsEditable);
    this->ui.listWidget->addItem(list_item);
  }
}

void EnvTools::OnTableItemChanged(QTableWidgetItem* item)
{
  QString value = this->_get_table_item_text(item);
  int row = this->ui.tableWidget->row(item);
  int column = this->ui.tableWidget->column(item);

  if (value.isEmpty())
  {
    return;
  }
  else
  {
    if (kCOLUMN_KEY == column)
    {
      QString new_key = this->_get_table_item_text(row, kCOLUMN_KEY);
      if (new_key.isEmpty())
      {
        return;
      }
      QString old_key = this->_get_table_item_data(row, kCOLUMN_KEY);
      QString new_value = this->_get_table_item_text(row, kCOLUMN_VALUE);
      if (old_key.isEmpty())
      {
        this->_update_cached(EditType::AddKey, old_key, new_key, "", new_value);
      }
      else
      {
        this->_update_cached(EditType::UpdateKey, old_key, new_key, "", new_value);
      }
    }
    else if (kCOLUMN_VALUE == column)
    {
      QString new_value = this->_get_table_item_text(row, kCOLUMN_VALUE);
      QString old_value = this->_get_table_item_data(row, kCOLUMN_VALUE);
      if (new_value.isEmpty() || (new_value == old_value))
      {
        return;
      }
      QString old_key = this->_get_table_item_text(row, kCOLUMN_KEY);
      this->_update_cached(EditType::UpdateValue, old_key, "", old_value, new_value);
    }
  }

  //// 新建的item
  //if (value.isEmpty())
  //{
  //  QTableWidgetItem* key_item = this->ui.tableWidget->item(row, kCOLUMN_KEY);
  //  QString new_key = key_item->text();
  //  if (new_key.isEmpty())
  //  {
  //    return;
  //  }
  //  //key_item->setData(Qt::UserRole, new_key);
  //  QTableWidgetItem* value_item = this->ui.tableWidget->item(row, kCOLUMN_VALUE);
  //  QString new_value = value_item->text();
  //  this->_update_cached(EditType::AddKey, "", new_key, "", new_value);
  //  //if (!new_value.isEmpty())
  //  //{
  //  //  value_item->setData(Qt::UserRole, new_value);
  //  //}
  //}
  //else
  //{
  //  if (kCOLUMN_KEY == column)
  //  {
  //    QTableWidgetItem* key_item = this->ui.tableWidget->item(row, kCOLUMN_KEY);
  //    QString new_key = key_item->text();
  //    if (new_key.isEmpty())
  //    {
  //      return;
  //    }
  //    this->_update_cached(EditType::UpdateKey, old_key, new_key, "", "");
  //  }
  //  else if (kCOLUMN_VALUE == column)
  //  {

  //  }
  //  //int row = this->ui.tableWidget->row(item);
  //  //int column = this->ui.tableWidget->column(item);
  //  // 判断和之前的值是否有变化，如果有变化，则保存
  //  // 注意key和name不可为空
  //}
}

void EnvTools::OnListItemChanged(QListWidgetItem* item)
{
  QString txt = item->text();
  if (txt.isEmpty())
  {
    return;
  }
  QString cur_key = ui.label_key->text();
  QString new_value = this->_list_get_value();
  this->_update_cached(EditType::UpdateValue, cur_key, "", "", new_value);
}

void EnvTools::OnListItemMoved(/*const QModelIndexList&*/)
{
  QString cur_key = ui.label_key->text();
  QString new_value = this->_list_get_value();
  QString old_value = this->_find_table_value(cur_key);
  if (new_value == old_value)
  {
    return;
  }
  this->_update_cached(EditType::UpdateValue, cur_key, "", "", new_value);
}

void EnvTools::on_btn_add_all_clicked()
{
  int row_count = this->ui.tableWidget->rowCount();
  this->ui.tableWidget->insertRow(row_count);
  this->ui.tableWidget->setItem(row_count, kCOLUMN_KEY, new QTableWidgetItem());
  this->ui.tableWidget->setItem(row_count, kCOLUMN_VALUE, new QTableWidgetItem());
  this->ui.tableWidget->scrollToBottom();
}

void EnvTools::on_btn_delete_all_clicked()
{
  int cur_row = this->ui.tableWidget->currentRow();
  if (cur_row == -1)
  {
    return;
  }
  QString key = this->ui.tableWidget->item(cur_row, kCOLUMN_KEY)->data(Qt::UserRole).toString();
  this->ui.tableWidget->removeRow(cur_row);
  this->_update_cached(EditType::DelKey, key, "", "", "");
}

void EnvTools::on_btn_add_sub_clicked()
{
  QListWidgetItem* item = new QListWidgetItem();
  item->setFlags(item->flags() | Qt::ItemIsEditable);
  ui.listWidget->addItem(item);
}

void EnvTools::on_btn_del_sub_clicked()
{
  int cur_row = ui.listWidget->currentRow();
  if (cur_row == -1)
  {
    return;
  }
  ui.listWidget->takeItem(cur_row);
  QString cur_key = ui.label_key->text();
  QString new_value = this->_list_get_value();
  this->_update_cached(EditType::UpdateValue, cur_key, "", "", new_value);
}

void EnvTools::on_btn_save_clicked()
{
  if (_cached_data.empty())
  {
    return;
  }

  for (auto& item : _cached_data) {
    auto value = item.second;
    if (value->type == EditType::AddKey)
    {
      this->_env_helper.set_key(value->new_key.c_str(), value->new_value.c_str());
    }
    else if (value->type == EditType::DelKey)
    {
      this->_env_helper.del_key(value->old_key);
    }
    else if (value->type == EditType::UpdateValue)
    {
      this->_env_helper.set_key(value->old_key.c_str(), value->new_value.c_str());
    }
    else if (value->type == EditType::UpdateKey)
    {
      this->_env_helper.del_key(value->old_key.c_str());
      this->_env_helper.set_key(value->new_key.c_str(), value->new_value.c_str());
    }
  }
  this->_reset_data();
}

void EnvTools::on_btn_restart_explorer_clicked()
{
  this->_kill_explorer();
  this->_start_explorer();
}

void EnvTools::test_data()
{
  //CachedData* data = new CachedData();
  //data->type = EditType::AddKey;
  //data->old_key = L"1";
  //data->new_key = L"2";
  //_cached_data.insert(data);

  //CachedData* data2 = new CachedData();
  //data2->type = EditType::AddKey;
  //data2->old_key = L"1";
  //data2->new_key = L"2";

  //auto ifd = _cached_data.find(data2);

  //int count = _cached_data.count(data2);
  //int count1 = _cached_data.count(data);
}
