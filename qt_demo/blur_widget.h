#ifndef BLURWIDGET_H
#define BLURWIDGET_H

#include <QWidget>
#include <mutex>


class BlurWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BlurWidget(QWidget *parent = 0);
    ~BlurWidget();
    void Show();
    void Hide();

public:
    bool setBlurWidget(QWidget * pWidget);
    bool setBlurRadius(float radius = 20.f);
    void setColor_Foreground(QColor color);

    bool updateBlurImage();
    void updateImage(const QImage &image);

protected:
    virtual void paintEvent(QPaintEvent *);

protected:
    QImage m_blur_image;
    QRect m_blur_rect;
    QWidget * m_pWidgetBlur;
    float m_BlurRadius;
    std::mutex m_MutexPaint;
};

#endif // BLURWIDGET
