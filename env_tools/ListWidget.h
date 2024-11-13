#pragma once
#include <QListWidget>
class ListWidget :public QListWidget
{
  Q_OBJECT

public:
  ListWidget(QWidget* parent = nullptr);
  ~ListWidget();

  void dropEvent(QDropEvent* event) override;

signals:
  void row_moved();

private:
  Q_DISABLE_COPY_MOVE(ListWidget);
};

