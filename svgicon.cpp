#include "svgicon.h"

#include <QDebug>
#include <QPainter>
#include <QSvgRenderer>
#include <QEasingCurve>
#include <QVariant>
#include <QColor>

#include "animatewidgets.h"

SvgIcon::SvgIcon(QWidget *parent)
    : BaseWidget{parent}, m_renderer(new QSvgRenderer(this))
{
}

SvgIcon::~SvgIcon()
{

}

void SvgIcon::paintEvent(QPaintEvent *event)
{
    if (m_renderer->isValid()) {
        auto palette = this->palette();
        auto r = rect();
        auto margins = this->margins();
        r.adjust(margins.left(), margins.top(), -margins.right(), -margins.bottom());
        if (m_svgImage.isNull() || m_needUpdateSvgImage) {
            m_needUpdateSvgImage = false;
            m_svgImage = QImage(rect().size() * devicePixelRatio(), QImage::Format_ARGB32_Premultiplied);
            m_svgImage.fill(Qt::transparent);
        }
        QPainter painter(&m_svgImage);
        hightQualityPainter(painter);
        m_renderer->render(&painter);
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.setBrush(palette.text());
        painter.setPen(Qt::NoPen);
        painter.drawRect(m_svgImage.rect());
        QPainter p(this);
        p.setOpacity(opacity());
        hightQualityPainter(p);
        QPointF center = QPointF(r.width() / 2., r.height() / 2.);
        p.translate(center + QPointF(margins.left(), margins.top()));
        p.rotate(rotation());
        p.drawImage(QRectF(-center, r.size()), m_svgImage);
    }
}

QString SvgIcon::name() const
{
    return m_name;
}

void SvgIcon::setName(const QString &newName)
{
    if (m_name == newName)
        return;
    m_name = newName;
    if (m_renderer->load(QBootstrap::iconPrefix.arg(m_name))) {
        m_needUpdateSvgImage = true;
        update();
    } else {
        qWarning() << "Load svg failed, " << m_name;
    }
    emit nameChanged();
}


void SvgIcon::resizeEvent(QResizeEvent *event)
{
    m_needUpdateSvgImage = true;
    update();
}
