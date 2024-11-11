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
  for (auto& item : values) {
    this->ui.tableWidget->insertRow(idx);
    this->ui.tableWidget->setItem(idx, 0, new QTableWidgetItem(QString::fromStdWString(item.first)));
    this->ui.tableWidget->setItem(idx, 1, new QTableWidgetItem(QString::fromStdWString(item.second)));
    idx++;
  }
}

void EnvTools::_init_signal()
{
  connect(ui.tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(DoCellClicked(int, int)));
}

void EnvTools::DoCellClicked(int row, int column)
{
  ui.listWidget->clear();
  this->_clear_list();
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

void EnvTools::on_btn_add_all_clicked()
{

}

void EnvTools::on_btn_del_all_clicked()
{

}

void EnvTools::on_btn_add_sub_clicked()
{

}

void EnvTools::on_btn_del_sub_clicked()
{

}


