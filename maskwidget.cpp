#include "maskwidget.h"
#include "anchormanager.h"
#include <QPainter>
#include <QEvent>
#include <QDebug>

QT_BEGIN_NAMESPACE
void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);
QT_END_NAMESPACE
MaskWidget::MaskWidget(QWidget *parent)
    : BaseWidget(parent)
{
    // setAttribute(Qt::WA_TranslucentBackground);
    // setVisible(false);
}

void MaskWidget::setSource(QImage source)
{
    m_source = source;
    qt_blurImage(m_source, 25, true, false);
    update();
}

void MaskWidget::paintEvent(QPaintEvent *event)
{
    if (!m_source.isNull()) {
        QPainter painter(this);
        hightQualityPainter(painter);
        painter.drawImage(rect(), m_source);
        setMask(m_mask);
    }
}

void MaskWidget::updateMaskImage()
{
    if (m_sourceWidget) {
        QPixmap pixmap(m_sourceWidget->size());
        pixmap.fill(Qt::transparent);
        m_sourceWidget->render(&pixmap, QPoint(), QRegion(), QWidget::DrawChildren);
        m_mask = pixmap.mask();
        setSource(pixmap.toImage());
    }
}

bool MaskWidget::event(QEvent *event)
{
    if (event->type() == QEvent::Resize) {
        static QSize s_oldSize;
        if (m_sourceWidget && s_oldSize != size()) {
            s_oldSize = size();
            updateMaskImage();
        }
    }
    return BaseWidget::event(event);
}

QWidget *MaskWidget::sourceWidget() const
{
    return m_sourceWidget;
}

void MaskWidget::setSourceWidget(QWidget *newSourceWidget)
{
    if (newSourceWidget == nullptr || m_sourceWidget == newSourceWidget)
        return;
    m_sourceWidget = newSourceWidget;
    AnchorManager::instance()->removeAnchors(this);
    AnchorManager::instance()->installAnchor(this, std::map<AnchorManager::AnchorType,
                                                            AnchorManager::AnchorType>({
                                                                                        {AnchorManager::AnchorFill, AnchorManager::AnchorFill},
                                                                                        }), QMargins(0, 0, 0, 0), m_sourceWidget, false);

    updateMaskImage();
    emit sourceWidgetChanged();
}
