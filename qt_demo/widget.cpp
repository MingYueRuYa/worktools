#include "widget.h"
#include "ui_sheet_style_demo.h"

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

