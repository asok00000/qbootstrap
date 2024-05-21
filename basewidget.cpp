#include "basewidget.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include "animatewidgets.h"
BaseWidget::BaseWidget(QWidget *parent)
    : QWidget{parent}
{
    setMouseTracking(true);
    setAttribute(Qt::WA_StyledBackground);
}

qreal BaseWidget::rotation() const
{
    return m_rotation;
}

void BaseWidget::setRotation(qreal newRotation)
{
    if (qFuzzyCompare(m_rotation, newRotation))
        return;
    m_rotation = newRotation;
    update();
    emit rotationChanged();
}

QColor BaseWidget::color() const
{
    return m_color;
}

void BaseWidget::setColor(const QColor &newColor)
{
    if (m_color == newColor)
        return;
    m_color = newColor;
    update();
    emit colorChanged();
}

QRect BaseWidget::margin() const
{
    return QRect(m_margin.left(), m_margin.top(), m_margin.right(), m_margin.bottom());
}

QMargins BaseWidget::margins()
{
    return m_margin;
}

void BaseWidget::setMargin(const QRect &newMargin)
{
    auto margin = QMargins(newMargin.x(), newMargin.y(), newMargin.width(), newMargin.height());
    if (m_margin == margin)
        return;
    m_margin = margin;
    emit marginChanged();
}

void BaseWidget::hightQualityPainter(QPainter &painter)
{
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::LosslessImageRendering);
}

qreal BaseWidget::opacity() const
{
    return m_opacity;
}

void BaseWidget::setOpacity(qreal newOpacity)
{
    if (qFuzzyCompare(m_opacity, newOpacity))
        return;
    m_opacity = newOpacity;
    emit opacityChanged();
}

bool BaseWidget::dragable() const
{
    return m_dragable;
}

void BaseWidget::setDragable(bool newDragable)
{
    if (m_dragable == newDragable)
        return;
    m_dragable = newDragable;
    emit dragableChanged();
}

void BaseWidget::mousePressEvent(QMouseEvent *event)
{
    setMousePressed(true);
    m_mousePos = event->globalPosition() - frameGeometry().topLeft();
    QWidget::mousePressEvent(event);
}

void BaseWidget::mouseReleaseEvent(QMouseEvent *event)
{
    setMousePressed(false);
    m_mousePos = QPoint(-1, -1);
    QWidget::mouseReleaseEvent(event);
}

void BaseWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragable && m_mousePressed) {
        move((event->globalPosition() - m_mousePos).toPoint());
    }
    QWidget::mouseMoveEvent(event);
}

bool BaseWidget::mousePressed() const
{
    return m_mousePressed;
}

void BaseWidget::setMousePressed(bool newMousePressed)
{
    if (m_mousePressed == newMousePressed)
        return;
    m_mousePressed = newMousePressed;
    emit mousePressedChanged();
}

bool BaseWidget::animationEnabled() const
{
    return m_animationEnabled;
}

void BaseWidget::setAnimationEnabled(bool newAnimationEnabled)
{
    if (m_animationEnabled == newAnimationEnabled)
        return;
    m_animationEnabled = newAnimationEnabled;
    if (m_animationEnabled) {
        AnimateWidgets::instance()->enableQssAnimation(this);

    } else {
        AnimateWidgets::instance()->disableQssAnimation(this);
    }
    emit animationEnabledChanged();
}
