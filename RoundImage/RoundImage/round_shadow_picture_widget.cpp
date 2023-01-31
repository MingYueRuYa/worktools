#include "round_shadow_picture_widget.h"
#include "round_image_helper.h"

#include <QAction>
#include <QFileDialog>
#include <QFileInfo>
#include <QPainter>
#include <QMessageBox>

RoundShadowPictureWidget::RoundShadowPictureWidget(const QString& pixmap_path,
                                                   QWidget* parent)
    : pixmap_path_(pixmap_path), QWidget(parent) {
  setupUI();
  updateUI();
}

RoundShadowPictureWidget::RoundShadowPictureWidget(const QPixmap& pixmap,
                                                   QWidget* parent)
    : pixmap_(pixmap), QWidget(parent) {
  setupUI();
  updateUI();
}

RoundShadowPictureWidget::~RoundShadowPictureWidget() {}

void RoundShadowPictureWidget::setupUI() {
  if (pixmap_.isNull()) {
    pixmap_ = QPixmap(pixmap_path_);
  }
  if (!pixmap_.isNull()) {
    RoundImageHelper helper;
    round_shadow_pixmap_ = helper.RoundImage(pixmap_);
  }
  QAction* action = new QAction(tr("save"), this);
  action->setObjectName("action_save");
  connect(action, SIGNAL(triggered(bool)), this,
          SLOT(on_action_save_triggered(bool)));
  menu_.addAction(action);
  action = new QAction(tr("exit"), this);
  action->setObjectName("action_exit");
  connect(action, SIGNAL(triggered(bool)), this,
          SLOT(on_action_exit_triggered(bool)));
  menu_.addAction(action);
}

void RoundShadowPictureWidget::updateUI() {
  if (!round_shadow_pixmap_.isNull()) {
    this->setFixedSize(round_shadow_pixmap_.size());
  }
}

void RoundShadowPictureWidget::paintEvent(QPaintEvent* event) {
  if (!round_shadow_pixmap_.isNull()) {
    QPainter painter(this);
    painter.drawPixmap(this->rect(), round_shadow_pixmap_);
  }
}

void RoundShadowPictureWidget::mousePressEvent(QMouseEvent* event) {
  this->raise();
  move_widget_helper_.mousePressEvent(event, pos());
}

void RoundShadowPictureWidget::mouseReleaseEvent(QMouseEvent* event) {
  move_widget_helper_.mouseReleaseEvent(event);
}

void RoundShadowPictureWidget::mouseMoveEvent(QMouseEvent* event) {
  if (move_widget_helper_.CanMove()) {
    move(move_widget_helper_.mouseMoveEvent(event));
  }
}

void RoundShadowPictureWidget::contextMenuEvent(QContextMenuEvent* event) {
  menu_.exec(event->globalPos());
}

void RoundShadowPictureWidget::on_action_save_triggered(bool) {
    QString file_name = QFileDialog::getSaveFileName(
        this, tr("Save File"), "", tr("Images (*.png)"));
    if (!file_name.isEmpty()) {
      round_shadow_pixmap_.save(file_name);
    } else {
      QMessageBox::information(NULL, tr("warning"),
                               tr("Please input file name"), QMessageBox::Ok);
    }
}

void RoundShadowPictureWidget::on_action_exit_triggered(bool) {
  emit Exit(this);
}
