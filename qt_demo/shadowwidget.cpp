#include "shadowwidget.h"
#include "ui_shadow.h"

ShadowWidget::ShadowWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form)
{
    this->setWindowFlags(this->windowFlags()|Qt::FramelessWindowHint);
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);

    m_frameless.activateOn(this);
    m_frameless.setWidgetMovable(true);
    m_frameless.setWidgetResizable(true);


    QRect rcTitleWidgetContainer = ui->widgetTitle->rect();
    QPoint ptLeftTop(0, 0);
    ptLeftTop = ui->widgetTitle->mapTo(this, ptLeftTop);
    // ptLeftTop += QPoint(0, 24);

    QSize nSize = rcTitleWidgetContainer.size();
    m_frameless.setRectRubberBandOnMove(QRect(ptLeftTop, nSize));

    // m_frameless.setRectRubberBandOnMove(ui->widgetTitle->rect());
}


void ShadowWidget::resizeEvent(QResizeEvent *ev)
{
    ui->background->setGeometry(this->rect());


    QRect rcTitleWidgetContainer = ui->widgetTitle->rect();
    QPoint ptLeftTop(0, 0);
    ptLeftTop = ui->widgetTitle->mapTo(this, ptLeftTop);
    ptLeftTop += QPoint(0, 24);

    QSize nSize = rcTitleWidgetContainer.size();
    m_frameless.setRectRubberBandOnMove(QRect(ptLeftTop, nSize));

    QWidget::resizeEvent(ev);

}
