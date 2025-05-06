#include "widget.h"
#include "ui_sheet_style_demo.h"

#include <QWidget>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form)
{
    ui->setupUi(this);

    QLabel *tab1=new QLabel("tab1", this);
     QLabel *tab2=new QLabel("tab2", this);
    // 添加按钮，设置向上
    ui->tabWidget->tabBar()->setTabButton(0, QTabBar::ButtonPosition::LeftSide, tab1);
    ui->tabWidget->tabBar()->setTabButton(1, QTabBar::ButtonPosition::RightSide, tab2);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButtonShadow_clicked()
{
    QWidget *widget = new QWidget();
    QString strQss = QString("QWidget{border-width: 24px; border-image: url(\":resource/shadow.png\") 24 24 24 24 stretch stretch;}");
    if (strQss != widget->styleSheet())
    {
        widget->setStyleSheet(strQss);
    }
    widget->setWindowFlags(widget->windowFlags()|Qt::FramelessWindowHint);
    widget->show();
}

