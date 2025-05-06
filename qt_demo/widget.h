#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui { class DemoWnd; }

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButtonShadow_clicked();

private:
    Ui::DemoWnd *ui;
};
#endif // WIDGET_H
