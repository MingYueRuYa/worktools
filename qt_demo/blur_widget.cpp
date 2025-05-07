#include "blur_widget.h"
#include <QPainter>

Q_WIDGETS_EXPORT void qt_blurImage(QPainter* p, QImage& blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);

BlurWidget::BlurWidget(QWidget *parent) :
QWidget(parent)
{
	m_pWidgetBlur = NULL;
	setBlurRadius();
}

BlurWidget::~BlurWidget()
{
}

void BlurWidget::Show()
{
    this->show();
    this->raise();
}

void BlurWidget::Hide()
{
    this->hide();
    this->lower();
}

void BlurWidget::paintEvent(QPaintEvent * ev)
{
	if (!isVisible())
	{
		return;
	}

    if (m_blur_image.isNull())
	{
		__super::paintEvent(ev);

		return;
	}

	QPainter painter(this);
	if (!m_blur_image.isNull())
	{
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		painter.drawImage(m_blur_rect, m_blur_image);
	}
}

bool BlurWidget::setBlurWidget(QWidget * pWidget)
{
	m_pWidgetBlur = pWidget;

	if (!isVisible())
	{
		return true;
	}

	return updateBlurImage();
}

bool BlurWidget::updateBlurImage()
{
	if (m_pWidgetBlur==NULL)
	{
		if (!m_blur_image.isNull())
		{
			m_blur_image = QImage();
		}
		return true;
	}

	if (!topLevelWidget()->isVisible())
	{
		return true;
	}

    std::unique_lock<std::mutex> lock(m_MutexPaint);

	QWidget * pWidget = m_pWidgetBlur;
	QRect blur_rect = pWidget->rect();

    m_blur_rect = blur_rect;
	m_blur_image = QImage(m_blur_rect.width(), m_blur_rect.height(), QImage::Format_ARGB32_Premultiplied);

	QPainter painter;
	if (!painter.begin(&m_blur_image))
	{
		return false;
	}

	pWidget->render(&painter, QPoint(), QRegion(), RenderFlags(IgnoreMask));
	painter.end();

	qt_blurImage(NULL, m_blur_image, m_BlurRadius, false, false);
	update();
	return true;
}

void BlurWidget::updateImage(const QImage& image)
{
    std::unique_lock<std::mutex> lock(m_MutexPaint);
    m_blur_rect = this->rect();
    m_blur_image = image.copy();
    qt_blurImage(NULL, m_blur_image, m_BlurRadius, true, false);
    update();
}

bool BlurWidget::setBlurRadius(float radius/* = 20.f*/)
{
	m_BlurRadius = radius;
	if (!isVisible())
	{
		return true;
	}

	return updateBlurImage();
}
