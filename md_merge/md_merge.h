#pragma once

#include <QtWidgets/QWidget>
#include "ui_md_meger.h"

class MDMerge : public QWidget
{
  Q_OBJECT

public:
  MDMerge(QWidget* parent = nullptr);
  ~MDMerge();

protected slots:
  void on_btn_start_clicked();

private:
  void traverse_files(const QString& directoryPath);
  void add_path();
  void sort_path();
  void merge_file();

private:
  Ui::md_megerClass ui;
  std::vector<QString> _file_path;
};
