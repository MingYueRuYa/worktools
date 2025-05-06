#ifndef SHADOWWIDGET_H
#define SHADOWWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui{
class Form;
}

QT_END_NAMESPACE

class ShadowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShadowWidget(QWidget *parent = nullptr);

protected:
    virtual void resizeEvent(QResizeEvent *ev);

signals:

private:
    Ui::Form *ui;
};

#endif // SHADOWWIDGET_H
