#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui { class Form; }

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Form *ui;
};
#endif // WIDGET_H
