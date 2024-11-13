#include "ListWidget.h"

ListWidget::ListWidget(QWidget* parent) :
  QListWidget(parent)
{
}

ListWidget::~ListWidget()
{
}

void ListWidget::dropEvent(QDropEvent* event)
{
  QListWidget::dropEvent(event);
  emit this->row_moved();
}

