#include "EnvTools.h"
#include <QTableWidgetItem>

EnvTools::EnvTools(QWidget* parent)
  : QWidget(parent)
{
  ui.setupUi(this);
  this->_init_ui();
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
