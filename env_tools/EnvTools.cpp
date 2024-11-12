#include "EnvTools.h"
#include <QTableWidgetItem>

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

  int idx = 0;
  QString key = "";
  QString value = "";
  for (auto& item : values) {
    this->ui.tableWidget->insertRow(idx);
    key = QString::fromStdWString(item.first);
    QTableWidgetItem* new_item = new QTableWidgetItem(key);
    new_item->setData(Qt::UserRole, key);
    this->ui.tableWidget->setItem(idx, 0, new_item);

    value = QString::fromStdWString(item.second);
    new_item = new QTableWidgetItem(value);
    new_item->setData(Qt::UserRole, value);
    this->ui.tableWidget->setItem(idx, 1, new_item);
    idx++;
  }
}

void EnvTools::_init_signal()
{
  connect(ui.tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(DoCellClicked(int, int)));
  connect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(OnItemChanged(QTableWidgetItem*)));
}

void EnvTools::_update_cached(EditType type,
  const QString& old_key,
  const QString& new_key,
  const QString& old_value,
  const QString& new_value)
{
  auto data = this->_gen_cached(type, old_key, new_key, old_value, new_value);
  _cached_data.insert(data);
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
  std::for_each(_cached_data.begin(), _cached_data.end(), [](CachedData* data) {delete data; });
  _cached_data.clear();
}


void EnvTools::DoCellClicked(int row, int column)
{
  ui.listWidget->clear();
  QString name = ui.tableWidget->item(row, 0)->text();
  auto target_value = _env_helper.find(name.toStdWString());
  if (target_value.first == L"")
  {
    return;
  }
  else
  {
    ui.label_key->setText(QString::fromStdWString(target_value.first));
    wstring value = target_value.second;
    std::vector<wstring> vec_v = EnvHelper::split(value, L';');
    for (auto& item : vec_v) {
      this->ui.listWidget->addItem(QString::fromStdWString(item));
    }
  }
}

void EnvTools::OnItemChanged(QTableWidgetItem* item)
{
  QString value = item->data(Qt::UserRole).toString();
  int row = this->ui.tableWidget->row(item);
  int column = this->ui.tableWidget->column(item);
  // 新建的item
  if (value.isEmpty())
  {
    QTableWidgetItem* key_item = this->ui.tableWidget->item(row, 0);
    QString new_key = key_item->text();
    if (new_key.isEmpty())
    {
      return;
    }
    key_item->setData(Qt::UserRole, new_key);
    QTableWidgetItem* value_item = this->ui.tableWidget->item(row, 1);
    QString new_value = value_item->text();
    this->_update_cached(EditType::AddKey, "", new_key, "", new_value);
    if (!new_value.isEmpty())
    {
      value_item->setData(Qt::UserRole, new_value);
    }
  }
  else
  {
    int row = this->ui.tableWidget->row(item);
    int column = this->ui.tableWidget->column(item);
    // 判断和之前的值是否有变化，如果有变化，则保存
    // 注意key和name不可为空
  }
}

void EnvTools::on_btn_add_all_clicked()
{
  int row_count = this->ui.tableWidget->rowCount();
  this->ui.tableWidget->insertRow(row_count);
  this->ui.tableWidget->setItem(row_count, 0, new QTableWidgetItem());
  this->ui.tableWidget->setItem(row_count, 1, new QTableWidgetItem());
  this->ui.tableWidget->scrollToBottom();
}

void EnvTools::on_btn_delete_all_clicked()
{
  int cur_row = this->ui.tableWidget->currentRow();
  if (cur_row == -1)
  {
    return;
  }
  QString key = this->ui.tableWidget->item(cur_row, 0)->data(Qt::UserRole).toString();
  this->ui.tableWidget->removeRow(cur_row);
  this->_update_cached(EditType::DelKey, key, "", "", "");
}

void EnvTools::on_btn_add_sub_clicked()
{

}

void EnvTools::on_btn_del_sub_clicked()
{

}

void EnvTools::on_btn_save_clicked()
{
  if (_cached_data.empty())
  {
    return;
  }

  for (auto& item : _cached_data) {
    if (item->type == EditType::AddKey)
    {
      this->_env_helper.set_key(item->new_key.c_str(), item->new_value.c_str());
    }
    else if (item->type == EditType::DelKey)
    {
      this->_env_helper.del_key(item->old_key);
    }
  }

  this->_clear_cached();
}

void EnvTools::test_data()
{
  CachedData* data = new CachedData();
  data->type = EditType::AddKey;
  data->old_key = L"1";
  data->new_key = L"2";
  _cached_data.insert(data);

  CachedData* data2 = new CachedData();
  data2->type = EditType::AddKey;
  data2->old_key = L"1";
  data2->new_key = L"2";

  auto ifd = _cached_data.find(data2);

  int count = _cached_data.count(data2);
  int count1 = _cached_data.count(data);
}
