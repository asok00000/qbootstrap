#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#include <QWidget>

#include "qbootstrap_global.h"

class QBOOTSTRAP_EXPORT BaseWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation NOTIFY rotationChanged FINAL)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged FINAL)
    Q_PROPERTY(QRect margin READ margin WRITE setMargin NOTIFY marginChanged FINAL)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged FINAL)
    Q_PROPERTY(bool dragable READ dragable WRITE setDragable NOTIFY dragableChanged FINAL)
    Q_PROPERTY(bool mousePressed READ mousePressed WRITE setMousePressed NOTIFY mousePressedChanged FINAL)
    Q_PROPERTY(bool animationEnabled READ animationEnabled WRITE setAnimationEnabled NOTIFY animationEnabledChanged FINAL)
public:
    explicit BaseWidget(QWidget *parent = nullptr);

    qreal rotation() const;
    void setRotation(qreal newRotation);

    QColor color() const;
    void setColor(const QColor &newColor);

    QRect margin() const;
    QMargins margins();
    void setMargin(const QRect &newMargin);
    static void hightQualityPainter(QPainter &painter);
    qreal opacity() const;
    void setOpacity(qreal newOpacity);

    bool dragable() const;
    void setDragable(bool newDragable);

    bool mousePressed() const;
    void setMousePressed(bool newMousePressed);

    bool animationEnabled() const;
    void setAnimationEnabled(bool newAnimationEnabled);

signals:
    void rotationChanged();
    void colorChanged();
    void marginChanged();

    void opacityChanged();

    void dragableChanged();

    void mousePressedChanged();

    void animationEnabledChanged();

private:
    qreal m_rotation = .0;
    QColor m_color = Qt::black;
    //x, y, w, h-> l, t, r, b
    QMargins m_margin = QMargins(0, 0, 0, 0);
    qreal m_opacity = 1.;
    bool m_dragable = false;
    bool m_mousePressed = false;
    QPointF m_mousePos = QPointF(-1, -1);

    bool m_animationEnabled = false;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // BASEWIDGET_H
