#include "shadowwidget.h"
#include "ui_shadow.h"

ShadowWidget::ShadowWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form)
{
    this->setWindowFlags(this->windowFlags()|Qt::FramelessWindowHint);
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);
}


void ShadowWidget::resizeEvent(QResizeEvent *ev)
{
    ui->background->setGeometry(this->rect());
    QWidget::resizeEvent(ev);
}
