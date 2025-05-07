#ifndef SHADOWWIDGET_H
#define SHADOWWIDGET_H

#include "NcFrameLessHelper.h"

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
    NcFramelessHelper m_frameless;
};

#endif // SHADOWWIDGET_H
