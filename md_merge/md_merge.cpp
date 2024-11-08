#include "md_merge.h"

#include <QDir>
#include <QTextStream>

MDMerge::MDMerge(QWidget* parent)
  : QWidget(parent)
{
  ui.setupUi(this);
}

MDMerge::~MDMerge()
{}

void MDMerge::on_btn_start_clicked()
{
  ui.listWidget->clear();
  this->_file_path.clear();
  this->traverse_files(ui.line_edit_input_path->text());
  this->sort_path();
  this->add_path();
  this->merge_file();
}

void MDMerge::traverse_files(const QString& directoryPath)
{
  QDir dir(directoryPath);

  if (!dir.exists()) {
    return;
  }

  // 获取目录中的所有文件和文件夹信息
  QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
  for (const QFileInfo& fileInfo : fileInfoList) {
    if (fileInfo.isDir()) {
      traverse_files(fileInfo.absoluteFilePath());
    }
    else if (fileInfo.isFile() and fileInfo.suffix() == "md") {
      this->_file_path.push_back(fileInfo.fileName());
    }
  }
}

void MDMerge::add_path()
{
  for (auto& item : _file_path) {
    ui.listWidget->addItem(item);
  }
}

void MDMerge::sort_path()
{
  std::sort(_file_path.begin(), _file_path.end(), [](const QString& f1, const QString& f2) {
    bool result = false;
    int sq1 = f1.split(" ")[0].toInt(&result);
    if (!result) {
      sq1 = 0;
    }

    int sq2 = f2.split(" ")[0].toInt(&result);
    if (!result) {
      sq2 = 0;
    }
    return sq1 < sq2;
    });
}

void MDMerge::merge_file()
{
  QString output_name = ui.line_edit_output_path->text();
  if (output_name.isEmpty()) {
    output_name = "demo.md";
  }

  QString output_path = ui.line_edit_input_path->text() + "\\" + output_name;
  QFile file(output_path);

  if (!file.open(QIODevice::Append | QIODevice::Text))
  {
    return;
  }

  QString tmp_path = "";
  QString title = "";
  QTextStream out(&file);
  for (int i = 0; i < ui.listWidget->count(); ++i) {
    QString name = ui.listWidget->item(i)->text();
    tmp_path = ui.line_edit_input_path->text() + "\\" + name;
    QFile file(tmp_path);
    if (!file.open(QIODevice::ReadOnly))
    {
      continue;
    }
    title = "# " + name;
    out << title << "\n";
    out << file.readAll() << "\n";
  }

  file.close();
}

